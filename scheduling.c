#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>   //pow

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

ProcessInfo* queue_pop(Queue* q)
{
    ProcessInfo* pi = &q->readyQueue[q->front];
    q->front = (q->front+1) % PROCESS_MAX_NUM;
    return pi;
}

int queue_isempty(Queue *q)
{
    if(q->front == q->top)
        return TRUE;
    else
        return FALSE;
}

void queue_push(Queue *q, ProcessInfo* p)
{
    if ((q->top + 1) % PROCESS_MAX_NUM == q->front){
        printf("Queue overflow.\n");
        return;
    }

    q->readyQueue[q->top].processId = p->processId;
    q->readyQueue[q->top].arrivalTime = p->arrivalTime;
    q->readyQueue[q->top].serviceTime = p->serviceTime;
    q->top = (q->top + 1) % PROCESS_MAX_NUM;
}


ProcessInfo pInfos[PROCESS_MAX_NUM]; // 입력받은 프로세스 정보들.
int *result[PROCESS_MAX_NUM]; // 결과 출력용 2차원 배열
int totalServeTime=0;         // ServeTime의 총합

// result를 totalServeTime*PROCESS_MAX_NUM 크기만큼 할당하고 0으로 전부 초기화.
void InitResult()
{
    int i,j;

    for(i=0;i<PROCESS_MAX_NUM;i++)
    {
        result[i] = (int*)malloc(totalServeTime*sizeof(int));
        for(j=0;j<totalServeTime;j++)
            result[i][j] = 0;
    }
}

//result에 저장된 결과에 따라 출력.
void PrintResult()
{
    int i,j;

    for(i=0;i<PROCESS_MAX_NUM;i++)
    {
        printf("%c ", (pInfos[i].processId + 'A'));
        for(j=0;j<totalServeTime;j++)
        {
            if(result[i][j] == 0)
                printf("   ");
            else
                printf("  ■");
        }
        printf("\n");
    }
}

void copyProcessInfo(ProcessInfo* t1, ProcessInfo* t2)
{
    t1->processId = t2->processId;
    t1->arrivalTime = t2->arrivalTime;
    t1->serviceTime = t2->serviceTime;
}
// FCFS, SJF, RR, MLFQ, Lottery
void SJF()
{
    int i;
    int currentTime; //현재 실행중인 시간
    ProcessInfo readySet[PROCESS_MAX_NUM];
    ProcessInfo front; // ready queue의 가장 앞의 원소를 저장합니다
    int minServ = 999;
    int minId = -1;

    InitResult();

    for(i=0;i<PROCESS_MAX_NUM;i++)
        readySet[i].processId = -1;

    copyProcessInfo(&front, &pInfos[0]);

    for(currentTime=0; currentTime<totalServeTime; currentTime++)
    {
        // 현재 시간이 arrivalTime에 해당되면 queue에 추가시켜줍니다.
        for(i=1;i<PROCESS_MAX_NUM;i++)
            if(pInfos[i].arrivalTime == currentTime)
                copyProcessInfo(&readySet[i], &pInfos[i]);

        result[front.processId][currentTime] = 1; // 현재 프로세스, 현재 시간에 프로세스가 진행중이란 것을 표시합니다.
        front.serviceTime--;           // 프로세스가 처리 되었으므로 readyQueue에 있는 프로세스의 서비스 시간을 줄여줍니다.

        // 서비스 시간이 끝나면 큐의 가장 앞 원소를 제거 합니다.
        if(front.serviceTime == 0)
        {
            readySet[front.processId].processId = -1;
            minServ = 999;
            minId = -1;

            for(i=0;i<PROCESS_MAX_NUM;i++)
            {
                if(readySet[i].processId != -1)
                {
                    if(minServ > readySet[i].serviceTime)
                    {
                        minServ = readySet[i].serviceTime;
                        minId = i;
                    }
                }
            }
            if(minId != -1)
                copyProcessInfo(&front, &readySet[minId]);
        }
    }

    PrintResult();
}

