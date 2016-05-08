#include "pcb.c"
#include "fifo_queue.c"

enum interrupt_type {
  IO_1, IO_2, TIMER, NEW_PROC_NEEDED, INVALID_INT_TYPE
};

#define INTERRUPTED 1
#define NEW_PROCESSES 2

void createProcesses(void);
void schedulePCBs(int);
void dispatch(void);
void interruptSR(void);
void setup(void);
