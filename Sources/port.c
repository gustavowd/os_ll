#include "os.h"

cpu_t *stk_tmp;

cpu_t *PrepareStack(void *task, cpu_t *stk, int stk_size){
  stk = (cpu_t*)((int)stk + stk_size - 1);
  
  *stk-- = (cpu_t)((int)task & 0xFF);
  *stk-- = (cpu_t)((int)task >> 8);
  *stk-- = 0;
  *stk-- = 0;
  *stk-- = 0;
  *stk = 0;
  
  return stk;
}


interrupt void SwitchContext(void){

  SAVE_CONTEXT();
  SAVE_SP();

#if with_scheduler == 1
  current_task->stk=stk_tmp;
#else
  TCB[ct].stk=stk_tmp;
#endif
  
#if with_scheduler == 1
  stk_tmp = scheduler();  
#else
  ct++;
  if (ct >= it){
    ct = 0;
  }
  stk_tmp = TCB[ct].stk;
#endif
  
  RESTORE_SP();
  RESTORE_CONTEXT();
}



void init_os_timer(void){
  TPM1SC = 0x00;
  TPM1MOD = 19999;
  TPM1SC = 0x48;
}                                     


interrupt void TickTimer(void){
  TPM1SC_TOF = 0;
  
  if(os_inc_and_compare()){
    SAVE_CONTEXT();
    SAVE_SP();
    
    #if (use_linked_list == 1)
    current_task->stk=stk_tmp;
    #else 
    TCB[ct].stk=stk_tmp;
    #endif
    stk_tmp = scheduler();
  
    RESTORE_SP();  
    RESTORE_CONTEXT();
  }
  
}