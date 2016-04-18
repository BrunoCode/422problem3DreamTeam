#include "pcb.c"
#include "fifo_queue.c"

void genProcesses(FIFOq_p);
void schedulePCBs(FIFOq_p, FIFOq_p);
void dispatcher(FIFOq_p);
void interuptSR(FIFOq_p);
