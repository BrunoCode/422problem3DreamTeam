#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "operating_system.h"

unsigned int sys_stack = 0;

PCB_p current_process;

FIFOq_p ready_queue;

FIFOq_p new_process_list;

void dispatch(void) {
  current_process = FIFOq_dequeue(ready_queue);
  current_process->state = running;
  sys_stack = current_process->pc;
  return;
}

int main(void) {
  return 0;
}
