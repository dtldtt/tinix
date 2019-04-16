/*
 * =====================================================================================
 *
 *       Filename:  string.h
 *
 *    Description:  字符串操作的相关定义
 *
 *        Version:  1.0
 *        Created:  2019.3.22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL(Tongliang Dong)
 *    
 *
 * =====================================================================================
 */
#ifndef INCLUDE_STRING_H
#define INCLUDE_STRING_H

#include "types.h"

//内存复制
void memcpy(uint8_t *dest,const uint8_t *src,uint32_t len);

//内存初始化
void memset(void *dest, uint8_t val, uint32_t len);

//指定位置长度置零
void bzero(void *dest, uint32_t len);

//字符串比较
int strcmp(const char * str1, const char* str2);

//字符串复制
char *strcpy(char *dest,const char *src);

//字符串连接
char *strcat(char *dest,const char *src);

//求字符串长度
int strlen(const char *src);

#endif      // INCLUDE_STRING_H