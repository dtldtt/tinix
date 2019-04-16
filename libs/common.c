#include "common.h"

//向端口写入一个字节
inline void outb(uint16_t port,uint8_t value)
{
	asm volatile ("outb %1,%0":: "dN"(port),"a"(value));
}

//从端口读出一个字节
inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	asm volatile ("inb %1,%0": "=a"(ret):"dN"(port));
	return ret;
}

//从端口读出一个字
inline uint16_t inw(uint16_t port)
{
	uint16_t ret;
	asm volatile("inw %1,%0":"=a"(ret):"dN"(port));
	return ret;
}