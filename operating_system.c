#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "operating_system.h"

FILE *outfile;

char * fileHeader = "Team 6: Derek Moore, Son Vu, Mat Sharff";


//Printing Buffer
char pcbString[128];


// PCB_p idl;

unsigned long sys_stack = 0;

int iteration = 0;

unsigned long cpu_pc = 0;

PCB_p current_process;

FIFOq_p ready_queue;

FIFOq_p new_process_list;

unsigned long PIDVALUE = 0;

void setup(){
  // idl = PCB_construct();
  // PCB_init(idl);
  // PCB_set_pid(idl, 0xFFFFFFFF);
  srand(time(NULL));
  ready_queue = FIFOq_construct();
  FIFOq_init(ready_queue);
  new_process_list = FIFOq_construct();
  FIFOq_init(new_process_list);
}

void enqueue_ready(PCB_p process) {
  FIFOq_enqueue(ready_queue, process);
  fprintf(outfile, "Process Enqueued: %s\n", PCB_toString(process, pcbString));
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
  PCB_p temp;
  for(i = 0; i < random_num; i++){
    temp = generate_random_pcb();
    //set values of PCB as needed.
    FIFOq_enqueue(new_process_list, temp);
  }

  if (iteration == 0 && FIFOq_is_empty(new_process_list)) {
    temp = generate_random_pcb();
    //set values of PCB as needed.
    FIFOq_enqueue(new_process_list, temp);
  }
}


void dispatcher(void) {
  if (iteration % 4 == 0) {
    PCB_p newproc = FIFOq_dequeue(ready_queue);
    // To match problem specs print output, current process should print as running
    // However, the pseudo_timer_isr changes it's state to interrupted before it gets printed here
    // If we print PCB: ... etc in pseudo_timer_isr, the scheduler adding the new processes
    // will print and break up the continuity of the context switch prints
    fprintf(outfile, "PCB: %s\n", PCB_toString(current_process, pcbString));
    fprintf(outfile, "Switching to: %s\n", PCB_toString(newproc, pcbString));

    // Context Switch
    PCB_p lastproc = current_process;
    PCB_set_state(lastproc, ready);
    FIFOq_enqueue(ready_queue, lastproc); //return to ready queue
    // enqueue_ready(lastproc);//return to ready queue

    current_process = newproc; // set current process to next process in ready queue
    PCB_set_state(current_process, running);

    fprintf(outfile, "Now running: %s\n", PCB_toString(current_process, pcbString));
    fprintf(outfile, "Returned to Ready Queue: %s\n", PCB_toString(lastproc, pcbString));

    int string_size = 32 + (10 * FIFOq_size(ready_queue)) + 1;    // 32 for header, 4 for each node, 1 for \0
    char* rq_string = (char*) malloc((size_t) string_size);
    FIFOq_toString(ready_queue, rq_string, string_size);

    fprintf(outfile, "%s\n", rq_string); // Print the ready queue
    free(rq_string);

  } else {
    PCB_p lastproc = current_process;
    PCB_set_state(lastproc, ready);
    // FIFOq_enqueue(ready_queue, lastproc); //return to ready queue
    enqueue_ready(lastproc);
    current_process = FIFOq_dequeue(ready_queue); // set current process to next process in ready queue
    // fprintf(outfile, "Process Enqueued: %s\n", PCB_toString(lastproc, pcbString));
  }
}

void scheduler(enum interrupt_type inter_type) {

  // Transfer any new process to the ready list
  while(FIFOq_size(new_process_list) > 0) {
    PCB_p newproc = FIFOq_dequeue(new_process_list);
    PCB_set_state(newproc, ready);
    // FIFOq_enqueue(ready_queue, newproc);
    enqueue_ready(newproc);
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
  // if (iteration % 4 == 0) {
  //   fprintf(outfile, "PCB: %s\n", PCB_toString(current_process, pcbString));
  // }
  current_process->state = interrupted; //Change state to interrupted
  current_process->pc = sys_stack; // save cpu state to pcb

  scheduler(timer); //timer interrupt
}

int main(void) {
  outfile = fopen("scheduleTrace.txt", "w");
  fprintf(outfile, "%s\n", fileHeader);
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

  PCB_destruct(current_process);
  FIFOq_destruct(new_process_list);
  FIFOq_destruct(ready_queue);
  fclose(outfile);
}
