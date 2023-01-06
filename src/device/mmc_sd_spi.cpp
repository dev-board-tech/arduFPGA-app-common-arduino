/*
 * MMC/SD driver ffile for arduFPGA designs.
 * 
 * Copyright (C) 2020  Iulian Gheorghiu (morgoth@devboard.tech)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "mmc_sd_spi.h"
#include "../lib/fs/fat/inc/diskio.h"
#if __AVR_MEGA__
#include <avr/interrupt.h>
#endif

//#######################################################################################
inline uint8_t ioData(mmcSd_t *inst, uint8_t value) {
	if (!inst->spiInst)
		return 0xFF;
	return inst->spiInst->transfer(value);
}
//#######################################################################################
static inline void csAssert(mmcSd_t *inst) {
	digitalWrite(inst->csPin, 0);
}
//#######################################################################################
static inline void csDeassert(mmcSd_t *inst) {
	digitalWrite(inst->csPin, 1);
}
void spiClkChange(mmcSd_t *inst, uint32_t clock)
  //__attribute__((__always_inline__))
{
  // Clock settings are defined as follows. Note that this shows SPI2X
  // inverted, so the bits form increasing numbers. Also note that
  // fosc/64 appears twice
  // SPR1 SPR0 ~SPI2X Freq
  //   0    0     0   fosc/2
  //   0    0     1   fosc/4
  //   0    1     0   fosc/8
  //   0    1     1   fosc/16
  //   1    0     0   fosc/32
  //   1    0     1   fosc/64
  //   1    1     0   fosc/64
  //   1    1     1   fosc/128

  // We find the fastest clock that is less than or equal to the
  // given clock rate. The clock divider that results in clock_setting
  // is 2 ^^ (clock_div + 1). If nothing is slow enough, we'll use the
  // slowest (128 == 2 ^^ 7, so clock_div = 6).
  uint8_t clockDiv;

  // When the clock is known at compile time, use this if-then-else
  // cascade, which the compiler knows how to completely optimize
  // away. When clock is not known, use a loop instead, which generates
  // shorter code.
  if (__builtin_constant_p(clock)) {
    if (clock >= F_CPU / 2) {
      clockDiv = 0;
    } else if (clock >= F_CPU / 4) {
      clockDiv = 1;
    } else if (clock >= F_CPU / 8) {
      clockDiv = 2;
    } else if (clock >= F_CPU / 16) {
      clockDiv = 3;
    } else if (clock >= F_CPU / 32) {
      clockDiv = 4;
    } else if (clock >= F_CPU / 64) {
      clockDiv = 5;
    } else {
      clockDiv = 6;
    }
  } else {
    uint32_t clockSetting = F_CPU / 2;
    clockDiv = 0;
    while (clockDiv < 6 && clock < clockSetting) {
      clockSetting /= 2;
      clockDiv++;
    }
  }

  // Compensate for the duplicate fosc/64
  if (clockDiv == 6)
	  clockDiv = 7;

  // Invert the SPI2X bit
  clockDiv ^= 0x1;

  // Pack into the SPISettings class
  inst->spiInst->setClockDivider(clockDiv);
//  spcr = _BV(SPE) | _BV(MSTR) | ((bitOrder == LSBFIRST) ? _BV(DORD) : 0) |
//    (dataMode & SPI_MODE_MASK) | ((clockDiv >> 1) & SPI_CLOCK_MASK);
//  spsr = clockDiv & SPI_2XCLOCK_MASK;
}
/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/
int waitReady (    /* 1:Ready, 0:Timeout */
mmcSd_t *inst,
unsigned int wt         /* Timeout [ms] */
)
{
	unsigned char d;

	volatile unsigned int Timer2 = wt;
	do {
		d = ioData(inst, 0xFF);
		/* This loop takes a time. Insert rot_rdq() here for multitask envilonment. */
		//sd_delay(1);
	} while (d != 0xFF && Timer2--);    /* Wait for card goes ready or timeout */

	return (d == 0xFF) ? 1 : 0;
}
/*-----------------------------------------------------------------------*/
/* Deselect card and release SPI                                         */
/*-----------------------------------------------------------------------*/
void deselect (mmcSd_t *inst)
{
	csDeassert(inst);      /* CS = H */
	ioData(inst, 0xFF);    /* Dummy clock (force DO hi-z for multiple slave SPI) */
}
/*-----------------------------------------------------------------------*/
/* Select card and wait for ready                                        */
/*-----------------------------------------------------------------------*/
int select(mmcSd_t *inst)    /* 1:OK, 0:Timeout */
{
	csAssert(inst);
	ioData(inst, 0xFF);    /* Dummy clock (force DO enabled) */

	if (waitReady(inst, 50000))
		return 1;   /* OK */
	deselect(inst);
		return 0;   /* Timeout */
}
/*-----------------------------------------------------------------------*/
/* Send a command packet to the MMC                                      */
/*-----------------------------------------------------------------------*/
unsigned char sendCmd (     /* Return value: R1 resp (bit7==1:Failed to send) */
mmcSd_t *inst,
unsigned char cmd,       /* Command index */
unsigned long arg       /* Argument */
)
{
	unsigned char n, res;


	if (cmd & 0x80)
	{   /* Send a CMD55 prior to ACMD<n> */
		cmd &= 0x7F;
		res = sendCmd(inst, CMD55, 0);
		if (res > 1)
		return res;
	}

	/* Select card */
	if (cmd != CMD12)
	{
		deselect(inst);
		if (!select(inst))
		return 0xFF;
	}

	//if (cmd == CMD17)
	//sendCmd(CMD59, 0);
	/* Send command packet */
	ioData(inst, 0x40 | cmd);              /* Start + command index */
	ioData(inst, (unsigned char)(arg >> 24));       /* Argument[31..24] */
	ioData(inst, (unsigned char)(arg >> 16));       /* Argument[23..16] */
	ioData(inst, (unsigned char)(arg >> 8));            /* Argument[15..8] */
	ioData(inst, (unsigned char)arg);               /* Argument[7..0] */
	n = 0x01;                           /* Dummy CRC + Stop */
	if (cmd == CMD0)
		n = 0x95;           /* Valid CRC for CMD0(0) */
	if (cmd == CMD8)
		n = 0x87;           /* Valid CRC for CMD8(0x1AA) */
	ioData(inst, n);

	/* Receive command resp */
	if (cmd == CMD12)
	ioData(inst, 0xFF);    /* Diacard following one byte when CMD12 */
	n = 255;                                /* Wait for response (10 bytes max) */
	do {
		res = ioData(inst, 0xFF);
	}while ((res & 0x80) && --n);

	return res;
	/* Return received response */
}
//#######################################################################################
bool initCard(mmcSd_t *inst)
{
	//spiClkChange(inst, 400000);
	csDeassert(inst);
	unsigned char cmd, ty, ocr[4], csd[16];
	unsigned short  n;
	inst->sdType = IsSd;
	for (n = 10; n; n--)
		ioData(inst, 0xFF);    /* Send 80 dummy clocks */

	ty = 0;
	if (sendCmd(inst, CMD0, 0) == 1)
	{           /* Put the card SPI/Idle state */
		unsigned int Timer1 = 1000;                     /* Initialization timeout = 1 sec */
		if (sendCmd(inst, CMD8, 0x1AA) == 1) {   /* SDv2? */
			for (n = 0; n < 4; n++) ocr[n] = ioData(inst, 0xFF);   /* Get 32 bit return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {               /* Is the card supports vcc of 2.7-3.6V? */
				while (Timer1-- && sendCmd(inst, ACMD41, 1UL << 30)) ;    /* Wait for end of initialization with ACMD41(HCS) */
				if (Timer1 && sendCmd(inst, CMD58, 0) == 0) {       /* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++)
						ocr[n] = ioData(inst, 0xFF);
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;  /* Card id SDv2 */
					if(ty & CT_BLOCK)
						inst->sdType = IsSdhc;
				}
			}
		} else {    /* Not SDv2 card */
			if (sendCmd(inst, ACMD41, 0) <= 1) {   /* SDv1 or MMC? */
				ty = CT_SD1; cmd = ACMD41;  /* SDv1 (ACMD41(0)) */
			} else {
				ty = CT_MMC; cmd = CMD1;    /* MMCv3 (CMD1(0)) */
			}
			while (Timer1-- && sendCmd(inst, cmd, 0)) ;       /* Wait for end of initialization */
			if (!Timer1 || sendCmd(inst, CMD16, 512) != 0)    /* Set block length: 512 */
				ty = 0;
		}
	}
	if (sendCmd(inst, CMD59, 0) == 0) {

	}

	/*if (sendCmd(CMD10, 0) == 0)
	{
	for (n = 0; n < 32; n++) cid[n] = ioData(0xFF);
	memcpy(raw_cid, cid, 16);
	}*/
	if (ty)
	{           /* OK */
		//spiClkChange(inst, 8000000);
		if (sendCmd(inst, CMD9, 0) == 0)
		{
			unsigned int wait = 0, response = 0;
			while (++wait <= 0x1ff && response != 0xfe)
			response = ioData(inst, 0xff);
			if (wait >= 0x1ff)
			{
				inst->SD_Init_OK = false;
				deselect(inst);
				return inst->SD_Init_OK;
			}
			for (n = 0; n < 16; n++)
				csd[n] = ioData(inst, 0xFF);
			ioData(inst, 0xFF);
			ioData(inst, 0xFF);
			inst->raw_csd[0] = ((unsigned long)csd[15] + ((unsigned long)csd[14] << 8) +  ((unsigned long)csd[13] << 16) +  ((unsigned long)csd[12] << 24));
			inst->raw_csd[1] = ((unsigned long)csd[11] + ((unsigned long)csd[10] << 8) +  ((unsigned long)csd[9] << 16) +  ((unsigned long)csd[8] << 24));
			inst->raw_csd[2] = ((unsigned long)csd[7] + ((unsigned long)csd[6] << 8) +  ((unsigned long)csd[5] << 16) +  ((unsigned long)csd[4] << 24));
			inst->raw_csd[3] = ((unsigned long)csd[3] + ((unsigned long)csd[2] << 8) +  ((unsigned long)csd[1] << 16) +  ((unsigned long)csd[0] << 24));
				//memcpy(raw_csd, csd, 16);
		}
		if (inst->sdType == IsSdhc)
		{
			inst->tranSpeed = SD_CARD1_TRANSPEED(inst);
			inst->blkLen = 1 << (SD_CARD1_RDBLKLEN(inst));
			inst->size = SD_CARD1_SIZE(inst);
			inst->nBlks = inst->size >> 9;// blkLen;
		}
		else
		{
			inst->tranSpeed = SD_CARD0_TRANSPEED(inst);
			inst->blkLen = 1 << (SD_CARD0_RDBLKLEN(inst));
			inst->nBlks = SD_CARD0_NUMBLK(inst);
			inst->size = SD_CARD0_SIZE(inst);
		}
		inst->SD_Init_OK = true;
	} else
	{           /* Failed */
	//power_off();
		inst->SD_Init_OK = false;
	}

	deselect(inst);
	//spiClkChange(inst, 8000000);
	return inst->SD_Init_OK;
}
//#######################################################################################
bool rcvrDatablock(mmcSd_t *inst, unsigned char *buff, unsigned int bytes_to_read)
{
	unsigned char token;
	unsigned long Timer1 = 50000;
	do {                           /* Wait for data packet in timeout of 100ms */
		token = ioData(inst, 255);
	} while ((token == 0xFF) && Timer1--);
	if(token != MMC_DATA_TOKEN)
		return false;   /* If not valid data token, return with error */
	do {                           // Receive the data block into buffer
		*buff++ = ioData(inst, 255);
	} while (--bytes_to_read);
	ioData(inst, 255);                      /* Discard CRC */
	ioData(inst, 255);
	return true;                    /* Return with success */
}
//#######################################################################################
unsigned int readPage(mmcSd_t *inst, void* _Buffer, unsigned long block, unsigned int nblks)
{
	if (!nblks)
		return false;       /* Check parameter */
	if (inst->SD_Init_OK == false)
		return false;   /* Check if drive is ready */
	if (inst->sdType == IsSd)
		block *= 512;   /* LBA ot BA conversion (byte addressing cards) */
	if (nblks == 1) {   /* Single sector read */
		if ((sendCmd(inst, CMD17, block) == 0)    /* READ_SINGLE_BLOCK */
			&& rcvrDatablock(inst, (unsigned char *)_Buffer, 512))
		nblks = 0;
	}
	else {               /* Multiple sector read */
		unsigned char* Buffer = (unsigned char*)_Buffer;
		if (sendCmd(inst, CMD18, block) == 0) {   /* READ_MULTIPLE_BLOCK */
			do {
				if (!rcvrDatablock(inst, Buffer, 512))
					break;
				Buffer += 512;
			} while (--nblks);
			sendCmd(inst, CMD12, 0);              /* STOP_TRANSMISSION */
		}
	}
	deselect(inst);
	return nblks ? false : true;    /* Return result */
}
//#######################################################################################
unsigned int mmcSdSpiRead(void *handler, void* _Buffer, unsigned long _block, unsigned int nblks)
{
	cli();
	if(!handler) {
		sei();
		return RES_ERROR;
	}
	mmcSd_t *inst = (mmcSd_t *)handler;
#ifdef uSD_LED_PORT
		uSD_LED_PORT |= uSD_LED_PIN;
#endif
	unsigned long block = _block;
	unsigned char* Buffer = (unsigned char*)_Buffer;
	do {
		if(!readPage(inst, Buffer, block++, 1)) {
#ifdef MMC_SD_LED_PORT
			if(uSD_LED_PORT)
				uSD_LED_PORT &= ~uSD_LED_PIN;
#endif
			sei();
			return RES_ERROR;
		}
		Buffer += 512;
	} while (--nblks);
#ifdef uSD_LED_PORT
	if(uSD_LED_PORT)
		uSD_LED_PORT &= ~uSD_LED_PIN;
#endif
	sei();
	return RES_OK;
}
//#######################################################################################
/* Send multiple byte */
void spiMulti (
mmcSd_t *inst,
const unsigned char *buff,   /* Pointer to the data */
unsigned long btx            /* Number of bytes to send (512) */
)
{
	do {                        // Transmit data block
		ioData(inst, *buff++);
	} while (btx -= 1);
}
/*-----------------------------------------------------------------------*/
/* Send a data packet to the MMC                                         */
/*-----------------------------------------------------------------------*/

