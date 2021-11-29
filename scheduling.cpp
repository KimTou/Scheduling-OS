#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduling.h"

//========================全局变量========================

PCB executeProcess;			//执行进程
PCB readyProcess = NULL;	//就绪进程队列
JCB doneJob = NULL;			//完成作业队列
JCB readyJob = NULL;		//就绪作业队列（被调入内存）
JCB waitJob = NULL;			//等待作业队列（等待被调度）
JCB nonarrivalJob = NULL;	//未到达作业
JCB selectJob;				//选中作业
JCB executeJob;				//执行作业
LinkedFPT partHead;			//分区头结点
Queue queueHead;			//第一队列

int Memory;				//内存空间大小
int device;				//可使用的磁带机数量
int partNum = 1;		//分区号计数
int robin;				//时间片
int round = 0;			//时间片时钟
int hour = 9;			//系统小时
int minute = 55;		//系统分钟

//=================选择算法=====================

int processAl;			//进程调度算法
int jobAl;				//作业调度算法
int partAl;				//空闲分区算法

//==============================================

/*
* 初始化
*/
void init() {

	//选择算法
	printf("\n请输入选择的作业调度算法（1.先来先服务  2.短作业优先）：");
	scanf("%d", &jobAl);
	printf("\n请输入选择的进程调度算法（1.先来先服务  2.短进程优先  3.时间片轮转  4.多级反馈队列  5.优先级调度）：");
	scanf("%d", &processAl);
	if (processAl == RR) {
		printf("\n请输入时间片大小:");
		scanf("%d", &robin);
	}
	if (processAl == MFQ) {
		initMFQ();
	}
	printf("\n请输入选择的内存分配算法（1.首次适应算法  2.最佳适应算法）：");
	scanf("%d", &partAl);
	printf("\n请输入内存空间大小：");
	scanf("%d", &Memory);
	printf("\n请输入设备数量：");
	scanf("%d", &device);

	//初始化作业进程队列的头结点
	nonarrivalJob = (JCB)malloc(sizeof(jcb));
	nonarrivalJob->next = NULL;
	waitJob = (JCB)malloc(sizeof(jcb));
	waitJob->next = NULL;
	readyJob = (JCB)malloc(sizeof(jcb));
	readyJob->next = NULL;
	doneJob = (JCB)malloc(sizeof(jcb));
	doneJob->next = NULL;
	readyProcess = (PCB)malloc(sizeof(pcb));
	readyProcess->next = NULL;

	//初始化空闲分区
	partHead = (LinkedFPT)malloc(sizeof(Linkedfpt));
	if (partHead == NULL) {
		return;
	}
	partHead->pre = NULL;
	partHead->next = NULL;
	partHead->data.number = partNum++;
	partHead->data.index = 0;
	partHead->data.size = Memory;
	partHead->data.state = FREE;

}

/*
* 初始化多级反馈队列
*/
void initMFQ() {
	queueHead = NULL;
	int i, num, round;
	printf("请输入设置的就绪队列个数:");
	scanf("%d", &num);
	printf("请输入第一个队列的时间片:");
	scanf("%d", &round);
	for (i = 0; i < num; i++) {
		if (queueHead == NULL) {
			queueHead = (Queue)malloc(sizeof(queue));
			queueHead->pcb = NULL;
			queueHead->priority = i + 1;
			queueHead->round = round;
			queueHead->next = NULL;
		}
		else {
			Queue node = queueHead;
			while (node->next != NULL) {
				node = node->next;
			}
			Queue p = (Queue)malloc(sizeof(queue));
			p->pcb = NULL;
			p->priority = i + 1;
			p->round = round;
			p->next = NULL;
			node->next = p;
		}
		round *= 2;
	}
}

