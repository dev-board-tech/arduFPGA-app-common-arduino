/*
 *  include/global.h
 */

#ifndef __LIB_API_GLOBAL_DEF_HPP__
#define __LIB_API_GLOBAL_DEF_HPP__

/*
 * Sys errors definitions.
 */
typedef enum {
	SYS_ERR_OK 						=   0,
	SYS_ERR_UNKNOWN					=  -1,
	SYS_ERR_INVALID_HANDLER			=  -2,
	SYS_ERR_OUT_OF_MEMORY			=  -3,
	SYS_ERR_OUT_OF_RANGE			=  -4,
	SYS_ERR_INVALID_COMMAND			=  -5,
	SYS_ERR_INVALID_PARAM			=  -6,
	SYS_ERR_INIT					=  -7,
	SYS_ERR_NOT_IMPLEMENTED			=  -8,
	SYS_ERR_NOT_INITIALIZED			=  -9,
	SYS_ERR_NO_PATH					= -10,
	SYS_ERR_INVALID_PATH			= -11,
	SYS_ERR_NOT_ACTIVE				= -12,
	SYS_ERR_NAK						= -13,
	SYS_ERR_BUSY					= -14,
	SYS_ERR_WRITE					= -15,
	SYS_ERR_READ					= -16,
	SYS_ERR_NO_REGISTERED_DEVICE	= -17,
	SYS_ERR_BUFF_NOT_ALLOCATED		= -18,
	SYS_ERR_BUS						= -19,
	SYS_ERR_NO_ASSIGNED_DEVICE		= -20,
	SYS_ERR_DEVICE_NOT_MATCH		= -21,
	SYS_ERR_DEVICE_NOT_FOUND		= -22,
	SYS_ERR_TIMEOUT					= -23,
	SYS_ERR_NOTHING_CHANGED			= -24,
    SYS_ERR_NOT_RECEIVED            = -25,
    SYS_ERR_TRANSMISSION            = -26,
    SYS_ERR_RECEPTION               = -27,
    SYS_ERR_TOO_LONG                = -28,
    SYS_ERR_OUT_OF_PAGE				= -29,
    SYS_ERR_INVALID_SIZE			= -30,
	SYS_ERR_INVALID_DEVICE			= -31,
	SYS_ERR_DATA_NOT_READY			= -32,
}SysErr;

typedef enum {
    size1 = 0,
    size2,
    size4,
    size8,
    size16,
    size32,
    size64,
    size128,
    size256,
    size512,
    size1024,
    size2048,
    size4096,
    size8192,
    size16384,
    size32768,
    size65536,
    size131072,
    size262144,
    size524288,
    size1048576,
    size2097152,
    size4194304,
    size8388608,
    size16777216,
}Sizes;

/*
 * Int definitions.
 */
/*#ifndef u8
typedef unsigned char u8;
#endif
#ifndef s8
typedef signed char s8;
#endif
#ifndef u16
typedef unsigned short u16;
#endif
#ifndef s16
typedef signed short s16;
#endif
#ifndef u32
typedef unsigned long u32;
#endif
#ifndef s32
typedef signed long s32;
#endif
#ifndef ui
typedef unsigned int ui;
#endif
#ifndef si
typedef signed int si;
#endif
#ifndef u64
typedef unsigned long long u64;
#endif
#ifndef s64
typedef signed long long s64;
#endif
#ifndef ff
typedef float ff;
#endif
#ifndef dd
typedef double dd;
#endif*/

typedef struct box_s{
    int x_min;
    int x_max;
    int y_min;
    int y_max;
}box_t;

typedef enum {
    horizontal = 0,
    vertical
} orientation;

typedef enum {
	LANDSCAPE,
	PORTRAIT,
	LANDSCAPE_FLIPPED,
	PORTRAIT_FLIPPED,
} screenOrientation;

//typedef char* string;

/*
 * Graphic definitions.
 */

/*#####################################################*/


#endif /* LIB_API_GLOBAL_DEF_HPP_ */
