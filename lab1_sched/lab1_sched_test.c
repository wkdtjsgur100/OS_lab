/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32121671, 32157437, 52131086
*	    Student name : 장선혁, 이은빈, 김도엽
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm test code.
*
*/

#include "lab1_sched_types.h"

extern ProcessInfo pInfos[PROCESS_MAX_NUM]; // 입력받은 프로세스 정보들.
extern int *result[PROCESS_MAX_NUM]; // 결과 출력용 2차원 배열
extern int totalServeTime;         // ServeTime의 총합

int main()
{
    int i;

    for(i=0;i<PROCESS_MAX_NUM;i++)
    {
    pInfos[i].processId = i;
    printf("Input Arrival Time of %c process: ", pInfos[i].processId + 'A');
    scanf("%d", &pInfos[i].arrivalTime);
    
    printf("Input Service Time of %c process: ", pInfos[i].processId + 'A');
    scanf("%d", &pInfos[i].serviceTime);
    }

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

    printf("============================= SJF  ============================\n");

    SJF();

    printf("======================== Lottery Result =======================\n");

    Lottery();


    return 0;
}