/*
* 输入作业
*/
void input() {
	int i, num;
	printf("\n请输入作业数:");
	scanf("%d", &num);
	for (i = 0; i < num; i++)
	{
		printf("\n序号No.%d:\n", i + 1);
		JCB p = (JCB)malloc(sizeof(jcb));
		if (p == NULL) {
			printf("系统错误\n");
			return;
		}
		printf("\n输入作业号:");
		scanf("%d", &p->jobId);
		if (processAl == PSA) {
			printf("\n输入作业优先数:");
			scanf("%d", &p->super);
		}
		printf("\n输入作业到达时刻（小时）:");
		scanf("%d", &p->chour);
		printf("\n输入作业到达时刻（分钟）:");
		scanf("%d", &p->cminute);
		printf("\n输入作业运行时间:");
		scanf("%d", &p->ntime);
		printf("\n输入作业所需内存空间大小（KB）:");
		scanf("%d", &p->memory);
		printf("\n输入作业所需设备数:");
		scanf("%d", &p->device);
		printf("\n");
		p->state = 'W';
		p->next = NULL;
		//加入等待队列（已到达的作业）
		if (p->chour * 60 + p->cminute <= hour * 60 + minute) {
			if (jobAl == FCFS) {
				//先来先服务
				jobFCFSSort();
			}
			else {
				//短作业优先
				jobSJFSort();
			}
		}
		else {
			//加入未到达队列（未到达的作业）
			JCB node = nonarrivalJob;
			while (node->next != NULL) {
				node = node->next;
			}
			node->next = p;
		}
	}
}

/*
* 计时
*/
void clock() {
	minute++;
	if (minute >= 60) {
		hour++;
		minute = 0;
	}
}

/*
* 打印进程情况
*/
void display(PCB pr)
{
	printf(" 进程号  状态  所需运行时间  服务时间 \n");
	printf("  %d\t", pr->pid);
	printf("  %c\t", pr->state);
	printf("  %d\t", pr->ntime);
	printf("\t%d\t", pr->rtime);
	printf("\n");
}

/*
* 打印完成作业情况
*/
void display(JCB job)
{
	printf("\n");
	printf(" 作业号  状态  所需内存空间大小  到达时间  所需运行时间  进入内存时间  完成时间  周转时间\n");
	printf("   %d\t", job->jobId);
	printf("  %c\t", job->state);
	printf("   %dKB\t\t", job->memory);
	printf("  %d:%02d\t", job->chour, job->cminute);
	printf("    %d分钟\t", job->ntime);
	printf("  %d:%02d\t", job->ehour, job->eminute);
	printf("\t%d:%02d\t", job->fhour, job->fminute);
	printf("  %d\t", job->fhour * 60 + job->fminute - job->chour * 60 - job->cminute);
	printf("\n");
}

/*
* 显示
*/
void display() {
	//==========显示被选中作业===========
	if (executeJob != NULL) {
		printf("\n目前正在执行的作业如下：\n");
		printf(" 作业号  状态  所需内存空间大小  所需设备数  到达时间  所需运行时间  进入内存时间  服务时间\n");
		printf("   %d\t", executeJob->jobId);
		printf("  %c\t", executeJob->state);
		printf("   %dKB\t\t", executeJob->memory);
		printf("    %d\t", executeJob->device);
		printf("      %d:%02d\t", executeJob->chour, executeJob->cminute);
		printf("  %d分钟\t", executeJob->ntime);
		printf("%d:%02d\t", executeJob->ehour, executeJob->eminute);
		printf("    %d分钟\t", executeProcess->rtime);
		printf("\n");
	}
	else {
		printf("当前无正在执行的作业\n");
	}

	//==========显示内存空闲区===========
	LinkedFPT p = partHead;
	printf("\n===============空闲分区链====================\n\n");
	printf("分区号 起始地址 分区大小 状态\n");
	while (p != NULL) {
		if (p->data.state == FREE) {
			printf("|%d\t", p->data.number);
			printf("|%d\t", p->data.index);
			printf("|%dKB\t", p->data.size);
			printf("|空闲\n");
		}
		else {
			printf("|%d\t", p->data.number);
			printf("|%d\t", p->data.index);
			printf("|%dKB\t", p->data.size);
			printf("|已分配\t");
			printf("|分配作业号:%d\n", p->data.job);
		}
		p = p->next;
	}

	//=========当前设备情况=========
	printf("\n=========当前设备使用情况==========\n");
	printf("设备总数：%d\t剩余空闲设备数：%d\n", DEVICE, device);
}

