#include "pcb.h"
#include "fifo_queue.h"

void createProcesses(FIFOq_p);
void schedulePCBs(FIFOq_p, FIFOq_p);
void dispatcher(FIFOq_p);
void interruptSR(FIFOq_p);
