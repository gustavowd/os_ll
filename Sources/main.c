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

TCB_t tcb1, tcb2, tcb3, tcb4;


void main(void) {
  MCU_init(); /* call Device Initialization */   

  /* include your code here */
  InstallTask(&tcb1, task1, 2, stk1, sizeof(stk1));
  InstallTask(&tcb2, task2, 3, stk2, sizeof(stk2));
  InstallTask(&tcb3, task3, 4, stk3, sizeof(stk3));
  InstallTask(&tcb4, task4, 1, stk4, sizeof(stk4));
  
  start_os();

  for(;;) {
    /* __RESET_WATCHDOG(); by default, COP is disabled with device init. When enabling, also reset the watchdog. */
  } /* loop forever */
  /* please make sure that you never leave main */
}