/*
* 内存分配
*/
int alloc(int job, int needSize) {
	LinkedFPT p = partHead;
	while (p != NULL) {
		if (p->data.state == FREE) {
			//若大小相等，则直接分配
			if (p->data.size == needSize) {
				p->data.state = ALLOCATED;
				p->data.job = job;
				break;
			}
			//若空闲区大于请求分配大小，则划分出一块内存空间出去
			if (p->data.size > needSize) {
				LinkedFPT node = (LinkedFPT)malloc(sizeof(Linkedfpt));
				node->data.number = partNum++;
				node->data.index = p->data.index;
				node->data.size = needSize;
				node->data.state = ALLOCATED;
				node->data.job = job;
				node->pre = p->pre;
				node->next = p;
				if (p == partHead) {
					partHead = node;
				}
				else {
					p->pre->next = node;
				}
				p->pre = node;
				p->data.index += needSize;
				p->data.size -= needSize;
				break;
			}
		}
		p = p->next;
	}
	if (p == NULL) {
		return 0;
	}
	else {
		return 1;
	}
}

/*
* 内存回收
*/
void freePart(int job) {
	LinkedFPT p = partHead;
	while (p != NULL) {
		if (p->data.state == ALLOCATED && p->data.job == job) {
			LinkedFPT front = NULL;
			LinkedFPT rear = NULL;
			LinkedFPT mid = partHead;
			while (mid != NULL) {
				if (mid->data.index + mid->data.size == p->data.index) {
					if (mid->data.state == FREE) {
						front = mid;
					}
				}
				if (mid->data.index == p->data.index + p->data.size) {
					if (mid->data.state == FREE) {
						rear = mid;
					}
				}
				mid = mid->next;
			}
			//情况一:回收区同时与插入点前、后两个空闲分区相邻接
			if (front != NULL && rear != NULL) {
				front->data.size += (p->data.size + rear->data.size);
				if (p->next != NULL) {
					p->next->pre = p->pre;
				}
				if (p->pre != NULL) {
					p->pre->next = p->next;
				}
				if (rear->next != NULL) {
					rear->next->pre = rear->pre;
				}
				if (rear->pre != NULL) {
					rear->pre->next = rear->next;
				}
				break;
			}
			//情况二:回收区与插入点前一个空闲分区相邻接
			else if (front != NULL) {
				front->data.size += p->data.size;
				if (p->next != NULL) {
					p->next->pre = p->pre;
				}
				if (p->pre != NULL) {
					p->pre->next = p->next;
				}
				break;
			}
			//情况三:回收区与插入点后一个空闲分区相邻接
			else if (rear != NULL) {
				p->data.size += rear->data.size;
				if (rear->next != NULL) {
					rear->next->pre = rear->pre;
				}
				if (rear->pre != NULL) {
					rear->pre->next = rear->next;
				}
				p->data.state = FREE;
				p->data.job = 0;
				break;
			}
			//情况四:回收区不与任何空闲分区相邻
			else {
				p->data.state = FREE;
			}
		}
		else {
			p = p->next;
		}
	}
	display(executeJob);
}

/*
* 对空闲分区链进行排序
*/
void sortPart() {
	int temp;
	LinkedFPT slow = partHead;
	LinkedFPT fast = NULL;
	LinkedFPT mid = NULL;	//mid为通过选择排序得到slow指针后面空闲分区最小的结点
	LinkedFPT newHead = NULL;
	while (slow != NULL) {
		if (slow->data.state == FREE) {
			break;
		}
		slow = slow->next;
	}
	while (slow != NULL) {
		fast = slow->next;
		mid = NULL;
		while (fast != NULL) {
			if (fast->data.state == FREE) {
				if (mid == NULL) {
					mid = fast;
				}
				else if (fast->data.size < mid->data.size) {
					mid = fast;
				}
			}
			fast = fast->next;
		}
		//若mid为NULL或mid空闲区更大，则空闲分区链已有序
		if (mid == NULL || mid->data.size >= slow->data.size) {
			break;
		}
		else {
			//调换slow与mid结点
			temp = slow->data.number;
			slow->data.number = mid->data.number;
			mid->data.number = temp;
			temp = slow->data.index;
			slow->data.index = mid->data.index;
			mid->data.index = temp;
			temp = slow->data.size;
			slow->data.size = mid->data.size;
			mid->data.size = temp;
			//后移慢指针
			slow = mid;
		}
	}
}

