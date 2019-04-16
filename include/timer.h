/*
 * =====================================================================================
 *
 *       Filename:  timer.h
 *
 *    Description:  定义 PIT(周期中断定时器) 相关函数
 *
 *        Version:  1.0
 *        Created:  2019.3.24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL (TONGLIANG DONG)
 *
 * =====================================================================================
 */

#ifndef INCLUDE_TIMER_H
#define INCLUDE_TIMER_H

#include "types.h"

void init_timer(uint32_t frequency);

#endif 	// INCLUDE_TIMER_H