// parameter tq는 time quantum 입니다. tq^i가 각 큐의 time quantum이 됩니다.
void MLFQ(int tq)
{
    /*
       int i;
       int currentTime; //현재 실행중인 시간
       int sumTQ = 0; //누적 time quantum
       int preemptedQId = 0;

       Queue *readyQ;
       int numOfQueue = 3; //q의 갯수는 3개라고 가정.

       ProcessInfo* runningProcess = NULL; // 실행중인 프로세스

       readyQ = (Queue*)malloc(numOfQueue*sizeof(Queue));

       for(i=0;i<numOfQueue;i++)
       readyQ[i].top = readyQ[i].front = readyQ[i].size = 0;

       InitResult();

       runningProcess = &pInfos[0];
       for(currentTime=0; currentTime<totalServeTime; currentTime++)
       {
    //process가 도착하면 가장 상위의 queue에 넣습니다.
    for(i=1;i<PROCESS_MAX_NUM;i++)
    if(pInfos[i].arrivalTime-1 == currentTime)
    queue_push(&readyQ[0], &pInfos[i]);

    result[runningProcess->processId][currentTime] = 1;
    runningProcess->serviceTime--;

    // 이하 다음 runningProcess를 결정하는 부분
    int t = pow(tq, preemptedQId);
    if(sumTQ < t) // tq이 아직 지나지 않았다면
    {
    sumTQ++;
    if(runningProcess->serviceTime > 0)
    continue;   // 서비스가 끝났지 않았다면 이하 생략
    }
    sumTQ = 0;

    //queue 밀어주는 처리 부분
    for(i=0;i<numOfQueue;i++)
    {
    if(queue_isempty(&readyQ[i]) == FALSE) // queue가 비어있지 않다면
    {
    if(preemptedQId + 1 < numOfQueue) // 우선순위를 낮출 수 있으면
    {
    queue_push(&readyQ[preemptedQId+1], runningProcess); // 우선순위 낮추기
    preemptedQId = i+1;
    }
    else
    queue_push(&readyQ[preemptedQId], runningProcess);

    runningProcess = queue_pop(&readyQ[i]);
    }
    }
    }

    PrintResult();
     */
}
LotteryInfo* copyLotteryFromInfo(ProcessInfo* pInfo)
{
    srand((unsigned int)time(NULL));

    LotteryInfo* lottery = (LotteryInfo*)malloc(sizeof(LotteryInfo));
    lottery->processId = pInfo->processId;
    lottery->serviceTime = pInfo->serviceTime;
    lottery->next = NULL;
    lottery->prev = NULL;

    return lottery;
}
void Lottery()
{
    int currentTime;
    int i;
    LotteryInfo* lotteryHead = NULL; //lottery들은 전부 list입니다.
    LotteryInfo *t=NULL, *t2 = NULL, *t_prev=NULL; //임시 변수

    int lastTicketMax=0; // 총 티켓 수, 실시간 ticket수에 따라 업데이트 됩니다.
    int winner;

    InitResult();

    //시드 값 생성
    srand((unsigned int)time(NULL));
    for(currentTime=0;currentTime<totalServeTime;currentTime++)
    {
        //현재 시간이 arrivalTime에 해당되면 list에 추가
        for(i=0;i<PROCESS_MAX_NUM;i++)
        {
            if(pInfos[i].arrivalTime == currentTime)
            {
                if(lotteryHead == NULL)
                    lotteryHead = copyLotteryFromInfo(&pInfos[i]);
                else
                {
                    t = lotteryHead;
                    while(t->next)
                        t = t->next;

                    t2 = copyLotteryFromInfo(&pInfos[i]);
                    t->next = t2;
                    t2->prev = t;
                }
            }
        }

        // 티켓을 분배합니다.
        t = lotteryHead;
        while(t->next != NULL)
        {
            lastTicketMax = t->ticketMax;
            t = t->next;
        }
        t->ticketMax = (lastTicketMax+1) + (rand() % 51); // lastTicketMax+1 부터 lastTicketMax+1+50 사이의 난수 생성.

        //승자 선출
        winner = rand() % t->ticketMax;

        if(lotteryHead == NULL)
        {
            printf("lottery head is NULL\n");
            continue;
        }

        //승자 스케줄
        t = lotteryHead;

        while(t->next)
        {
            if(t->ticketMax > winner)
                break;

            t=t->next;
        }

        result[t->processId][currentTime] = 1;
        t->serviceTime--;

        //서비스가 끝나면 리스트에서 제거
        if(t->serviceTime == 0)
        {
            t_prev = t->prev;

            if(t_prev == NULL) // t가 head라는 뜻.
            {
                if(lotteryHead->next != NULL)
                {
                    lotteryHead = lotteryHead->next;
                    lotteryHead->prev = NULL;
                }
                else
                    lotteryHead = NULL;
            }
            else
                t_prev->next = t->next;

            free(t);
            t = NULL;
        }
    }

    PrintResult();
}