int xmitDatablock (    /* 1:OK, 0:Failed */
mmcSd_t *inst,
const unsigned char *buff,   /* Ponter to 512 byte data to be sent */
unsigned char token          /* Token */
)
{
	unsigned char resp;

	if (!waitReady(inst, 50000))
		return 0;       /* Wait for card ready */

	ioData(inst, token);                   /* Send token */
	if (token != 0xFD) {                /* Send data if token is other than StopTran */
		spiMulti(inst, buff, 512);       /* Data */
		//unsigned short crc = mmcsd_crc16(buff, 512);
		ioData(inst, 0xFF);
		ioData(inst, 0xFF);    /* Dummy CRC */

		resp = ioData(inst, 0xFF);             /* Receive data resp */
		if ((resp & 0x1F) != 0x05)      /* Function fails if the data packet was not accepted */
			return 0;
	}
	return 1;
}


unsigned int writePage(mmcSd_t *inst, void* _Buffer, unsigned long block, unsigned int nblks)
{
	if (!nblks)
		return false;       /* Check parameter */
	if (inst->SD_Init_OK == false)
		return false;   /* Check if drive is ready */

	if (inst->sdType == IsSd)
		block *= 512;   /* LBA ot BA conversion (byte addressing cards) */

	if (nblks == 1) {   /* Single sector write */
		if ((sendCmd(inst, CMD24, block) == 0)    /* WRITE_BLOCK */
		&& xmitDatablock(inst, (const unsigned char *)_Buffer, 0xFE))
		nblks = 0;
	}
	else {              /* Multiple sector write */
	if (inst->sdType == IsSd)
		sendCmd(inst, ACMD23, nblks); /* Predefine number of sectors */
	if (sendCmd(inst, CMD25, block) == 0) {   /* WRITE_MULTIPLE_BLOCK */
		unsigned char* Buffer = (unsigned char*)_Buffer;
			do {
				if (!xmitDatablock(inst, Buffer, 0xFC))
					break;
				Buffer += 512;
			} while (--nblks);
		if (!xmitDatablock(inst, 0, 0xFD))    /* STOP_TRAN token */
			nblks = 1;
		}
	}
	deselect(inst);
	return nblks ? false : true;    /* Return result */
}
//#######################################################################################
unsigned int mmcSdSpiWrite(void *handler, void* _Buffer, unsigned long _block, unsigned int nblks)
{
	cli();
	if(!handler) {
		sei();
		return RES_ERROR;
	}
	mmcSd_t *inst = (mmcSd_t *)handler;
#ifdef uSD_LED_PORT
	if(uSD_LED_PORT)
		uSD_LED_PORT |= uSD_LED_PIN;
#endif
	unsigned long block = _block;
	unsigned char* Buffer = (unsigned char*)_Buffer;
	do {
		if(!writePage(inst, Buffer, block++, 1)) {
#ifdef uSD_LED_PORT
			if(uSD_LED_PORT)
				uSD_LED_PORT &= ~uSD_LED_PIN;
#endif
			sei();
			return RES_ERROR;
		}
		Buffer += 512;
	} while (--nblks);
#ifdef uSD_LED_PORT
	if(uSD_LED_PORT)
		uSD_LED_PORT &= ~uSD_LED_PIN;
#endif
	sei();
	return RES_OK;
}
//#######################################################################################
void mmcSdSpiIoctl(void *handler, unsigned int  command,  unsigned int *buffer)
{
	if(!handler)
		return;
	mmcSd_t *inst = (mmcSd_t *)handler;
	switch(command) {
		case GET_SECTOR_COUNT:
			*buffer = inst->nBlks;
			break;
		case GET_SECTOR_SIZE:
			*buffer = inst->blkLen;
			break;
		case CTRL_SYNC :        /* Make sure that no pending write process */
			select(inst);
			if (!waitReady(inst, 5000))
			return;
		default:
			*buffer = 0;
			break;
	}
	return;
}
//#######################################################################################
bool mmc_sd_connected(void *handler) {
	mmcSd_t *inst = (mmcSd_t *)handler;
	if (inst->cdPin != -1)
		return digitalRead(inst->cdPin) ? false : true;
	else
		return true;
}
//#######################################################################################
bool mmcSdSpiIdle(mmcSd_t *inst) {
	if(!inst)
		return inst->fs_mounted;
	if(inst->unitNr > 2)
		return inst->fs_mounted;
	if(mmc_sd_connected((void *)inst)) {
		if(inst->initFlg) {
#ifdef MMC_SD_LED_DIR
			uSD_LED_DIR |= uSD_LED_PIN;
			uSD_LED_PORT &= ~uSD_LED_PIN;
#endif
			inst->initFlg = 0;
			delay(400);
			initCard(inst);
			if(initCard(inst)) {
				inst->connected = true;
				inst->fatFs->drv_rw_func.DriveStruct = (void *)inst;
				inst->fatFs->drv_rw_func.drv_connected = mmc_sd_connected;
				inst->fatFs->drv_rw_func.drv_r_func = mmcSdSpiRead;
				inst->fatFs->drv_rw_func.drv_w_func = mmcSdSpiWrite;
				inst->fatFs->drv_rw_func.drv_ioctl_func = mmcSdSpiIoctl;
#if (_FFCONF == 82786)
				char drv_name_buff[4];
				if(!f_mount(0 + inst->unitNr, &g_s_mmcFatFs))
#else
				char drv_name_buff[7];
				//drv_name_buff[0] = 'u';
				//drv_name_buff[1] = 'S';
				//drv_name_buff[2] = 'D';
				//drv_name_buff[3] = '1' + inst->unitNr;
				drv_name_buff[0] = '0';
				drv_name_buff[1] = ':';
				drv_name_buff[2] = '\0';
				if(!f_mount(inst->fatFs, drv_name_buff, 1))
#endif
				{
					delay(4000);
#if (_FFCONF == 82786)
					drv_name_buff[0] = '0' + inst->unitNr;
					drv_name_buff[1] = ':';
					drv_name_buff[2] = '/';
					drv_name_buff[3] = '\0';
#else
					drv_name_buff[2] = '/';
					drv_name_buff[3] = '\0';
#endif
					DIR g_sDirObject;
					if(f_opendir(&g_sDirObject, drv_name_buff) == FR_OK) {
						inst->fs_mounted = true;
					}
					//else     GI::IO::writeF((char *)CONSOLE_UART_OUT, (char *)"MMCSD_SPI%d ERROR oppening path\n\r" , inst->unitNr);
				}

				//else        GI::IO::writeF((char *)CONSOLE_UART_OUT, (char *)"MMCSD_SPI%d ERROR mounting disk\n\r" , inst->unitNr);
			}
			//else            GI::IO::writeF((char *)CONSOLE_UART_OUT, (char *)"MMCSD_SPI%d not detected\n\r" , inst->unitNr);
		}
	} else {
		//delay(1);
		if(inst->initFlg != 1) {
			inst->fs_mounted = false;
			inst->connected = false;
			inst->initFlg = 1;
			/*#ifdef MMCSD_DEBUG_EN
			GI::IO::writeF((char *)CONSOLE_UART_OUT, (char *)"MMCSD_SPI%d Disconnected\n\r" , inst->unitNr);
			#endif*/
		}
	}
	return inst->fs_mounted;
}
//#######################################################################################
