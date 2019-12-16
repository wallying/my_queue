/**
 * file:    main.c
 * author:  wallying@foxmail.com
 * date:    2019-12-16
 **/


#include <stdio.h>
#include <windows.h>
#include "lib_queue.h"


#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)   (sizeof(a) / sizeof(a[0]))
#endif


static DWORD WINAPI key_ISR(LPVOID lpParameter);


queue_t circle_queue;


typedef struct {
    int n1;
    int n2;
} userData_t;
userData_t Array[10] = {0x00};
userData_t writeData[10];


void print_array(void)
{
    int idx = 0;
    printf("[%d,%d] ", queue_item_len(&circle_queue), queue_free_len(&circle_queue));
    if (queue_is_empty(&circle_queue)) {
        printf("queue_is_empty!!!\n");
    } else if (queue_is_full(&circle_queue)) {
        printf("queue_is_full!!!\n");
    } else {
        for (idx = 0; idx < ARRAY_SIZE(Array); ++idx) {
            printf("(%2d,%2d), ", Array[idx].n1, Array[idx].n2);
        }
        printf("\n");
    }
}



int cnt = 0;
/*----------------------------------------------*/
void key_1_down(void)
{
    writeData[cnt].n1 = rand() % 100;
    writeData[cnt].n2 = rand() % 100;
    printf("writeData[%d] = (%2d,%2d)\n", cnt, writeData[cnt].n1, writeData[cnt].n2);

    ++cnt;
}
void key_1_up(void)
{

}
/*----------------------------------------------*/
void key_2_down(void)
{
    int num = 0;

    if (cnt > 0) {
        num = queue_write(&circle_queue, writeData, cnt);
        if (num > 0) {
            printf("queue_write(%d) \n", num);
        } else {
            printf("queue_write: full! \n");
        }
        cnt = 0;
    }

    print_array();
}
void key_2_up(void)
{

}
/*----------------------------------------------*/
void key_3_down(void)
{
    int num = 0;

    if (cnt > 0) {
        num = queue_push(&circle_queue, writeData, cnt);
        if (num > 0) {
            printf("queue_push(%d) \n", num);
        } else {
            printf("queue_push: full! \n");
        }
        cnt = 0;
    }

    print_array();
}
void key_3_up(void)
{

}
/*----------------------------------------------*/
void key_4_down(void)
{
    int num = 0;
    userData_t temp[10];

    num = queue_read(&circle_queue, temp, 1);
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
void key_4_up(void)
{

}
/*----------------------------------------------*/


int main(int argc, char *argv[])
{
    HANDLE keyThread = CreateThread(0, 0, key_ISR, 0, 0, 0);
    if (keyThread == NULL) {exit(-1);}


    printf("KEY[1] : get_data() \n");
    printf("KEY[2] : queue_write() \n");
    printf("KEY[3] : queue_push() \n");
    printf("KEY[4] : queue_read() \n\n");

    queue_init(&circle_queue, Array, ARRAY_SIZE(Array), sizeof(userData_t));
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

static struct key_isr_t key_isr[] =
{
    {'1', key_1_down, key_1_up},
    {'2', key_2_down, key_2_up},
    {'3', key_3_down, key_3_up},
    {'4', key_4_down, key_4_up},
};

static DWORD WINAPI key_ISR(LPVOID lpParameter)
{
    int idx = 0;
    while (1) {
        for (idx = 0; idx < ARRAY_SIZE(key_isr); ++idx) {
            if (GetKeyState(key_isr[idx].key) < 0) {
                if (key_isr[idx].flag == 0) { key_isr[idx].flag = 1; key_isr[idx].pDn(); }
            } else {
                if (key_isr[idx].flag == 1) { key_isr[idx].flag = 0; key_isr[idx].pUp(); }
            }
        }
    }
    return 0;
}