/*
* 将新达到的未到达作业加入等待队列
*/
void come() {
	JCB node = nonarrivalJob;
	while (node->next != NULL) {
		if (node->next->chour * 60 + node->next->cminute == hour * 60 + minute) {
			selectJob = node->next;
			node->next = node->next->next;
			selectJob->next = NULL;
			if (jobAl == FCFS) {
				jobFCFSSort();
			}
			else {
				jobSJFSort();
			}
			selectJob = NULL;
		}
		else {
			node = node->next;
		}
	}
}

/*
* 先来先服务排序（对等待队列进行排序）
*/
void jobFCFSSort() {
	//将到达时间最早的排在最前面
	JCB p = waitJob;
	while (p->next != NULL) {
		p = p->next;
	}
	p->next = selectJob;
}

/*
* 短作业优先调度排序（对等待队列进行排序）
*/
void jobSJFSort() {
	//将到达时间最早的排在最前面
	JCB p = waitJob;
	while (p->next != NULL) {
		if (selectJob->ntime < p->next->ntime) {
			selectJob->next = p->next;
			p->next = selectJob;
			return;
		}
		p = p->next;
	}
	p->next = selectJob;
}

/*
* 作业调度（将等待队列的作业调入内存，调入就绪队列）
*/
void jobScheduling() {
	JCB wait = waitJob;
	while (wait->next != NULL) {
		if (wait->next->device <= device) {
			JCB job = wait->next;
			if (partAl == 2) {
				//最佳适应算法，对空闲分区排序
				sortPart();
			}
			//若内存与设备数足够，则调入内存
			if (alloc(job->jobId, job->memory) == 1) {
				wait->next = job->next;
				device -= job->device;
				job->ehour = hour;
				job->eminute = minute;
				job->state = 'R';
				job->next = readyJob->next;
				readyJob->next = job;
				//为作业创建PCB进程
				PCB p = (PCB)malloc(sizeof(pcb));
				p->pid = job->jobId;
				p->state = 'W';
				p->ntime = job->ntime;
				p->rtime = 0;
				if (processAl == PSA) {
					p->super = job->super;
				}
				p->next = NULL;
				PCB node = readyProcess;
				//将新进程加入就绪进程队列
				if (processAl == FCFS || processAl == RR) {
					//先来先服务/时间片轮转 进程调度排序
					while (node->next != NULL) {
						node = node->next;
					}
					node->next = p;
				}
				else if (processAl == PSA) {
					//进程优先数调度
					while (node->next != NULL) {
						if (p->super < node->next->super) {
							p->next = node->next;
							node->next = p;
							break;
						}
						node = node->next;
					}
					if (node->next == NULL) {
						node->next = p;
					}
				}
				else if(processAl == SJF){
					//短进程优先调度排序
					while (node->next != NULL) {
						//所需运行时间短的排在前面（所需运行时间-服务时间）
						if (p->ntime < (node->next->ntime - node->next->rtime)) {
							p->next = node->next;
							node->next = p;
							break;
						}
						node = node->next;
					}
					if (node->next == NULL) {
						node->next = p;
					}
				}
				else {
					//加入多级反馈队列第一队列队尾
					PCB first = queueHead->pcb;
					p->round = queueHead->round;		//赋值时间片
					if (first == NULL) {
						first = p;
						queueHead->pcb = first;
					}
					else {
						while (first->next != NULL) {
							first = first->next;
						}
						first->next = p;
					}
				}
			}
			else {
				//内存不足
				wait = wait->next;
			}
		}
		else {
			//设备不足
			wait = wait->next;
		}
	}
}

