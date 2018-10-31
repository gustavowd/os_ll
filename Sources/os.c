#include "os.h"
#include <stdlib.h>

#if (use_linked_list == 1)
TCB_t tcb_idle;
#else
volatile TCB_t TCB[4];
#endif
cpu_t idle_stk[128];

volatile cpu_t ct = 0;
volatile cpu_t it = 0;
volatile long long os_time = 0;

list_t readylist = {null, null};
list_t timelist = {null, null};

TCB_t *current_task;


#define sched sched_prio

void idle(void) {
  for(;;){
    // dorme aqui
  }
}

#if (use_linked_list == 0)
cpu_t *scheduler(void){
  cpu_t i = 0;
  cpu_t higher_prio = 0;
  cpu_t higher_task = 0;
  
  for(i=0;i<it;i++){
    if(TCB[i].ready == 1){
      if (TCB[i].prio >= higher_prio){
        higher_prio = TCB[i].prio;
        higher_task = i;
      }
    }
  }
  ct = higher_task;
  return TCB[higher_task].stk;
}
#endif


#if (use_linked_list == 1)
cpu_t *scheduler(void){
#if (sched == sched_prio)
  TCB_t *ptask = readylist.head;
  cpu_t higher_prio = 0;
  TCB_t *higher_task = 0;
  
  while(ptask != null)
  { 
    if (ptask->prio >= higher_prio){
      higher_prio = ptask->prio;
      higher_task = ptask;
    }
    ptask = ptask->next;
  }
  current_task = higher_task;
#else
  if (current_task->next == null){
    current_task = readylist.head;
  }
  else{
    current_task = current_task->next;
  }
#endif
  return current_task->stk;
}
#endif
 
   

void start_os(void){
#if (use_linked_list == 1)
  InstallTask(&tcb_idle, idle, 0, idle_stk, sizeof(idle_stk));
#else
  InstallTask(idle, 0, idle_stk, sizeof(idle_stk));
#endif
  init_os_timer();
  
  #if sched == sched_prio
  stk_tmp = scheduler();
  #else
  current_task = readylist.head;
  stk_tmp = current_task->stk;
  #endif
  
  dispatcher();
}


void delay(long long timeout){
  #if (use_linked_list == 1)
  TCB_t *ptask = current_task;
  ptask->timeout = timeout + os_time;
  RemoveFromList(ptask, &readylist);
  IncludeTaskIntoList(ptask, &timelist);
  #else
  TCB[ct].timeout = timeout + os_time;
  TCB[ct].ready = 0;
  #endif
  yield();
}


cpu_t os_inc_and_compare(void){
#if use_linked_list == 1)
  TCB_t *ptask = timelist.head;
  TCB_t *tmp_task;
  cpu_t ready = 0; 

  os_time++;
  while(ptask != null){ 
    if (ptask->timeout == os_time){
      tmp_task = ptask->next;
      RemoveFromList(ptask, &timelist);
      IncludeTaskIntoList(ptask, &readylist);
      if (ptask->event != null){
        remove_event(ptask);
      }
      ready = 1;
      ptask = tmp_task;
    }else{ 
      ptask = ptask->next;
    }
  }  
  
  return ready;
#else
  int i;
  cpu_t ready = 0;
  os_time++;
  
  for(i=0;i<it;i++){
    if(TCB[i].timeout == os_time){
      TCB[i].ready = 1;
      ready = 1;
    }
  }
  
  return ready;
#endif
}


#if (use_linked_list == 1)
void InstallTask(TCB_t *ptask, task_t task, cpu_t prio, cpu_t *stk, int stk_size){
  ptask->stk = PrepareStack(task, stk, stk_size);
  ptask->prio = prio;
  ptask->event = null;
  ptask->event_type = 0;
  IncludeTaskIntoList(ptask, &readylist);
  it++;
#else
void InstallTask(task_t task, cpu_t prio, cpu_t *stk, int stk_size)  
  TCB[it].stk = PrepareStack(task, stk, stk_size);
  TCB[it].prio = prio;
  TCB[it].ready = 1;
  it++;
#endif
}


cpu_t sem_init(sem_t *sem){
  sem->value = 0;
  sem->list.head = null;
  sem->list.tail = null;
  return 0;
}

cpu_t sem_pend(sem_t *sem, long long timeout){
  TCB_t *ptask = current_task;
  if (sem->value){
    sem->value--;
    return 0;
  }

  ptask->timeout = timeout + os_time;
  RemoveFromList(ptask, &readylist);
  IncludeTaskIntoList(ptask, &(sem->list));
  if (timeout){
    IncludeTaskIntoList(ptask, &timelist);
    ptask->event = (void *)sem;
    ptask->event_type = event_sem;
  }
  yield();
  
  return 0;
}

cpu_t sem_post(sem_t *sem){
  TCB_t *ptask = sem->list.head;
  if (sem->list.tail == null){
    if (!(sem->value)){
      sem->value++;
    }
    return 0;
  }
  
  RemoveFromList(ptask, &(sem->list));
  if (ptask->event != null){
    ptask->event = null;
    ptask->event_type = 0;
    RemoveFromList(ptask, &timelist);
  }
  IncludeTaskIntoList(ptask, &readylist);
  
  yield();
  
  return 0;
}

void remove_event(TCB_t *ptask){
  sem_t *sem_tmp;
  switch(ptask->event_type){
    case event_sem:
      sem_tmp = (sem_t *)ptask->event;
      RemoveFromList(ptask, &(sem_tmp->list));
      ptask->event = null;
      ptask->event_type = 0;
    break;
    
    case event_queue:
    break;
  }
}


#if (use_linked_list == 1)
void RemoveFromList(TCB_t *ptask, list_t *list){  
    if(ptask == list->head){               
      if(ptask == list->tail){                
        list->tail = NULL;   
        list->head = NULL;   
      }                
      else{                
        list->head = ptask->next;
        list->head->previous = NULL;
      }                       
    }                         
    else{                         
      if(ptask == list->tail){                       
        list->tail = ptask->previous;
        list->tail->next = NULL;    
      }                       
      else{                       
        ptask->next->previous = ptask->previous;
        ptask->previous->next = ptask->next;   
      }                                       
    }
}


void IncludeTaskIntoList(TCB_t *ptask, list_t *list){
    if(list->tail != NULL){
      /* Insert task into list */
      list->tail->next = ptask;
      ptask->previous = list->tail;
      list->tail = ptask;
      list->tail->next = NULL;
    }
    else{
       /* Init delay list */
       list->tail = ptask;
       list->head = ptask;
       ptask->next = NULL;
       ptask->previous = NULL;
    }
}
#endif