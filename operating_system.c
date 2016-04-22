#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "operating_system.h"

unsigned int sys_stack = 0;

char pcbString[128];

int iteration = 0;

PCB_p current_process;

FIFOq_p ready_queue;

FIFOq_p new_process_list;

unsigned long PIDVALUE = 0;

void setup(){
  srand(time(NULL));
  ready_queue = FIFOq_construct();
  FIFOq_init(ready_queue);
  new_process_list = FIFOq_construct();
  FIFOq_init(new_process_list);
}

PCB_p generate_random_pcb() {
  PCB_p pcb = PCB_construct();
  PCB_init(pcb);
  PCB_set_pid(pcb, PIDVALUE);
  PIDVALUE = PIDVALUE + 1;
  PCB_set_state(pcb, new);
  PCB_set_priority(pcb, rand() % 15);
  pcb->pc = rand() % 5000;
  return pcb;
}

void create_processes(void){
  int i;
  for(i = 0; i < (rand() % 6); i++){
    PCB_p temp = generate_random_pcb();
    //set values of PCB as needed.
    FIFOq_enqueue(new_process_list, temp);
  }

  if (iteration == 0 && FIFOq_is_empty(new_process_list)) {
    PCB_p temp = generate_random_pcb();
    //set values of PCB as needed.
    FIFOq_enqueue(new_process_list, temp);
  }
}

void schedulePCBs(int flag){

  if(flag == INTERRUPTED){
    current_process->state = ready;
    if(iteration%4==0)printf("Returned to Ready Queue: %s\n", PCB_toString(current_process, pcbString));
    FIFOq_enqueue(ready_queue, current_process);
  } else {
    while(!FIFOq_is_empty(new_process_list)){
      PCB_p temp = FIFOq_dequeue(new_process_list);
      temp->state = ready;
      FIFOq_enqueue(ready_queue, temp);

      printf("New process: %s\n", PCB_toString(temp, pcbString));
    }
  }
}

void dispatch(void) {
  current_process = FIFOq_dequeue(ready_queue);
  if(iteration%4==0)printf("Switching to: %s\n", PCB_toString(current_process, pcbString));
  current_process->state = running;
  if(iteration%4==0)printf("Now running: %s\n", PCB_toString(current_process, pcbString));
}

void interruptSR(void){
  current_process->pc+=rand() % 1001 + 3000;
  current_process->state= interrupted;
  sys_stack=current_process->pc;
  schedulePCBs(INTERRUPTED);
}



int main(void) {
  setup();
  do{
    create_processes();
    schedulePCBs(NEW_PROCESSES);
    dispatch();
    interruptSR();
    printf("%d\n", iteration++);
  }while(FIFOq_size(ready_queue) < 3000);//limiting factor to be determined.

  return 0;
}