/*
* 销毁进程，加入完成队列
*/
void destroy() {
	JCB node = readyJob;
	while (node->next != NULL) {
		if (node->next->jobId == executeProcess->pid) {
			selectJob = node->next;
			node->next = selectJob->next;
			selectJob->fhour = hour;
			selectJob->fminute = minute;
			device += selectJob->device;	//回收设备
			//加入完成作业队列
			selectJob->state = 'D';
			JCB find = doneJob;
			while (find->next != NULL) {
				if (selectJob->jobId < find->next->jobId) {
					selectJob->next = find->next;
					find->next = selectJob;
					break;
				}
				find = find->next;
			}
			if (find->next == NULL) {
				selectJob->next = NULL;
				find->next = selectJob;
			}
			selectJob = NULL;
			break;
		}
		node = node->next;
	}

	//若为反馈队列算法，需从就绪队列中移除
	if (processAl == MFQ) {
		Queue node = queueHead;
		//找到对应的就绪队列
		while (node != NULL) {
			if (node->round == executeProcess->round) {
				node->pcb = executeProcess->next;
				break;
			}
			node = node->next;
		}
	}

	printf("\n 作业 [%d] 已完成.\n", executeProcess->pid);
	freePart(executeProcess->pid);
	free(executeProcess);
	executeProcess = NULL;
	executeJob = NULL;
}

/*
* 短进程优先调度
*/
void processSJF() {
	if (executeProcess != NULL) {
		executeProcess->rtime++;
		if (executeProcess->ntime == executeProcess->rtime) {
			destroy();
			jobScheduling();	//马上调度内存
		}
	}
	//判断是否能抢占
	if (executeProcess != NULL && processAl == 2) {
		PCB node = readyProcess->next;
		if (node != NULL && node->ntime < (executeProcess->ntime - executeProcess->rtime)) {
			//抢占
			executeProcess->state = 'W';
			executeProcess->next = node->next;
			readyProcess->next = executeProcess;
			node->next = NULL;
			executeProcess = node;
			executeProcess->state = 'R';
		}
	}
	if (executeProcess == NULL) {
		if (readyProcess->next != NULL) {
			executeProcess = readyProcess->next;
			readyProcess->next = executeProcess->next;
			executeProcess->next = NULL;
			executeProcess->state = 'R';
		}
	}
	//展示执行作业
	JCB node = readyJob->next;
	while (node != NULL && executeProcess != NULL) {
		if (node->jobId == executeProcess->pid) {
			executeJob = node;
		}
		node = node->next;
	}
}

/*
* 进程优先级调度
*/
void processPSA() {
	if (executeProcess != NULL) {
		executeProcess->rtime++;
		if (executeProcess->ntime == executeProcess->rtime) {
			destroy();
			jobScheduling();	//马上调度内存
		}
	}
	//判断是否能抢占
	if (executeProcess != NULL && processAl == PSA) {
		PCB node = readyProcess->next;
		if (node != NULL && node->super < executeProcess->super) {
			//抢占
			executeProcess->state = 'W';
			executeProcess->next = node->next;
			readyProcess->next = executeProcess;
			node->next = NULL;
			executeProcess = node;
			executeProcess->state = 'R';
		}
	}
	if (executeProcess == NULL) {
		if (readyProcess->next != NULL) {
			executeProcess = readyProcess->next;
			readyProcess->next = executeProcess->next;
			executeProcess->next = NULL;
			executeProcess->state = 'R';
		}
	}
	//展示执行作业
	JCB node = readyJob->next;
	while (node != NULL && executeProcess != NULL) {
		if (node->jobId == executeProcess->pid) {
			executeJob = node;
		}
		node = node->next;
	}
}

