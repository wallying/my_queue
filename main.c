/**
 * file:    main.c
 * author:  wallying@foxmail.com
 * date:    2019-12-17
 **/


#include <stdio.h>
#include <windows.h>
#include "lib_queue.h"


#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)   (sizeof(a) / sizeof(a[0]))
#endif


static DWORD WINAPI key_ISR(LPVOID lpParameter);


/*******************************************************************************
lib_queue循环队列使用方法:
1.定义队列变量(类型必须为queue_t)
    例如：queue_t circular_queue;
2.定义数据结构体
    例如：struct data_t {...};
3.定义数据结构体数组
    例如：struct data_t array[N];
4.使用队列API函数(函数形参必须为队列地址)
    queue_init(&circular_queue, array, sizeof(struct data_t), ARRAY_SIZE(array));
    queue_write(&circular_queue, &temp, 3);
    queue_read(&circular_queue, &temp, 2);
*******************************************************************************/

queue_t circular_queue;

typedef struct _data_t {
    unsigned char n1;
    unsigned char n2;
} myData_t;

myData_t Array[10] = {0x00};



void print_array(void)
{
    int idx = 0;
    for (idx = 0; idx < ARRAY_SIZE(Array); ++idx) {
        printf("[%02d,%02d] ", Array[idx].n1, Array[idx].n2);
    }

    if (queue_is_empty(&circular_queue)) {
        printf("-> empty!!!\n");
    } else if (queue_is_full(&circular_queue)) {
        printf("-> full!!!\n");
    } else {
        printf("-> (%d,%d) \n", queue_used_cnt(&circular_queue), queue_free_cnt(&circular_queue));
    }
}



int cnt = 0;
myData_t writeData[10];
/*----------------------------------------------------------------------------*/
void key_1_down(void)
{
    writeData[cnt].n1 = rand() % 100;
    writeData[cnt].n2 = rand() % 100;
    printf("writeData[%d] = [%02d,%02d]\n", cnt, writeData[cnt].n1, writeData[cnt].n2);

    ++cnt;
}
/*----------------------------------------------------------------------------*/
void key_2_down(void)
{
    int num = 0;

    if (cnt > 0) {
        num = queue_write(&circular_queue, writeData, cnt);
        if (num > 0) {
            printf("queue_write(%d) \n", num);
        } else {
            printf("queue_write: full! \n");
        }
        cnt = 0;
    }

    print_array();
}
/*----------------------------------------------------------------------------*/
void key_3_down(void)
{
    int num = 0;

    if (cnt > 0) {
        num = queue_write_rewind(&circular_queue, writeData, cnt);
        if (num > 0) {
            printf("queue_write_rewind(%d) \n", num);
        } else {
            printf("queue_write_rewind: error! \n");
        }
        cnt = 0;
    }

    print_array();
}
/*----------------------------------------------------------------------------*/
void key_4_down(void)
{
    int num = 0;

    if (cnt > 0) {
        num = queue_push(&circular_queue, writeData, cnt);
        if (num > 0) {
            printf("queue_push(%d) \n", num);
        } else {
            printf("queue_push: full! \n");
        }
        cnt = 0;
    }

    print_array();
}
/*----------------------------------------------------------------------------*/
void key_5_down(void)
{
    int num = 0;

    if (cnt > 0) {
        num = queue_push_rewind(&circular_queue, writeData, cnt);
        if (num > 0) {
            printf("queue_push_rewind(%d) \n", num);
        } else {
            printf("queue_push_rewind: error! \n");
        }
        cnt = 0;
    }

    print_array();
}
/*----------------------------------------------------------------------------*/
void key_0_down(void)
{
    int num = 0;
    myData_t temp[10];

    num = queue_read(&circular_queue, temp, 1);
    if (num > 0) {
        printf("queue_read(%d): \n", num);
        for (int i = 0; i < num; ++i) {
            printf("    %d -- (%2d,%2d) \n", i, temp[i].n1, temp[i].n2);
        }
    } else {
        printf("queue_read: empty! \n");
    }
    cnt = 0;

    print_array();
}
/*----------------------------------------------------------------------------*/





int main(int argc, char *argv[])
{
    HANDLE keyThread = CreateThread(0, 0, key_ISR, 0, 0, 0);
    if (keyThread == NULL) {
        exit(-1);
    }

    printf("hello lib_queue!\n");


    printf("\n\tKEY[1] : get_data() \n");
    printf("\tKEY[2] : queue_write() \n");
    printf("\tKEY[3] : queue_write_rewind() \n");
    printf("\tKEY[4] : queue_push() \n");
    printf("\tKEY[5] : queue_push_rewind() \n");
    printf("\tKEY[0] : queue_read() \n\n");

    queue_init(&circular_queue, Array, sizeof(myData_t), ARRAY_SIZE(Array));
    print_array();


    while (1) {

    }

    getchar();
    return 0;
}


/*============================================================================*/
struct key_isr_t {
    char key;
    void (*pDn)(void);
    void (*pUp)(void);
    unsigned char flag;
};

static struct key_isr_t key_isr[] = {
    {'1', key_1_down, NULL},
    {'2', key_2_down, NULL},
    {'3', key_3_down, NULL},
    {'4', key_4_down, NULL},
    {'5', key_5_down, NULL},
    {'0', key_0_down, NULL},
};

static DWORD WINAPI key_ISR(LPVOID lpParameter)
{
    int idx = 0;
    while (1) {
        for (idx = 0; idx < ARRAY_SIZE(key_isr); ++idx) {
            if (GetKeyState(key_isr[idx].key) < 0) {
                if (key_isr[idx].flag == 0) {
                    key_isr[idx].flag = 1;
                    if (key_isr[idx].pDn) {
                        key_isr[idx].pDn();
                    }
                }
            } else {
                if (key_isr[idx].flag == 1) {
                    key_isr[idx].flag = 0;
                    if (key_isr[idx].pUp) {
                        key_isr[idx].pUp();
                    }
                }
            }
        }
    }
    return 0;
}

