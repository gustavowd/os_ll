#include "derivative.h" /* include peripheral declarations */

typedef unsigned char cpu_t;

extern cpu_t *stk_tmp;

void init_os_timer(void);
cpu_t *PrepareStack(void *task, cpu_t *stk, int stk_size);

#define dispatcher()          \
        asm("LDHX stk_tmp");  \
        asm("TXS");           \
        asm("PULH");          \
        asm("RTI")
        
#define SAVE_CONTEXT()

#define RESTORE_CONTEXT()
        
        
#define SAVE_SP()             \
        asm("TSX");           \
        asm("STHX stk_tmp")
        
#define RESTORE_SP()          \
        asm("LDHX stk_tmp");  \
        asm("TXS")
        
#define yield() asm("SWI")

        