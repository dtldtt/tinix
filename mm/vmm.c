/*
 * =====================================================================================
 *
 *       Filename:  vmm.c
 *
 *    Description:  虚拟内存管理的实现virtual memory management
 *
 *        Version:  1.0
 *        Created:  2019.4.02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL(Tongliang Dong)
 *    
 *
 * =====================================================================================
 */
#include "idt.h"
#include "string.h"
#include "debug.h"
#include "vmm.h"
#include "pmm.h"
//#include "bitmap.h"

// 内核页目录区域，1024项的一个数组
pgd_t *pgd_kern  = (uint32_t *)PGD_ADDR;
//static bitmap page_bitmap={(PTE_COUNT-1)*PTE_SIZE,}
//static uint8_t page_bitmap[(PTE_COUNT-1)*PTE_SIZE/8]={0};
static uint8_t page_status[(PTE_COUNT-1)*PTE_SIZE];
// 内核页表区域
static pte_t *pte_kern  = PTE_ADDR;
/**
 * 在加载内核之前，我们已经对页表做了必要的初始化
 * 比如，页目录的第0项和第768项都指向第0个页表，其中映射了低端4M地址
 * 页目录的最后一项指向了自己
 * 页目录的769-1022项分别指向了从第1个页表开始往后的254个页表，
 * 但是对应的页表还没有建立，也就是只做了页目录到页表的对应，相应页表对应的物理地址还没有做
 * 因为在初始化阶段，内核没有访问到4mb以后的空间
 * 也就是说，现在访问虚存4M以内的空间是可以直接访问的，有正确的映射关系。
 * 而4MB之后的只能对应到第1到第253的页表，然而页表里边没有内容，所以不能映射成功。
 * 初始化函数负责吧这些页表内容清空，当访问到时候再进行分配和具体的映射
 */
void init_vmm()
{
	//pte_kern[0]=(uint32_t *)PTE_ADDR;
	// 0xc0000000 这个地址在页目录的索引
	//uint32_t kern_pte_first_idx = PGD_INDEX(PAGE_OFFSET);
	//memset(pte_kern+0x1000,0,0x1000*254);

	//uint32_t i,j;
	// for(i=kern_pte_first_idx,j=0;i<PTE_COUNT+kern_pte_first_idx;i++,j++) {
	//     // 此处是内核虚拟地址，MMU需要物理地址，所以减去偏移
	//     pgd_kern[i] = ((uint32_t)pte_kern[j]-PAGE_OFFSET) |
	//         PAGE_PRESENT | PAGE_WRITE;
	// }
	// pte指向pte_kern + 0x1000,也就是指向了第1个页表的地址
	//uint32_t *pte = pte_kern + 0x1000;
	// 第0页在加载内核前已经映射过了，指向低端4m
	// 从0x400000开始，前面的都已经映射过了
	// uint32_t i;
	// uint32_t initial_addr = 0x400000;
	int i;
	for(i=0;i<(PTE_COUNT-1)*PTE_SIZE;i++)
	{
		// pte[i]= initial_addr | PAGE_PRESENT | PAGE_WRITE;
		// initial_addr+=PMM_PAGE_SIZE;
		page_status[i]=0;
	}
	// 页目录物理地址，用于更新cr3值
	//uint32_t pgd_kern_phy_addr = (uint32_t)pgd_kern - PAGE_OFFSET;

	// 注册也错误中断的处理函数（14 是页故障的中断号）
	register_interrupt_handler(14,&page_fault);

	//switch_pgd(pgd_kern_phy_addr);
}

// 几乎用不到，我们的内核过程不会更改页目录地址的
void switch_pgd(uint32_t pd)
{
	asm volatile ("mov %0,%%cr3" : : "r" (pd));
}

void map(pgd_t *pgd_now,uint32_t va,uint32_t pa,uint32_t flags)
{
	uint32_t pgd_idx = PGD_INDEX(va);
	uint32_t pte_idx = PTE_INDEX(va);
	//printk("pgd_idx:%d    pte_idx:%d\n",pgd_idx,pte_idx);
	// 目前页目录项只有第0项和第768到页目录结束有内容，在loader中初始化了
	// 其他页目录项是空的，这部分空间也是用户进程才会访问到的，在0xc0000000以下
	// 所以如果访问到了，但是发现页目录是空的，就要分配具体的一页来当做该页目录对应的页表了
	pte_t *pte= (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
	uint8_t found=0;
	if (!pte) {
		//printk("pte is 0\n");
		 int i=0;
		for(i=0;i<(PTE_COUNT-1)*PTE_SIZE;i++){
			//int index = i/8;
			if(page_status[i] == 0 ){
				pte = pte_kern[1024 + i*4] & PAGE_MASK;
				pgd_now[pgd_idx]= (uint32_t)pte | PAGE_PRESENT | PAGE_WRITE;
				found=1;
				break;
			} 
		}
		// 有没有找到空闲页
		assert(found!=0,"Memory is run out!\n");
		//printk("new pte is 0x%X, page table this item is 0x%X\n",pte,pgd_now[pgd_idx]);
		
	} 
	else {
		// 转换到内核虚拟地址，这样才能访问到页表所在的地址，因为给页表的地址是物理地址，我们必须给出虚拟地址才能访问到
		//pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
	}
	
	//printk("new pte is 0x%X, page table this item is 0x%X\n",pte,pgd_now[pgd_idx]);
	// 转换到内核虚拟地址
	pte=(pte_t *)((uint32_t)pte +PAGE_OFFSET);
	pte[pte_idx] = (pa & PAGE_MASK) | flags;
	//printk("pte[pte_idx] is : 0x%X\n",pte[pte_idx]);

	// 通知cpu更新页表缓存；
	asm volatile("invlpg (%0)" : : "a" (va));
}

void unmap(pgd_t *pgd_now,uint32_t va)
{
	uint32_t pgd_idx = PGD_INDEX(va);
	uint32_t pte_idx = PTE_INDEX(va);
	pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);

	if (!pte) {
		return ;
	}

	// //转换到内核线性地址
	pte = (pte_t*)((uint32_t)pte + PAGE_OFFSET);

	pte[pte_idx]=0;

	//通知CPU更新页表缓存
	asm volatile ("invlpg (%0)" : : "a" (va));
}

uint32_t get_mapping(pgd_t *pgd_now, uint32_t va ,uint32_t *pa)
{
	uint32_t pgd_idx = PGD_INDEX(va);
	uint32_t pte_idx = PTE_INDEX(va);


	pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
	if (!pte) {
		//printk("pte is 0,so return 0\n");
		return 0;
	}
	// 转换到内核线性地址
	pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);

	// 如果地址有效而且指针不是NULL，则返回地址
	//printk("pa is 0x%X",pa);
	if (pte[pte_idx] !=0 && pa) {
		//printk("pte[pte_idx]:0x%X\n",pte[pte_idx]);
		*pa = pte[pte_idx] & PAGE_MASK;
		return *pa;
	}

	return 0;
}