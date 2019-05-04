/*
 * =====================================================================================
 *
 *       Filename:  test.h
 *
 *    Description:  内核功能测试相关函数声明
 *
 *        Version:  1.0
 *        Created:  2019.5.04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL(Tongliang Dong)
 *    
 *
 * =====================================================================================
 */
#ifndef INCLUDE_TEST_H
#define INCLUDE_TEST_H

void test_interrupt();
void test_memory_info();
void test_page_alloc();
void test_virtual_memory();
void test_kmalloc();
void test_multi_threads();
void test_panic();

#endif