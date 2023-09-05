#ifndef FATTIME_H_

#if !defined(ESP32)

#include "integer.h"

#ifdef __cplusplus
extern "C"
{
#endif
DWORD get_fattime(void);
#ifdef __cplusplus
}
#endif
	
#endif

#endif
