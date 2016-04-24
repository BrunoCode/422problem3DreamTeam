#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "operating_system.h"

FILE *outfile;


//Printing Buffer
char pcbString[128];


PCB_p idl;

unsigned long sys_stack = 0;

int iteration = 0;

unsigned long cpu_pc = 0;

PCB_p current_process;

FIFOq_p ready_queue;

FIFOq_p new_process_list;

unsigned long PIDVALUE = 0;

void setup(){
  idl = PCB_construct();
  PCB_init(idl);
  PCB_set_pid(idl, 0xFFFFFFFF);
  srand(time(NULL));
  ready_queue = FIFOq_construct();
  FIFOq_init(ready_queue);
  new_process_list = FIFOq_construct();
  FIFOq_init(new_process_list);
}

PCB_p generate_random_pcb(void) {
  PCB_p pcb = PCB_construct();
  PCB_init(pcb);
  PCB_set_pid(pcb, PIDVALUE);
  PIDVALUE = PIDVALUE + 1;
  PCB_set_state(pcb, new);
  PCB_set_priority(pcb, rand() % 15);
  pcb->pc = 0;
  return pcb;
}

void create_processes(void){
  int i;
  int random_num = rand() % 6;
  for(i = 0; i < random_num; i++){
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
  // current_process->state = running; // done in dispatcher to comply with assignment printing guidelines
  if(iteration%4==0)printf("Now running: %s\n", PCB_toString(current_process, pcbString));
}

void interruptSR(void){
  current_process->pc+=rand() % 1001 + 3000;
  current_process->state= interrupted;
  sys_stack=current_process->pc;
  schedulePCBs(INTERRUPTED);
}

void dispatcher(void) {
  if (iteration % 4 == 0) {
    PCB_p newproc = FIFOq_dequeue(ready_queue);

    // printf("PCB: %s\n", PCB_toString(current_process, pcbString));
    fprintf(outfile, "Switching to: %s\n", PCB_toString(newproc, pcbString));

    // Context Switch
    PCB_p lastproc = current_process;
    PCB_set_state(lastproc, ready);
    FIFOq_enqueue(ready_queue, lastproc); //return to ready queue
    current_process = FIFOq_dequeue(ready_queue); // set current process to next process in ready queue
    PCB_set_state(current_process, running);

    fprintf(outfile, "Now running: %s\n", PCB_toString(current_process, pcbString));
    fprintf(outfile, "Returned to Ready Queue: %s\n", PCB_toString(lastproc, pcbString));

    fprintf(outfile, "%s\n", FIFOq_toString(ready_queue)); // Print the ready queue

  } else {
    PCB_p lastproc = current_process;
    PCB_set_state(lastproc, ready);
    FIFOq_enqueue(ready_queue, lastproc); //return to ready queue
    current_process = FIFOq_dequeue(ready_queue); // set current process to next process in ready queue
  }
}

void scheduler(enum interrupt_type inter_type) {

  // Transfer any new process to the ready list
  while(FIFOq_size(new_process_list) > 0) {
    PCB_p temp = FIFOq_dequeue(new_process_list);
    PCB_set_state(temp, ready);
    FIFOq_enqueue(ready_queue, temp);
  }

  //determine situation
  switch (inter_type) {
    case timer:

      dispatcher(); //call dispatcher
      break;
    default:
      printf("INVALID INTERRUPT TYPE");
      break;
  }
}

void pseudo_timer_isr(void) {
  if (iteration % 4 == 0) {
    fprintf(outfile, "PCB: %s\n", PCB_toString(current_process, pcbString));
  }
  current_process->state = interrupted; //Change state to interrupted
  current_process->pc = sys_stack; // save cpu state to pcb

  scheduler(timer); //timer interrupt
}

int main(void) {
  outfile = fopen("output.txt", "w");
  setup();
  current_process = generate_random_pcb(); // Set initial process
  PCB_set_state(current_process, running);
  do { //Main Loop

    create_processes();
    cpu_pc = current_process->pc;
    cpu_pc += rand() % 1001 + 3000; // Simulate running of process
    sys_stack = cpu_pc; // Pseudo-push PC to system sys_stack

    pseudo_timer_isr();
    cpu_pc = sys_stack;

  } while (FIFOq_size(ready_queue) < 40);
  fclose(outfile);
}

// int main(void) {
//   setup();
//   do{
//     create_processes();
//     schedulePCBs(NEW_PROCESSES);
//     dispatch();
//     interruptSR();
//     printf("%d\n", iteration++);
//   }while(FIFOq_size(ready_queue) < 40);//limiting factor to be determined.
//
//   return 0;
// }
