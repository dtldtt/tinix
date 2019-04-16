/*
 * =====================================================================================
 *
 *       Filename:  string.c
 *
 *    Description:  字符串操作的相关实现
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
#include "string.h"
#include "types.h"

//内存复制
void memcpy(uint8_t *dest,const uint8_t *src,uint32_t len)
{
    while(len--)
    {
        *dest++ = *src++;
    }
}

//内存初始化
void memset(void *dest, uint8_t val, uint32_t len)
{
    uint8_t *destination = (uint8_t *) dest;
    while(len--)
    {
        *destination++ = val;
    }
}

//指定位置长度置零
void bzero(void *dest, uint32_t len)
{
    memset(dest,0,len);
}

//字符串比较
int strcmp(const char * str1, const char* str2)
{
    while(*str1 && *str2 && *str1==*str2)
    {
        str1++;
        str2++;
    }
    return *str1 - *str2 ;
}

//字符串复制
char *strcpy(char *dest,const char *src)
{
    char* temp=dest;
    while(*src)
    {
        *dest++ = *src++;
    }
    *dest='\0';
    return temp;
}

//字符串连接
char *strcat(char *dest,const char *src)
{
    char *temp=dest;
    while(*dest++);
    while(*src)
    {
        *dest++ = *src++;
    }
    *dest='\0';
    return temp;
}

//求字符串长度
int strlen(const char *src)
{
    int len=0;
    while(*src++)
    {
        len++;
    }
    return len;
}