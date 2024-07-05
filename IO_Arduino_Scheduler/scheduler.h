#include "tcb.h"
#include "tcb_list.h"

// the (detached) running process
extern TCB* current_tcb;

// the running queue
extern TCBList running_queue;
extern TCBList wait_read_queue;
extern TCBList wait_write_queue;

void startSchedule(void);

// used in the ISR of the timer
void schedule(void);

void read_wait(void);

void write_wait(void);

void read_wakeup(void);

void write_wakeup(void);
     
