#include "pcb.h"
#include "fifo_queue.h"

unsigned int sys_stack = 0;

PCB_p current_process;

FIFOq_p ready_queue;

void dispatch(void) {
  current_process = FIFOq_dequeue(ready_queue);
  current_process->state = running;
  sys_stack = current_process->pc;
  return;
}

int main(void) {

  return 0;
}
