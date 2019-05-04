/*
 * =====================================================================================
 *
 *       Filename:  types.h
 *
 *    Description:  一些常用类型的定义
 *
 *        Version:  1.0
 *        Created:  2019.3.21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL(Tongliang Dong)
 *    
 *
 * =====================================================================================
 */

#ifndef INCLUDE_TYPES_H
#define INCLUDE_TYPES_H

#ifndef NULL
	#define NULL 0
#endif

#ifndef TRUE
	#define TRUE  1
	#define FALSE 0
#endif

typedef unsigned int   uint32_t;
typedef          int   int32_t;
typedef unsigned short uint16_t;
typedef          short int16_t;
typedef unsigned char  uint8_t;
typedef          char  int8_t;
typedef uint8_t bool;

typedef int32_t pid_t;

#endif 	// INCLUDE_TYPES_H
