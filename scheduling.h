#ifndef SCHEDULING_H_INCLUDED
#define SCHEDULING_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY 100		//����ռ��ܴ�С
#define DEVICE 4		//�Ŵ�������

#define FREE 0				//����
#define ALLOCATED 1			//�ѷ���

//========================�㷨ѡ��========================

#define FCFS 1			//�����ȷ����㷨
#define SJF	2			//����ҵ�����㷨
#define RR 3			//����ʱ��Ƭ��ת
#define MFQ 4			//�༶��������
#define PSA 5			//���ȼ������㷨
#define FIRSTFIT 1		//�״���Ӧ�㷨
#define BESTFIT 2		//�����Ӧ�㷨

//========================���ݽṹ========================

typedef struct jcb {	//��ҵ���ƿ�
	int jobId;			//��ҵ��
	char state;			//״̬
	int super;			//��Ӧ�������ȼ�
	int chour;			//����ʱ�̣�Сʱ��
	int cminute;		//����ʱ�̣����ӣ�
	int ntime;			//��������ʱ�䣨���ӣ�
	int ehour;			//�����ڴ�ʱ�̣�Сʱ��
	int eminute;		//�����ڴ�ʱ�̣����ӣ�
	int fhour;			//���ʱ�̣�Сʱ��
	int fminute;		//���ʱ�̣����ӣ�
	int memory;			//�����ڴ�ռ��С
	int device;			//�����豸��
	struct jcb* next;
}*JCB, jcb;

typedef struct pcb {  //���̿��ƿ�PCB 
	int pid;		  //���̺ţ�����ҵ����ͬ��
	char state;		  //״̬
	int round;		  //ʱ��Ƭ
	int ntime;		  //��������ʱ�䣨���ӣ�
	int rtime;		  //����ʱ�䣨���ӣ�
	int super;		  //���ȼ�
	struct pcb* next;
}*PCB, pcb;

typedef struct queue {	//�༶��������
	PCB pcb;		//��������ͷ���
	int priority;	//���ȼ�
	int round;		//ʱ��Ƭ
	struct queue* next;
}*Queue, queue;

typedef struct freePartition {		//���з���
	int number;		//������
	int index;		//��ʼ��ַ
	int	size;		//������С
	int state;		//״̬
	int job;		//��ҵ��
	struct freePartition* next;
}*FPT, fpt;

typedef struct Linkedfpt {		//���з�����
	fpt data;
	struct Linkedfpt* pre;
	struct Linkedfpt* next;
}*LinkedFPT, Linkedfpt;

//========================��������========================

void init();		//��ʼ��
void initMFQ();		//��ʼ���༶��������
void input();		//������ҵ
void clock();		//��¼��ǰʱ��
void display(PCB pr);	//��ӡ�������
void display(JCB job);	//��ӡ�����ҵ���
void display();		//��ӡ���з�����
int alloc(int job, int needSize);	//�����ڴ�
void freePart(int job);				//�ڴ����
void sortPart();					//�Կ��з�������������		
void come();		//���´ﵽ��δ������ҵ����ȴ�����
void jobFCFSSort();	//��ҵ�����ȷ������򣨶Եȴ����н�������
void jobSJFSort();	//����ҵ���ȵ������򣨶Եȴ����н�������
void jobScheduling();		//��ҵ���ȣ����ȴ����е���ҵ�����ڴ棬����������У�
void destroy();		//���ٽ��̣�������ɶ���
void processFCFS();		//���������ȷ�����ȣ��Ӿ���������ȡ��ִ�У�
void processPSA();		//�������ȼ�����
void processSJF();		//�̽������ȵ��ȣ��Ӿ���������ȡ��ִ�У�
void processRound();	//ʱ��Ƭ��ת
void check();			//�������̲鿴����
void find();			//������һ�����ȼ���ߵĽ���
void analyse();			//�ж��Ƿ��и������ȼ��Ľ��̵���
void sort();			//�����Խ��̽������ȼ����к���
void processMFQ();		//�༶�������н��̵���


#endif