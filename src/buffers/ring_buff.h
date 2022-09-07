/*
 *  lib/buffers/ring_buff.h
 */
#ifndef __RING_BUFF_H__
#define __RING_BUFF_H__
//###########################################################################
#include <stdbool.h>
#include <stdint.h>
#ifdef _UTIL_ATOMIC_H_
#include <util/atomic.h>
#endif
#include "include/global.h"
//###########################################################################
typedef struct {
	uint32_t elemenmtsOnBuff;
	uint32_t elements;
	uint32_t elementSize;
	void* buff;
	uint32_t pushPtr;
	uint32_t popPtr;
}ringbuff;

void ringbuff_init_v_size(ringbuff *param, uint32_t elements, uint32_t elementSize);
void ringbuff_init(ringbuff *param, uint32_t elements);
void ringbuff_deinit(ringbuff *param);
bool ringbuff_push_byte(ringbuff *param, uint8_t data);
bool ringbuff_push_half(ringbuff *param, uint16_t data);
bool ringbuff_push_word(ringbuff *param, uint32_t data);
bool ringbuff_push_dword(ringbuff *param, uint64_t data);
int32_t ringbuff_push_buff(ringbuff *param, void *data, int32_t len);
bool ringbuff_pop_byte(ringbuff *param, uint8_t *data);
bool ringbuff_pop_half(ringbuff *param, uint16_t *data);
bool ringbuff_pop_word(ringbuff *param, uint32_t *data);
bool ringbuff_pop_dword(ringbuff *param, uint64_t *data);
int32_t ringbuff_pop_buff(ringbuff *param, uint8_t *buff, int32_t len);
SysErr ringbuff_reinit_v_size(ringbuff *param, uint32_t elements, uint32_t elementSize);
SysErr ringbuff_reinit(ringbuff *param, uint32_t elements);

/*#####################################################*/
#ifdef USE_VISUAL_STUDIO
#include "ring_buff.cpp"
#endif
/*#####################################################*/
#endif
