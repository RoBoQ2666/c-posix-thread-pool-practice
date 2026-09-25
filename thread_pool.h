#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_
#endif

#include <pthread.h>
#include <stdbool.h>

//任务结构体
typedef struct task
{
    void (*task_func)(void* arg);
    void* arg;
    struct task* next_task;
}task;

//任务队列
typedef struct task_queue
{
    task* front;
    task* rear;
}task_queue;

//线程池结构体
typedef struct thread_pool
{
    //线程池状态
    bool thread_pool_status;

    //任务队列
    task_queue* task_q;

    //管理者线程ID
    pthread_t manager_id;
    //工作线程ID
    pthread_t* thread_ids;

    //当前线程数量
    int thread_count;

    //线程最小数量
    int min_num;

    //线程最大数量
    int max_num;

    //正在工作的线程数量
    int busy_num;

    //目前存活的线程数量
    int live_num;

    //需要杀死的线程数量
    int exit_num;

    //互斥锁
    pthread_mutex_t* mutex_thread_pool;  // 线程池锁

    //条件变量
    pthread_cond_t* cond_task_available;  //任务存在，队列不空
    
}thread_pool;


// 创建线程池
thread_pool* thread_pool_create(int thread_count);

// 工作线程函数
void *worker(void *arg);

// 管理者线程函数
void *manager(void* arg);

// 销毁线程池
void thread_pool_destroy(thread_pool** target);

// 判断任务队列是否为空
inline bool task_queue_isempty(const task_queue* tq) { return (tq->front == NULL && tq->rear == NULL); };

// 添加任务
void add_task(thread_pool* thp, void(*task_routine)(void* arg), void* arg);