#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "os.h"
#include "tasks.h"

#ifdef __cplusplus
 extern "C"
#endif
void MCU_init(void); /* Device initialization function declaration */
                                

cpu_t stk1[128];
cpu_t stk2[128];
cpu_t stk3[128];
cpu_t stk4[128];

#if (use_linked_list == 1)
TCB_t tcb1, tcb2, tcb3, tcb4;
#endif


void main(void) {
  MCU_init(); /* call Device Initialization */   

  /* include your code here */
#if (use_linked_list == 1)
  InstallTask(&tcb1, task1, 2, stk1, sizeof(stk1));
  InstallTask(&tcb2, task2, 3, stk2, sizeof(stk2));
  InstallTask(&tcb3, task3, 4, stk3, sizeof(stk3));
  InstallTask(&tcb4, task4, 1, stk4, sizeof(stk4));
#else  
  InstallTask(task1, 2, stk1, sizeof(stk1));
  InstallTask(task2, 3, stk2, sizeof(stk2));
  InstallTask(task3, 4, stk3, sizeof(stk3));
  InstallTask(task4, 1, stk4, sizeof(stk4));
#endif    
  
#if with_scheduler == 1
  start_os();
#else
  stk_tmp = TCB[0].stk;
  dispatcher();
#endif

  for(;;) {
    /* __RESET_WATCHDOG(); by default, COP is disabled with device init. When enabling, also reset the watchdog. */
  } /* loop forever */
  /* please make sure that you never leave main */
}
