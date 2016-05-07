#include "pcb.h"
#include "fifo_queue.h"

enum interrupt_type {
  io, timer,
};

#define INTERRUPTED 1
#define NEW_PROCESSES 2

void createProcesses(void);
void schedulePCBs(int);
void dispatch(void);
void interruptSR(void);
void setup(void);
