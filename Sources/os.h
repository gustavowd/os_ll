#include "port.h"

#define sched_prio 0
#define sched_circ 1

#define with_scheduler 1
#define sched sched_prio

#define null (void*)0

typedef enum{
  event_sem=1,
  event_queue
}event_t;

typedef void (*task_t)(void); 

struct _TCB{
  cpu_t *stk;
  cpu_t prio;
  cpu_t ready;
  void *event;
  event_t event_type;
  long long timeout;
  struct _TCB *next;
  struct _TCB *previous;  
};

typedef struct _TCB TCB_t;

typedef struct _list_t{
  TCB_t *tail;
  TCB_t *head;
}list_t;


typedef struct _sem_t_{
  cpu_t value;
  list_t list;
}sem_t;

extern volatile TCB_t TCB[4];
extern volatile cpu_t ct;
extern volatile cpu_t it;
extern TCB_t *current_task;

void InstallTask(TCB_t *tcb, task_t task, cpu_t prio, cpu_t *stk, int stk_size);

void delay(long long timeout);
void start_os(void);
cpu_t os_inc_and_compare(void);
cpu_t *scheduler(void);

cpu_t sem_init(sem_t *sem);
cpu_t sem_pend(sem_t *sem, long long timeout);
cpu_t sem_post(sem_t *sem);

void remove_event(TCB_t *ptask);

void RemoveFromList(TCB_t *ptask, list_t *list);
void IncludeTaskIntoList(TCB_t *ptask, list_t *list);

