/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*
*	    Student id : 32121671, 32157437, 52131086
*	    Student name : 장선혁, 이은빈, 김도엽
*
*   lab1_sched_types.h :
*       - lab1 header file.
*       - must contains scueduler algorithm function's declations.
*
*/

#ifndef _LAB1_HEADER_H
#define _LAB1_HEADER_H

#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#define PROCESS_MAX_NUM 5 // The maximum number of process

#define TRUE 1
#define FALSE 0

typedef struct
{
    int processId;
    int arrivalTime;
    int serviceTime;
}ProcessInfo;

typedef struct
{
    ProcessInfo readyQueue[PROCESS_MAX_NUM]; //ready Queue는 구현의 편의를 위해 배열로 구현했습니다.
    int top, front; // top, front index of the queue.
    int size;
}Queue; // 이 Queue는 ProcessInfo를 넣을 수 있는 원형 큐 입니다.


typedef struct LotteryInfoStruct
{
    int processId;
    int serviceTime;
    int ticketMax;
    struct LotteryInfoStruct* next; // list로 구현.
    struct LotteryInfoStruct* prev; // list로 구현.
}LotteryInfo; //Lottery 스케줄링 용 구조체


ProcessInfo* queue_pop(Queue* q);
int queue_isempty(Queue* q);
void queue_push(Queue* q, ProcessInfo* p);

void InitResult();
void PrintResult();
void SJF();
void MLFQ(int tq);
LotteryInfo* copyLotteryInfo(ProcessInfo* pInfo);
void Lottery();
void FCFS();
void RR(int timeSlice);

#endif /* LAB1_HEADER_H*/



