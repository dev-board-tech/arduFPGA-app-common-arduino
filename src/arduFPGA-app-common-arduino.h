#ifndef __ARDUFPGA_COMMON_H__
#define __ARDUFPGA_COMMON_H__

#define EXCLUDE_AVR_ASM_COMPILER	true

#if defined (__AVR_MEGA__) && defined(USE_TEXT_EDITOR)
#include "app/textEditor.h"
#endif
#include "buffers/ring_buff.h"
#include "convert/base58.h"
#include "crypto/aes256.h"
#include "crypto/ecc.h"
#include "crypto/sha256.h"
#if !defined(QT_WIDGETS_LIB)
#include "device/25flash.h"
#endif
#include "device/hKbd_lrudab.h"
#if !defined(QT_WIDGETS_LIB)
#include "device/mmc_sd_spi.h"
#include "device/ssd1331_spi.h"
#endif
#include "device/screen.h"
#include "device/ssd1306_spi.h"
#include "device/vKbd.h"
#include "device/vKbdLineSimple.h"
#include "device/vKbdSimple.h"
#if !defined(QT_WIDGETS_LIB)
#include "device/wpdw21_spi.h"
#include "device/st7735_spi.h"
#include "device/st7789_spi.h"
#endif
#include "device/vs10xx.h"
#include "gfx/logo.h"
#include "include/global.h"
#if defined(__AVR_MEGA__) || defined(QT_WIDGETS_LIB)
#include "lib/fs/fat.h"
#endif
#include "sys/sTimer.h"
#include "util/math.h"
#include "util/unions.h"
#include "util/util-fat.h"
#include "util/util.h"
#ifndef _ESPAsyncWebServer_H_
#if defined(ESP8266) || defined(ESP32)
#include "web/webBrowser.h"
#endif
#endif


#endif
