#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "operating_system.h"

unsigned int sys_stack = 0;

PCB_p current_process;

FIFOq_p ready_queue;

FIFOq_p new_process_list;

void setup(){
  srand(time(NULL));
  ready_queue = FIFOq_construct();
  FIFOq_init(ready_queue);
  new_process_list = FIFOq_construct();
  FIFOq_init(new_process_list);
}

void create_processes(void){
  int i;
  for(i = 0; i < (rand() % 6); i++){
    PCB_p temp = PCB_construct();
    PCB_init(temp);
    //set values of PCB as needed.
    FIFOq_enqueue(new_process_list, temp);
  }
}

void schedulePCBs(int flag){
  if(flag == INTERRUPTED){
    current_process->state = interrupted;
    FIFOq_enqueue(ready_queue, current_process);
    current_process = NULL;
  } else {
    while(!FIFOq_is_empty(new_process_list)){
      PCB_p temp = FIFOq_dequeue(new_process_list);
      temp->state = running;
      FIFOq_enqueue(ready_queue, temp);
    }
  }
}

void dispatch(void) {
  current_process = FIFOq_dequeue(ready_queue);
  current_process->state = running;
  sys_stack = current_process->pc;
  return;
}

void interruptSR(void){
  current_process->pc+=rand() % 1001 + 3000;
  current_process->state= interrupted;
  schedulePCBs(INTERRUPTED);
}



int main(void) {
  setup();
  do{
    create_processes();
    schedulePCBs(NEW_PROCESSES);
    dispatch();
    interruptSR();
  }while(0);//limiting factor to be determined.

  return 0;
}
