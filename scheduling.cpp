#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduling.h"

//========================ȫ�ֱ���========================

PCB executeProcess;			//ִ�н���
PCB readyProcess = NULL;	//�������̶���
JCB doneJob = NULL;			//�����ҵ����
JCB readyJob = NULL;		//������ҵ���У��������ڴ棩
JCB waitJob = NULL;			//�ȴ���ҵ���У��ȴ������ȣ�
JCB nonarrivalJob = NULL;	//δ������ҵ
JCB selectJob;				//ѡ����ҵ
JCB executeJob;				//ִ����ҵ
LinkedFPT partHead;			//����ͷ���
Queue queueHead;			//��һ����

int Memory;				//�ڴ�ռ��С
int device;				//��ʹ�õĴŴ�������
int partNum = 1;		//�����ż���
int robin;				//ʱ��Ƭ
int round = 0;			//ʱ��Ƭʱ��
int hour = 9;			//ϵͳСʱ
int minute = 55;		//ϵͳ����

//=================ѡ���㷨=====================

int processAl;			//���̵����㷨
int jobAl;				//��ҵ�����㷨
int partAl;				//���з����㷨

//==============================================

/*
* ��ʼ��
*/
void init() {

	//ѡ���㷨
	printf("\n������ѡ�����ҵ�����㷨��1.�����ȷ���  2.����ҵ���ȣ���");
	scanf("%d", &jobAl);
	printf("\n������ѡ��Ľ��̵����㷨��1.�����ȷ���  2.�̽�������  3.ʱ��Ƭ��ת  4.�༶��������  5.���ȼ����ȣ���");
	scanf("%d", &processAl);
	if (processAl == RR) {
		printf("\n������ʱ��Ƭ��С:");
		scanf("%d", &robin);
	}
	if (processAl == MFQ) {
		initMFQ();
	}
	printf("\n������ѡ����ڴ�����㷨��1.�״���Ӧ�㷨  2.�����Ӧ�㷨����");
	scanf("%d", &partAl);
	printf("\n�������ڴ�ռ��С��");
	scanf("%d", &Memory);
	printf("\n�������豸������");
	scanf("%d", &device);

	//��ʼ����ҵ���̶��е�ͷ���
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

	//��ʼ�����з���
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
* ��ʼ���༶��������
*/
void initMFQ() {
	queueHead = NULL;
	int i, num, round;
	printf("���������õľ������и���:");
	scanf("%d", &num);
	printf("�������һ�����е�ʱ��Ƭ:");
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
* ������ҵ
*/
void input() {
	int i, num;
	printf("\n��������ҵ��:");
	scanf("%d", &num);
	for (i = 0; i < num; i++)
	{
		printf("\n���No.%d:\n", i + 1);
		JCB p = (JCB)malloc(sizeof(jcb));
		if (p == NULL) {
			printf("ϵͳ����\n");
			return;
		}
		printf("\n������ҵ��:");
		scanf("%d", &p->jobId);
		if (processAl == PSA) {
			printf("\n������ҵ������:");
			scanf("%d", &p->super);
		}
		printf("\n������ҵ����ʱ�̣�Сʱ��:");
		scanf("%d", &p->chour);
		printf("\n������ҵ����ʱ�̣����ӣ�:");
		scanf("%d", &p->cminute);
		printf("\n������ҵ����ʱ��:");
		scanf("%d", &p->ntime);
		printf("\n������ҵ�����ڴ�ռ��С��KB��:");
		scanf("%d", &p->memory);
		printf("\n������ҵ�����豸��:");
		scanf("%d", &p->device);
		printf("\n");
		p->state = 'W';
		p->next = NULL;
		//����ȴ����У��ѵ������ҵ��
		if (p->chour * 60 + p->cminute <= hour * 60 + minute) {
			if (jobAl == FCFS) {
				//�����ȷ���
				jobFCFSSort();
			}
			else {
				//����ҵ����
				jobSJFSort();
			}
		}
		else {
			//����δ������У�δ�������ҵ��
			JCB node = nonarrivalJob;
			while (node->next != NULL) {
				node = node->next;
			}
			node->next = p;
		}
	}
}

/*
* ��ʱ
*/
void clock() {
	minute++;
	if (minute >= 60) {
		hour++;
		minute = 0;
	}
}

/*
* ��ӡ�������
*/
void display(PCB pr)
{
	printf(" ���̺�  ״̬  ��������ʱ��  ����ʱ�� \n");
	printf("  %d\t", pr->pid);
	printf("  %c\t", pr->state);
	printf("  %d\t", pr->ntime);
	printf("\t%d\t", pr->rtime);
	printf("\n");
}

/*
* ��ӡ�����ҵ���
*/
void display(JCB job)
{
	printf("\n");
	printf(" ��ҵ��  ״̬  �����ڴ�ռ��С  ����ʱ��  ��������ʱ��  �����ڴ�ʱ��  ���ʱ��  ��תʱ��\n");
	printf("   %d\t", job->jobId);
	printf("  %c\t", job->state);
	printf("   %dKB\t\t", job->memory);
	printf("  %d:%02d\t", job->chour, job->cminute);
	printf("    %d����\t", job->ntime);
	printf("  %d:%02d\t", job->ehour, job->eminute);
	printf("\t%d:%02d\t", job->fhour, job->fminute);
	printf("  %d\t", job->fhour * 60 + job->fminute - job->chour * 60 - job->cminute);
	printf("\n");
}

/*
* ��ʾ
*/
void display() {
	//==========��ʾ��ѡ����ҵ===========
	if (executeJob != NULL) {
		printf("\nĿǰ����ִ�е���ҵ���£�\n");
		printf(" ��ҵ��  ״̬  �����ڴ�ռ��С  �����豸��  ����ʱ��  ��������ʱ��  �����ڴ�ʱ��  ����ʱ��\n");
		printf("   %d\t", executeJob->jobId);
		printf("  %c\t", executeJob->state);
		printf("   %dKB\t\t", executeJob->memory);
		printf("    %d\t", executeJob->device);
		printf("      %d:%02d\t", executeJob->chour, executeJob->cminute);
		printf("  %d����\t", executeJob->ntime);
		printf("%d:%02d\t", executeJob->ehour, executeJob->eminute);
		printf("    %d����\t", executeProcess->rtime);
		printf("\n");
	}
	else {
		printf("��ǰ������ִ�е���ҵ\n");
	}

	//==========��ʾ�ڴ������===========
	LinkedFPT p = partHead;
	printf("\n===============���з�����====================\n\n");
	printf("������ ��ʼ��ַ ������С ״̬\n");
	while (p != NULL) {
		if (p->data.state == FREE) {
			printf("|%d\t", p->data.number);
			printf("|%d\t", p->data.index);
			printf("|%dKB\t", p->data.size);
			printf("|����\n");
		}
		else {
			printf("|%d\t", p->data.number);
			printf("|%d\t", p->data.index);
			printf("|%dKB\t", p->data.size);
			printf("|�ѷ���\t");
			printf("|������ҵ��:%d\n", p->data.job);
		}
		p = p->next;
	}

	//=========��ǰ�豸���=========
	printf("\n=========��ǰ�豸ʹ�����==========\n");
	printf("�豸������%d\tʣ������豸����%d\n", DEVICE, device);
}

/*
* �ڴ����
*/
int alloc(int job, int needSize) {
	LinkedFPT p = partHead;
	while (p != NULL) {
		if (p->data.state == FREE) {
			//����С��ȣ���ֱ�ӷ���
			if (p->data.size == needSize) {
				p->data.state = ALLOCATED;
				p->data.job = job;
				break;
			}
			//��������������������С���򻮷ֳ�һ���ڴ�ռ��ȥ
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
* �ڴ����
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
			//���һ:������ͬʱ������ǰ�����������з������ڽ�
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
			//�����:������������ǰһ�����з������ڽ�
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
			//�����:�������������һ�����з������ڽ�
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
			//�����:�����������κο��з�������
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
* �Կ��з�������������
*/
void sortPart() {
	int temp;
	LinkedFPT slow = partHead;
	LinkedFPT fast = NULL;
	LinkedFPT mid = NULL;	//midΪͨ��ѡ������õ�slowָ�������з�����С�Ľ��
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
		//��midΪNULL��mid��������������з�����������
		if (mid == NULL || mid->data.size >= slow->data.size) {
			break;
		}
		else {
			//����slow��mid���
			temp = slow->data.number;
			slow->data.number = mid->data.number;
			mid->data.number = temp;
			temp = slow->data.index;
			slow->data.index = mid->data.index;
			mid->data.index = temp;
			temp = slow->data.size;
			slow->data.size = mid->data.size;
			mid->data.size = temp;
			//������ָ��
			slow = mid;
		}
	}
}

/*
* ���´ﵽ��δ������ҵ����ȴ�����
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
* �����ȷ������򣨶Եȴ����н�������
*/
void jobFCFSSort() {
	//������ʱ�������������ǰ��
	JCB p = waitJob;
	while (p->next != NULL) {
		p = p->next;
	}
	p->next = selectJob;
}

/*
* ����ҵ���ȵ������򣨶Եȴ����н�������
*/
void jobSJFSort() {
	//������ʱ�������������ǰ��
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
* ��ҵ���ȣ����ȴ����е���ҵ�����ڴ棬����������У�
*/
void jobScheduling() {
	JCB wait = waitJob;
	while (wait->next != NULL) {
		if (wait->next->device <= device) {
			JCB job = wait->next;
			if (partAl == 2) {
				//�����Ӧ�㷨���Կ��з�������
				sortPart();
			}
			//���ڴ����豸���㹻��������ڴ�
			if (alloc(job->jobId, job->memory) == 1) {
				wait->next = job->next;
				device -= job->device;
				job->ehour = hour;
				job->eminute = minute;
				job->state = 'R';
				job->next = readyJob->next;
				readyJob->next = job;
				//Ϊ��ҵ����PCB����
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
				//���½��̼���������̶���
				if (processAl == FCFS || processAl == RR) {
					//�����ȷ���/ʱ��Ƭ��ת ���̵�������
					while (node->next != NULL) {
						node = node->next;
					}
					node->next = p;
				}
				else if (processAl == PSA) {
					//��������������
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
					//�̽������ȵ�������
					while (node->next != NULL) {
						//��������ʱ��̵�����ǰ�棨��������ʱ��-����ʱ�䣩
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
					//����༶�������е�һ���ж�β
					PCB first = queueHead->pcb;
					p->round = queueHead->round;		//��ֵʱ��Ƭ
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
				//�ڴ治��
				wait = wait->next;
			}
		}
		else {
			//�豸����
			wait = wait->next;
		}
	}
}

/*
* ���ٽ��̣�������ɶ���
*/
void destroy() {
	JCB node = readyJob;
	while (node->next != NULL) {
		if (node->next->jobId == executeProcess->pid) {
			selectJob = node->next;
			node->next = selectJob->next;
			selectJob->fhour = hour;
			selectJob->fminute = minute;
			device += selectJob->device;	//�����豸
			//���������ҵ����
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

	//��Ϊ���������㷨����Ӿ����������Ƴ�
	if (processAl == MFQ) {
		Queue node = queueHead;
		//�ҵ���Ӧ�ľ�������
		while (node != NULL) {
			if (node->round == executeProcess->round) {
				node->pcb = executeProcess->next;
				break;
			}
			node = node->next;
		}
	}

	printf("\n ��ҵ [%d] �����.\n", executeProcess->pid);
	freePart(executeProcess->pid);
	free(executeProcess);
	executeProcess = NULL;
	executeJob = NULL;
}

/*
* �̽������ȵ���
*/
void processSJF() {
	if (executeProcess != NULL) {
		executeProcess->rtime++;
		if (executeProcess->ntime == executeProcess->rtime) {
			destroy();
			jobScheduling();	//���ϵ����ڴ�
		}
	}
	//�ж��Ƿ�����ռ
	if (executeProcess != NULL && processAl == 2) {
		PCB node = readyProcess->next;
		if (node != NULL && node->ntime < (executeProcess->ntime - executeProcess->rtime)) {
			//��ռ
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
	//չʾִ����ҵ
	JCB node = readyJob->next;
	while (node != NULL && executeProcess != NULL) {
		if (node->jobId == executeProcess->pid) {
			executeJob = node;
		}
		node = node->next;
	}
}

/*
* �������ȼ�����
*/
void processPSA() {
	if (executeProcess != NULL) {
		executeProcess->rtime++;
		if (executeProcess->ntime == executeProcess->rtime) {
			destroy();
			jobScheduling();	//���ϵ����ڴ�
		}
	}
	//�ж��Ƿ�����ռ
	if (executeProcess != NULL && processAl == PSA) {
		PCB node = readyProcess->next;
		if (node != NULL && node->super < executeProcess->super) {
			//��ռ
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
	//չʾִ����ҵ
	JCB node = readyJob->next;
	while (node != NULL && executeProcess != NULL) {
		if (node->jobId == executeProcess->pid) {
			executeJob = node;
		}
		node = node->next;
	}
}

/*
* ���������ȷ������
*/
void processFCFS() {
	if (executeProcess != NULL) {
		executeProcess->rtime++;
		if (executeProcess->ntime == executeProcess->rtime) {
			destroy();
			jobScheduling();	//���ϵ����ڴ�
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
	//չʾִ����ҵ
	JCB node = readyJob->next;
	while (node != NULL && executeProcess != NULL) {
		if (node->jobId == executeProcess->pid) {
			executeJob = node;
		}
		node = node->next;
	}
}

/*
* ʱ��Ƭ��ת
*/
void processRound() {
	if (executeProcess != NULL) {
		round++;
		executeProcess->rtime++;
		if (executeProcess->ntime == executeProcess->rtime) {
			destroy();
			round = 0;
			jobScheduling();	//���ϵ����ڴ�
		}
		else {
			//ʱ��Ƭ�ѵ�
			if (round == robin) {
				printf("\n����%d ʱ��Ƭ�ѵ�\n", executeProcess->pid);
				PCB node = readyProcess;
				//�����������̶��ж�β
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
				printf("\n����%d ������ʱ��Ƭ��%d\n", executeProcess->pid, round);
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
	//չʾִ����ҵ
	JCB node = readyJob->next;
	while (node != NULL && executeProcess != NULL) {
		if (node->jobId == executeProcess->pid) {
			executeJob = node;
		}
		node = node->next;
	}
}


/*
* �������̲鿴����
*/
void check()
{
	if (executeProcess == NULL) {
		return;
	}
	int count = 1;
	PCB pr;
	Queue node = queueHead;
	printf("\n ============��ǰ�������еĽ�����:\n"); /*��ʾ��ǰ���н���*/
	display(executeProcess);
	printf("\n ============��ǰ�༶��������״̬Ϊ:\n"); /*��ʾ��������״̬*/
	while (node != NULL) {
		printf("\n#��������%d��ʱ��ƬΪ%d��:\n\n", count++, node->round);
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
* ������һ�����ȼ���ߵĽ���
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
* �ж��Ƿ��и������ȼ��Ľ��̵���
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
		//�������ȼ����������ռ
		if (find->round < executeProcess->round) {
			//�ҵ���Ӧ�ľ�������
			while (node != NULL) {
				if (node->round == executeProcess->round) {
					break;
				}
				node = node->next;
			}
			//ԭ��λ�ڶ��ף��ֽ��������β
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
			//ִ�н���nextָ�벻��ΪNULL�����ɴ��ڷ���������
			find->state = 'R';
			executeProcess = find;
			round = 0;
		}
	}
}

/*
* �����Խ��̽������ȼ����к���
*/
void sort()
{
	Queue node = queueHead;
	//�ҵ���Ӧ�ľ�������
	while (node != NULL) {
		if (node->round == executeProcess->round) {
			break;
		}
		node = node->next;
	}
	//������������ȼ����У���ֱ�ӷ�����β
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
		//������һ���ȼ����еĶ�β
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
* �༶�������н��̵���
*/
void processMFQ() {
	if (executeProcess != NULL) {
		round++;
		executeProcess->rtime++;
		if (executeProcess->ntime == executeProcess->rtime) {
			destroy();
			round = 0;
			jobScheduling();	//���ϵ����ڴ�
		}
		else {
			if (executeProcess->round <= round) {
				//��ʱ��Ƭ���꣬�������ȼ�����
				sort();
				executeProcess->state = 'W';
				executeProcess = NULL;
				round = 0;
			}
			else {
				printf("\n����%d ������ʱ��Ƭ��%d\n", executeProcess->pid, round);
				//�ж��Ƿ��и������ȼ��Ľ���
				analyse();
			}
		}
	}
	if (executeProcess == NULL) {
		find();
	}
	//չʾִ����ҵ
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
		printf("\n\n��ǰʱ��Ϊ %d:%02d\n", hour, minute);

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
		//ʱ��+1������ʱ���+1
		clock();
		come();
		printf("\n--------------------------------------------\n");
		//ch = getchar();
	}
	printf("\n=============��ҵ��ȫ�����=============\n\n");
	float zz = 0;
	float result = 0;
	int number = 0;
	JCB done = doneJob->next;
	while (done != NULL) {
		printf("\n");
		printf(" ��ҵ��  ״̬  �����ڴ�ռ��С  ����ʱ��  ��������ʱ��  �����ڴ�ʱ��  ���ʱ��  ��תʱ��");
		if (processAl == PSA) {
			printf("  ������");
		}
		printf("\n");
		printf("   %d\t", done->jobId);
		printf("  %c\t", done->state);
		printf("   %dKB\t\t", done->memory);
		printf("  %d:%02d\t", done->chour, done->cminute);
		printf("    %d����\t", done->ntime);
		printf("  %d:%02d\t", done->ehour, done->eminute);
		printf("\t%d:%02d\t", done->fhour, done->fminute);
		printf("  %d\t", done->fhour * 60 + done->fminute - done->chour * 60 - done->cminute);
		if (processAl == PSA) {
			printf("    %d", done->super);
		}
		printf("\n");
		zz += (done->fhour * 60 + done->fminute - done->chour * 60 - done->cminute);
		float socre = (float)(done->fhour * 60 + done->fminute - done->chour * 60 - done->cminute) / done->ntime;
		printf("\n|��ҵ%d�Ĵ�Ȩʱ��Ϊ%f\n\n", done->jobId, socre);
		result += socre;
		number++;
		done = done->next;
	}
	printf("ϵͳƽ����תʱ��Ϊ��%.4f min\n", zz / number);
	printf("ϵͳƽ����Ȩ��תʱ��Ϊ��%.4f min\n", result / number);
	return 0;
}