/*
* 进程先来先服务调度
*/
void processFCFS() {
	if (executeProcess != NULL) {
		executeProcess->rtime++;
		if (executeProcess->ntime == executeProcess->rtime) {
			destroy();
			jobScheduling();	//马上调度内存
		}
	}
	if(executeProcess == NULL) {
		if (readyProcess->next != NULL) {
			executeProcess = readyProcess->next;
			readyProcess->next = executeProcess->next;
			executeProcess->next = NULL;
			executeProcess->state = 'R';
		}
	}
	//展示执行作业
	JCB node = readyJob->next;
	while (node != NULL && executeProcess != NULL) {
		if (node->jobId == executeProcess->pid) {
			executeJob = node;
		}
		node = node->next;
	}
}

/*
* 时间片轮转
*/
void processRound() {
	if (executeProcess != NULL) {
		round++;
		executeProcess->rtime++;
		if (executeProcess->ntime == executeProcess->rtime) {
			destroy();
			round = 0;
			jobScheduling();	//马上调度内存
		}
		else {
			//时间片已到
			if (round == robin) {
				printf("\n进程%d 时间片已到\n", executeProcess->pid);
				PCB node = readyProcess;
				//移至就绪进程队列队尾
				executeProcess->state = 'W';
				while (node->next != NULL) {
					node = node->next;
				}
				executeProcess->next = node->next;
				node->next = executeProcess;
				executeProcess = NULL;
				round = 0;
			}
			else {
				printf("\n进程%d 已运行时间片：%d\n", executeProcess->pid, round);
			}
		}
	}
	if (executeProcess == NULL) {
		if (readyProcess->next != NULL) {
			executeProcess = readyProcess->next;
			readyProcess->next = executeProcess->next;
			executeProcess->next = NULL;
			executeProcess->state = 'R';
		}
	}
	//展示执行作业
	JCB node = readyJob->next;
	while (node != NULL && executeProcess != NULL) {
		if (node->jobId == executeProcess->pid) {
			executeJob = node;
		}
		node = node->next;
	}
}


/*
* 建立进程查看函数
*/
void check()
{
	if (executeProcess == NULL) {
		return;
	}
	int count = 1;
	PCB pr;
	Queue node = queueHead;
	printf("\n ============当前正在运行的进程是:\n"); /*显示当前运行进程*/
	display(executeProcess);
	printf("\n ============当前多级反馈队列状态为:\n"); /*显示就绪队列状态*/
	while (node != NULL) {
		printf("\n#就绪队列%d（时间片为%d）:\n\n", count++, node->round);
		pr = node->pcb;
		while (pr != NULL)
		{
			display(pr);
			pr = pr->next;
		}
		node = node->next;
	}
}

/*
* 查找下一个优先级最高的进程
*/
void find()
{
	Queue node = queueHead;
	while (node != NULL) {
		executeProcess = node->pcb;
		if (executeProcess != NULL) {
			executeProcess->state = 'R';
			break;
		}
		else {
			node = node->next;
		}
	}
}

/*
* 判断是否有更高优先级的进程到来
*/
void analyse() {
	Queue node = queueHead;
	PCB find = NULL;
	while (node != NULL) {
		find = node->pcb;
		if (find != NULL) {
			break;
		}
		else {
			node = node->next;
		}
	}
	if (find != NULL) {
		//更高优先级，则进行抢占
		if (find->round < executeProcess->round) {
			//找到对应的就绪队列
			while (node != NULL) {
				if (node->round == executeProcess->round) {
					break;
				}
				node = node->next;
			}
			//原本位于队首，现将其调至队尾
			if (node != NULL && node->pcb->next != NULL) {
				node->pcb = node->pcb->next;
				PCB target = node->pcb;
				while (target->next != NULL) {
					target = target->next;
				}
				executeProcess->state = 'W';
				executeProcess->next = NULL;
				target->next = executeProcess;
			}
			//执行进程next指针不置为NULL，依旧处在反馈队列中
			find->state = 'R';
			executeProcess = find;
			round = 0;
		}
	}
}

