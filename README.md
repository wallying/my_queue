# my_queue
my queue library

https://github.com/wallying/my_queue.git

##lib_queue循环队列使用方法:
###1.定义队列变量(类型必须为queue_t)
    例如：queue_t circular_queue;
###2.定义数据结构体
    例如：struct data_t {...};
###3.定义数据结构体数组
    例如：struct data_t array[N];
###4.使用队列API函数(函数形参必须为队列地址)
    queue_init(&circular_queue, array, sizeof(struct data_t), ARRAY_SIZE(array));
    queue_write(&circular_queue, &temp, 3);
    queue_read(&circular_queue, &temp, 2);