void FCFS()
{
    int i;
    int currentTime; //현재 실행중인 시간
    Queue ready_q;
    ProcessInfo* front; // ready queue의 가장 앞의 원소를 저장합니다.

    ready_q.top = ready_q.front = ready_q.size = 0;

    queue_push(&ready_q, &pInfos[0]); //가장 첫번째 프로세스를 ready queue에 넣습니다.
    front = queue_pop(&ready_q);

    InitResult();

    for(currentTime=0; currentTime<totalServeTime; currentTime++)
    {
        // 현재 시간이 arrivalTime에 해당되면 queue에 추가시켜줍니다.
        for(i=1;i<PROCESS_MAX_NUM;i++)
            if(pInfos[i].arrivalTime == currentTime)
                queue_push(&ready_q, &pInfos[i]);

        result[front->processId][currentTime] = 1; // 현재 프로세스, 현재 시간에 프로세스가 진행중이란 것을 표시합니다.
        front->serviceTime--;           // 프로세스가 처리 되었으므로 readyQueue에 있는 프로세스의 서비스 시간을 줄여줍니다.

        // 서비스 시간이 끝나면 큐의 가장 앞 원소를 제거 합니다.
        if(front->serviceTime == 0)
            front = queue_pop(&ready_q);
    }

    PrintResult();
}

// RR, Round-Robin
void RR(int timeSlice)
{
    int i;
    int currentTime; //현재 실행중인 시간
    Queue ready_q;
    ProcessInfo* front; // ready queue의 가장 앞의 원소를 저장합니다.
    int sumOfTimeSlice = 0; // 누적 timeslice 시간

    ready_q.top = ready_q.front = ready_q.size = 0;

    queue_push(&ready_q, &pInfos[0]); //가장 첫번째 프로세스를 ready queue에 넣습니다.
    front = queue_pop(&ready_q);

    InitResult();

    for(currentTime=0; currentTime<totalServeTime; currentTime++)
    {
        // 현재 시간이 arrivalTime에 해당되면 queue에 추가시켜줍니다.
        for(i=1;i<PROCESS_MAX_NUM;i++)
            if(pInfos[i].arrivalTime-1 == currentTime)
                queue_push(&ready_q, &pInfos[i]);

        //////
        // insert scheduling implementation code here.
        result[front->processId][currentTime] = 1; // 현재 프로세스, 현재 시간에 프로세스가 진행중이란 것을 표시합니다.
        front->serviceTime--;           // 프로세스가 처리 되었으므로 readyQueue에 있는 프로세스의 서비스 시간을 줄여줍니다.

        // 서비스 시간이 끝나면 큐의 가장 앞 원소를 제거 합니다.
        if(front->serviceTime == 0)
        {
            front = queue_pop(&ready_q);
            sumOfTimeSlice = 0;
            continue; // ts 처리 부분은 스킵합니다.
        }

        // Time Slice 처리 부분
        sumOfTimeSlice++;
        if(sumOfTimeSlice >= timeSlice) // 누적 ts가 넘어가면
        {
            if(queue_isempty(&ready_q) == FALSE) //queue가 비어있지 않을 경우엔
            {
                queue_push(&ready_q, front);
                front = queue_pop(&ready_q);     //프로세스를 전환합니다.
                sumOfTimeSlice = 0;
            }
        }
    }

    PrintResult();
}
int main()
{
    int i;

    pInfos[0].processId = 0;
    pInfos[0].arrivalTime = 0;
    pInfos[0].serviceTime = 3;

    pInfos[1].processId = 1;
    pInfos[1].arrivalTime = 2;
    pInfos[1].serviceTime = 6;

    pInfos[2].processId = 2;
    pInfos[2].arrivalTime = 4;
    pInfos[2].serviceTime = 4;

    pInfos[3].processId = 3;
    pInfos[3].arrivalTime = 6;
    pInfos[3].serviceTime = 5;

    pInfos[4].processId = 4;
    pInfos[4].arrivalTime = 8;
    pInfos[4].serviceTime = 2;

    //    for(i=0;i<PROCESS_MAX_NUM;i++)
    //    {
    //        pInfos[i].processId = i;
    //        printf("Input Arrival Time of %c process: ", pInfos[i].processId + 'A');
    //        scanf("%d", &pInfos[i].arrivalTime);
    //        
    //        printf("Input Service Time of %c process: ", pInfos[i].processId + 'A');
    //        scanf("%d", &pInfos[i].serviceTime);
    //    }

    for(i=0;i<PROCESS_MAX_NUM;i++)
        totalServeTime += pInfos[i].serviceTime;

    printf("======================== Time Elapsed =========================\n  ");
    for(i=0;i<totalServeTime;i++)
        printf("%3d",i+1);
    printf("\n");

    printf("========================= FCFS Result =========================\n");

    FCFS();

    printf("======================= RR Result(ts=1) =======================\n");

    RR(1);

    printf("======================= RR Result(ts=4) =======================\n");

    RR(4);

    printf("========================= SJF Result ==========================\n");

    SJF();

    printf("======================= MLFQ Result(q=1) ======================\n");

    MLFQ(1);

    printf("======================= MLFQ Result(q=2^i) ======================\n");

    MLFQ(2);

    printf("======================== Lottery Result =======================\n");

    Lottery();


    return 0;
}