/*
* 建立对进程进行优先级排列函数
*/
void sort()
{
	Queue node = queueHead;
	//找到对应的就绪队列
	while (node != NULL) {
		if (node->round == executeProcess->round) {
			break;
		}
		node = node->next;
	}
	//若已是最低优先级队列，则直接放至队尾
	if (node->next == NULL) {
		PCB p = executeProcess->next;
		if (p == NULL) {
			return;
		}
		node->pcb = p;
		executeProcess->next = NULL;
		while (p->next != NULL) {
			p = p->next;
		}
		p->next = executeProcess;
	}
	else {
		node->pcb = executeProcess->next;
		//放至下一优先级队列的队尾
		node = node->next;
		executeProcess->round = node->round;
		executeProcess->next = NULL;
		if (node->pcb == NULL) {
			node->pcb = executeProcess;
		}
		else {
			PCB p = node->pcb;
			while (p->next != NULL) {
				p = p->next;
			}
			p->next = executeProcess;
		}
	}
}

/*
* 多级反馈队列进程调度
*/
void processMFQ() {
	if (executeProcess != NULL) {
		round++;
		executeProcess->rtime++;
		if (executeProcess->ntime == executeProcess->rtime) {
			destroy();
			round = 0;
			jobScheduling();	//马上调度内存
		}
		else {
			if (executeProcess->round <= round) {
				//若时间片走完，进行优先级调整
				sort();
				executeProcess->state = 'W';
				executeProcess = NULL;
				round = 0;
			}
			else {
				printf("\n进程%d 已运行时间片：%d\n", executeProcess->pid, round);
				//判断是否有更高优先级的进程
				analyse();
			}
		}
	}
	if (executeProcess == NULL) {
		find();
	}
	//展示执行作业
	JCB node = readyJob->next;
	while (node != NULL && executeProcess != NULL) {
		if (node->jobId == executeProcess->pid) {
			executeJob = node;
		}
		node = node->next;
	}
}

int main() {
	char ch;
	int choice;
	int h = 0;
	init();
	input();
	while (readyJob->next != NULL || waitJob->next != NULL || nonarrivalJob->next != NULL) {
		printf("\n\n当前时间为 %d:%02d\n", hour, minute);

		jobScheduling();

		if (processAl == 1) {
			processFCFS();
		}
		else if (processAl == 2) {
			processSJF();
		}
		else if (processAl == 3) {
			processRound();
		}
		else if(processAl == 4){	
			processMFQ();
			check();
		}
		else {
			processPSA();
		}

		display();
		//时钟+1后，运行时间才+1
		clock();
		come();
		printf("\n--------------------------------------------\n");
		//ch = getchar();
	}
	printf("\n=============作业已全部完成=============\n\n");
	float zz = 0;
	float result = 0;
	int number = 0;
	JCB done = doneJob->next;
	while (done != NULL) {
		printf("\n");
		printf(" 作业号  状态  所需内存空间大小  到达时间  所需运行时间  进入内存时间  完成时间  周转时间");
		if (processAl == PSA) {
			printf("  优先数");
		}
		printf("\n");
		printf("   %d\t", done->jobId);
		printf("  %c\t", done->state);
		printf("   %dKB\t\t", done->memory);
		printf("  %d:%02d\t", done->chour, done->cminute);
		printf("    %d分钟\t", done->ntime);
		printf("  %d:%02d\t", done->ehour, done->eminute);
		printf("\t%d:%02d\t", done->fhour, done->fminute);
		printf("  %d\t", done->fhour * 60 + done->fminute - done->chour * 60 - done->cminute);
		if (processAl == PSA) {
			printf("    %d", done->super);
		}
		printf("\n");
		zz += (done->fhour * 60 + done->fminute - done->chour * 60 - done->cminute);
		float socre = (float)(done->fhour * 60 + done->fminute - done->chour * 60 - done->cminute) / done->ntime;
		printf("\n|作业%d的带权时间为%f\n\n", done->jobId, socre);
		result += socre;
		number++;
		done = done->next;
	}
	printf("系统平均周转时间为：%.4f min\n", zz / number);
	printf("系统平均带权周转时间为：%.4f min\n", result / number);
	return 0;
}