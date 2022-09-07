/*
 *  lib/buffers/ring_buff.cpp
 */
#ifndef __FiFo__
#define __FiFo__
//###########################################################################
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#ifdef _UTIL_ATOMIC_H_
#include <util/atomic.h>
#endif
#include "ring_buff.h"
//###########################################################################
void ringbuff_init_v_size(ringbuff *param, uint32_t elements, uint32_t elementSize) {
	if ((param->buff = (uint8_t*) malloc(elements * elementSize)) == 0)
		return;
	param->elements = elements;
	param->elementSize = elementSize;
}
//###########################################################################
void ringbuff_init(ringbuff *param, uint32_t elements) {
	if ((param->buff = (uint8_t*) malloc(elements)) == 0)
		return;
	param->elements = elements;
	param->elementSize = 1;
}
//###########################################################################
void ringbuff_deinit(ringbuff *param) {
	if(param && param->buff)
		free(param->buff);
}
//###########################################################################
bool ringbuff_push_byte(ringbuff *param, uint8_t data)
{
	if(param->elementSize != 1)
		return false;
	bool ret = false;
#ifdef _UTIL_ATOMIC_H_
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
#endif
	unsigned int BuffSize = param->elements;
	unsigned int BytesToBuff = param->elemenmtsOnBuff;
	if (BytesToBuff < BuffSize)
	{
		unsigned int PushPtr = param->pushPtr;
		((uint8_t *)param->buff)[param->pushPtr] = data;
		PushPtr++;
		if (PushPtr == BuffSize)
			PushPtr = 0;
		BytesToBuff++;
		param->pushPtr = PushPtr;
		param->elemenmtsOnBuff = BytesToBuff;
		ret = true;
	}
#ifdef _UTIL_ATOMIC_H_
}
#endif
	return ret;
}
//###########################################################################
bool ringbuff_push_half(ringbuff *param, uint16_t data)
{
	if(param->elementSize != 2)
		return false;
	bool ret = false;
#ifdef _UTIL_ATOMIC_H_
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
#endif
	unsigned int BuffSize = param->elements;
	unsigned int BytesToBuff = param->elemenmtsOnBuff;
	if (BytesToBuff < BuffSize)
	{
		unsigned int PushPtr = param->pushPtr;
		((uint16_t *)(param->buff))[param->pushPtr] = data;
		PushPtr++;
		if (PushPtr == BuffSize)
			PushPtr = 0;
		BytesToBuff++;
		param->pushPtr = PushPtr;
		param->elemenmtsOnBuff = BytesToBuff;
		ret = true;
	}
#ifdef _UTIL_ATOMIC_H_
}
#endif
	return ret;
}
//###########################################################################
bool ringbuff_push_word(ringbuff *param, uint32_t data)
{
	if(param->elementSize != 4)
		return false;
	bool ret = false;
#ifdef _UTIL_ATOMIC_H_
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
#endif
	unsigned int BuffSize = param->elements;
	unsigned int BytesToBuff = param->elemenmtsOnBuff;
	if (BytesToBuff < BuffSize)
	{
		unsigned int PushPtr = param->pushPtr;
		((uint32_t *)param->buff)[param->pushPtr] = data;
		PushPtr++;
		if (PushPtr == BuffSize)
			PushPtr = 0;
		BytesToBuff++;
		param->pushPtr = PushPtr;
		param->elemenmtsOnBuff = BytesToBuff;
		ret = true;
	}
#ifdef _UTIL_ATOMIC_H_
}
#endif
	return ret;
}
//###########################################################################
bool ringbuff_push_dword(ringbuff *param, uint64_t data)
{
	if(param->elementSize != 8)
		return false;
	bool ret = false;
#ifdef _UTIL_ATOMIC_H_
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
#endif
	unsigned int BuffSize = param->elements;
	unsigned int BytesToBuff = param->elemenmtsOnBuff;
	if (BytesToBuff < BuffSize)
	{
		unsigned int PushPtr = param->pushPtr;
		((uint64_t *)param->buff)[param->pushPtr] = data;
		PushPtr++;
		if (PushPtr == BuffSize)
			PushPtr = 0;
		BytesToBuff++;
		param->pushPtr = PushPtr;
		param->elemenmtsOnBuff = BytesToBuff;
		ret = true;
	}
#ifdef _UTIL_ATOMIC_H_
}
#endif
	return ret;
}
//###########################################################################
int32_t ringbuff_push_buff(ringbuff *param, void *buff, int32_t len)
{
	int32_t cnt = 0;
	for (; cnt < len; cnt++)
	{
		if (!ringbuff_push_byte(param, ((uint8_t*)buff)[cnt]))
		{
			return cnt;
		}
	}
	return cnt;
}
//###########################################################################
bool ringbuff_pop_byte(ringbuff *param, uint8_t *data)
{
	if(param->elementSize != 1)
		return false;
	bool res;
#ifdef _UTIL_ATOMIC_H_
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
#endif
	unsigned int BytesToBuff = param->elemenmtsOnBuff;
	if (BytesToBuff == 0)
		res = false;
	else
	{
		unsigned int PopPtr = param->popPtr;
		*data = ((uint8_t *)param->buff)[PopPtr];
		PopPtr++;
		if (PopPtr == param->elements)
			PopPtr = 0;
		BytesToBuff--;
		param->elemenmtsOnBuff = BytesToBuff;
		param->popPtr = PopPtr;
		res = true;
	}
#ifdef _UTIL_ATOMIC_H_
}
#endif
	return res;
}
//###########################################################################
bool ringbuff_pop_half(ringbuff *param, uint16_t *data)
{
	if(param->elementSize != 2)
		return false;
	bool res;
#ifdef _UTIL_ATOMIC_H_
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
#endif
	unsigned int BytesToBuff = param->elemenmtsOnBuff;
	if (BytesToBuff == 0)
		res = false;
	else
	{
		unsigned int PopPtr = param->popPtr;
		*data = ((uint16_t *)param->buff)[PopPtr];
		PopPtr++;
		if (PopPtr == param->elements)
			PopPtr = 0;
		BytesToBuff--;
		param->elemenmtsOnBuff = BytesToBuff;
		param->popPtr = PopPtr;
		res = true;
	}
#ifdef _UTIL_ATOMIC_H_
}
#endif
	return res;
}
//###########################################################################
bool ringbuff_pop_word(ringbuff *param, uint32_t *data)
{
	if(param->elementSize != 4)
		return false;
	bool res;
#ifdef _UTIL_ATOMIC_H_
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
#endif
	unsigned int BytesToBuff = param->elemenmtsOnBuff;
	if (BytesToBuff == 0)
		res = false;
	else
	{
		unsigned int PopPtr = param->popPtr;
		*data = ((uint32_t *)param->buff)[PopPtr];
		PopPtr++;
		if (PopPtr == param->elements)
			PopPtr = 0;
		BytesToBuff--;
		param->elemenmtsOnBuff = BytesToBuff;
		param->popPtr = PopPtr;
		res = true;
	}
#ifdef _UTIL_ATOMIC_H_
}
#endif
	return res;
}
//###########################################################################
bool ringbuff_pop_dword(ringbuff *param, uint64_t *data)
{
	if(param->elementSize != 8)
		return false;
	bool res;
#ifdef _UTIL_ATOMIC_H_
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
#endif
	unsigned int BytesToBuff = param->elemenmtsOnBuff;
	if (BytesToBuff == 0)
		res = false;
	else
	{
		unsigned int PopPtr = param->popPtr;
		*data = ((uint64_t *)param->buff)[PopPtr];
		PopPtr++;
		if (PopPtr == param->elements)
			PopPtr = 0;
		BytesToBuff--;
		param->elemenmtsOnBuff = BytesToBuff;
		param->popPtr = PopPtr;
		res = true;
	}
#ifdef _UTIL_ATOMIC_H_
}
#endif
	return res;
}
//###########################################################################
int32_t ringbuff_pop_buff(ringbuff *param, uint8_t *buff, int32_t len)
{
	int32_t cnt = 0;
	for (; cnt < len; cnt++)
	{
		if (!ringbuff_pop_byte(param, &buff[cnt]))
			return cnt;
	}
	return cnt;
}
//###########################################################################
SysErr ringbuff_reinit_v_size(ringbuff *param, uint32_t elements, uint32_t elementSize)
{
	ringbuff_deinit(param);
	if ((param->buff = (uint8_t*) malloc(elements * elementSize)) == 0)
	{
		return SYS_ERR_OUT_OF_MEMORY;
	}
	param->elements = elements;
	param->elementSize = elementSize;
	return SYS_ERR_OK;
}
//###########################################################################
SysErr ringbuff_reinit(ringbuff *param, uint32_t elements)
{
	ringbuff_deinit(param);
	if ((param->buff = (uint8_t*) malloc(elements)) == 0)
	{
		return SYS_ERR_OUT_OF_MEMORY;
	}
	param->elements = elements;
	param->elementSize = 1;
	return SYS_ERR_OK;
}
//###########################################################################
#endif
