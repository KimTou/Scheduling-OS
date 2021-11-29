#ifndef SCHEDULING_H_INCLUDED
#define SCHEDULING_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY 100		//主存空间总大小
#define DEVICE 4		//磁带机数量

#define FREE 0				//空闲
#define ALLOCATED 1			//已分配

//========================算法选择========================

#define FCFS 1			//先来先服务算法
#define SJF	2			//短作业优先算法
#define RR 3			//进程时间片轮转
#define MFQ 4			//多级反馈队列
#define PSA 5			//优先级调度算法
#define FIRSTFIT 1		//首次适应算法
#define BESTFIT 2		//最佳适应算法

//========================数据结构========================

typedef struct jcb {	//作业控制块
	int jobId;			//作业号
	char state;			//状态
	int super;			//对应进程优先级
	int chour;			//到达时刻（小时）
	int cminute;		//到达时刻（分钟）
	int ntime;			//所需运行时间（分钟）
	int ehour;			//进入内存时刻（小时）
	int eminute;		//进入内存时刻（分钟）
	int fhour;			//完成时刻（小时）
	int fminute;		//完成时刻（分钟）
	int memory;			//所需内存空间大小
	int device;			//所需设备数
	struct jcb* next;
}*JCB, jcb;

typedef struct pcb {  //进程控制块PCB 
	int pid;		  //进程号（与作业号相同）
	char state;		  //状态
	int round;		  //时间片
	int ntime;		  //所需运行时间（分钟）
	int rtime;		  //服务时间（分钟）
	int super;		  //优先级
	struct pcb* next;
}*PCB, pcb;

typedef struct queue {	//多级反馈队列
	PCB pcb;		//就绪队列头结点
	int priority;	//优先级
	int round;		//时间片
	struct queue* next;
}*Queue, queue;

typedef struct freePartition {		//空闲分区
	int number;		//分区号
	int index;		//起始地址
	int	size;		//分区大小
	int state;		//状态
	int job;		//作业号
	struct freePartition* next;
}*FPT, fpt;

typedef struct Linkedfpt {		//空闲分区链
	fpt data;
	struct Linkedfpt* pre;
	struct Linkedfpt* next;
}*LinkedFPT, Linkedfpt;

//========================基本操作========================

void init();		//初始化
void initMFQ();		//初始化多级反馈队列
void input();		//输入作业
void clock();		//记录当前时间
void display(PCB pr);	//打印进程情况
void display(JCB job);	//打印完成作业情况
void display();		//打印空闲分区链
int alloc(int job, int needSize);	//分配内存
void freePart(int job);				//内存回收
void sortPart();					//对空闲分区链进行排序		
void come();		//将新达到的未到达作业加入等待队列
void jobFCFSSort();	//作业先来先服务排序（对等待队列进行排序）
void jobSJFSort();	//短作业优先调度排序（对等待队列进行排序）
void jobScheduling();		//作业调度（将等待队列的作业调入内存，调入就绪队列）
void destroy();		//销毁进程，加入完成队列
void processFCFS();		//进程先来先服务调度（从就绪队列中取出执行）
void processPSA();		//进程优先级调度
void processSJF();		//短进程优先调度（从就绪队列中取出执行）
void processRound();	//时间片轮转
void check();			//建立进程查看函数
void find();			//查找下一个优先级最高的进程
void analyse();			//判断是否有更高优先级的进程到来
void sort();			//建立对进程进行优先级排列函数
void processMFQ();		//多级反馈队列进程调度


#endif