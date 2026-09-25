#include <thread_pool.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

//创建线程池
thread_pool *thread_pool_create(int thread_count)
{
    //创建线程池实例
    thread_pool* thp = (thread_pool*)malloc(sizeof(thread_pool));
    if (thp == NULL) { fprintf(stderr, "errno:%d\n", errno); perror("malloc"); exit(EXIT_FAILURE); }
    
    //创建线程id数组
    thp->thread_ids = (pthread_t*)malloc(sizeof(pthread_t) * thp->thread_count);
    if (thp->thread_ids == NULL) { fprintf(stderr, "errno:%d\n", errno); perror("malloc"); exit(EXIT_FAILURE); }

    //创建管理者线程id
    thp->manager_id = (pthread_t*)malloc(sizeof(pthread_t));
    if (thp->manager_id == NULL) { fprintf(stderr, "errno:%d\n", errno); perror("malloc"); exit(EXIT_FAILURE); }

    //线程池状态
    thp->thread_pool_status = false;

    //TODO: 各种num的赋值
    thp->min_num = thp->thread_count; thp->max_num = thp->thread_count; thp->busy_num = thp->thread_count; thp->live_num = thp->thread_count; thp->exit_num = 0;

    //互斥锁的初始化
    thp->mutex_thread_pool = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if (thp->mutex_thread_pool == NULL) { fprintf(stderr, "errno:%d\n", errno); perror("malloc"); exit(EXIT_FAILURE); }
    
    if (pthread_mutex_init(thp->mutex_thread_pool, NULL) == -1) { fprintf(stderr, "errno:%d\n", errno); perror("pthread_mutex_init"); exit(EXIT_FAILURE); };


    //条件变量初始化
    thp->cond_task_available = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
    if (thp->cond_task_available == NULL) { fprintf(stderr, "errno:%d\n", errno); perror("malloc"); exit(EXIT_FAILURE); }
    if(pthread_cond_init(thp->cond_task_available, NULL) == -1) { fprintf(stderr, "errno:%d\n", errno); perror("pthread_cond_init"); exit(EXIT_FAILURE); };

    //任务队列初始化
    thp->task_q->front = thp->task_q->rear = NULL;

    //创建线程
    //创建管理者线程
    pthread_create(&thp->manager_id, NULL, manager,NULL);

    //创建工作线程
    for (size_t i = 0; i < thp->thread_count; i++)
    {
        pthread_create(&thp->thread_ids[i], NULL, worker, thp);
    }
    

    return thp;
}

// 工作线程函数
void *worker(void *arg)
{
    thread_pool* thp = (thread_pool*)arg;
    while (1)
    {
        pthread_mutex_lock(thp->mutex_thread_pool);

        // 线程池启用并且任务队列为空，等待条件
        while (thp->thread_pool_status && task_queue_isempty(thp->task_q))
        {
            pthread_cond_wait(thp->cond_task_available, thp->mutex_thread_pool);
        }
        if (!thp->thread_pool_status)
        {
            pthread_mutex_unlock(thp->mutex_thread_pool);
            pthread_exit(NULL);
        }
        
        // 取任务
        task t;
        t.task_func = thp->task_q->front->task_func;
        t.arg = thp->task_q->front->arg;

        // 出队
        task* delete = thp->task_q->front;
        thp->task_q->front = thp->task_q->front->next_task;
        free(delete);
        
        pthread_mutex_unlock(thp->mutex_thread_pool);
        
        if (t.task_func == NULL) continue;
        t.task_func(t.arg);
    }
}

void *manager(void *arg)
{
    return NULL;
}
