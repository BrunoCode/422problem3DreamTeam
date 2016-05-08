/*
 *  TCSS 422 Spring 2016
 *  Team 4 Luis Solis-Bruno, Mat Sharff, Tempest Parr, Sara Vandandaigue
 *  Problem 3 Discontinuities
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "operating_system.h"

FILE *outfile;

char * fileHeader = "Team 4 Luis Solis-Bruno, Mat Sharff, Tempest Parr, Sara Vandandaigue\n";


//Printing Buffer
char pcbString[128];


// PCB_p idl;

unsigned long sys_stack = 0;

int iteration = 0;

unsigned long cpu_pc = 0;

PCB_p current_process;

FIFOq_p ready_queue;

FIFOq_p new_process_list;

FIFOq_p wait_queue_1;

FIFOq_p wait_queue_2;

FIFOq_p terminated_list;

unsigned long PIDVALUE = 0;

int IO_1_device_status = 0; // means OFF
int IO_1_downcounter = 0;
int IO_2_device_status = 0; // means OFF
int IO_2_downcounter = 0;


void setup(){
  // idl = PCB_construct();
  // PCB_init(idl);
  // PCB_set_pid(idl, 0xFFFFFFFF);
  srand(time(NULL));

  ready_queue = FIFOq_construct();
  FIFOq_init(ready_queue);
  new_process_list = FIFOq_construct();
  FIFOq_init(new_process_list);

  wait_queue_1 = FIFOq_construct();
  FIFOq_init(wait_queue_1);

  wait_queue_2 = FIFOq_construct();
  FIFOq_init(wait_queue_2);

  terminated_list = FIFOq_construct();
  FIFOq_init(terminated_list);
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
  pcb->creation = sys_stack;
  return pcb;
}

void create_processes(void){
  int i;
  static int call_counter = 0;
  int random_num = rand() % 6;
  PCB_p temp;

  if (call_counter >= 5) {
    return;
  } else {
    call_counter++;
  }

  fprintf(outfile, "creating %d processes\n", random_num);
  for(i = 0; i < random_num; i++) {
    temp = generate_random_pcb();
    fprintf(outfile, "process %d has io traps at %ld %ld %ld %ld and %ld %ld %ld %ld\n", i, temp->IO_1_trap[0], temp->IO_1_trap[1], temp->IO_1_trap[2], 
        temp->IO_1_trap[3], temp->IO_2_trap[0], temp->IO_2_trap[1], temp->IO_2_trap[2], temp->IO_2_trap[3]);
    temp->creation = sys_stack;
    //set values of PCB as needed.
    FIFOq_enqueue(new_process_list, temp);
  }

  if (iteration == 0 && FIFOq_is_empty(new_process_list)) {
    temp = generate_random_pcb();
    temp->creation = sys_stack;
    //set values of PCB as needed.
    FIFOq_enqueue(new_process_list, temp);
  }
}


void dispatcher(enum interrupt_type inter_type) {
  int verbose = 0;
  PCB_p newproc;
  PCB_p proc_to_enqueue;

  if (iteration % 4 == 0) {
    verbose = 1;
  }

  switch(inter_type) {
    case TIMER:
      newproc = FIFOq_dequeue(ready_queue);
      // To match problem specs print output, current process should print as running
      // However, the pseudo_timer_isr changes it's state to interrupted before it gets printed here
      // If we print PCB: ... etc in pseudo_timer_isr, the scheduler adding the new processes
      // will print and break up the continuity of the context switch prints
      if (verbose) {
        fprintf(outfile, "PCB: %s\n", PCB_toString(current_process, pcbString));
        fprintf(outfile, "Switching to: %s\n", PCB_toString(newproc, pcbString));
      }

      // Context Switch
      proc_to_enqueue = current_process;
      PCB_set_state(proc_to_enqueue, ready);
      FIFOq_enqueue(ready_queue, proc_to_enqueue); //return to ready queue
      // enqueue_ready(proc_to_enqueue);//return to ready queue

      current_process = newproc; // set current process to next process in ready queue
      PCB_set_state(current_process, running);
      break;
    case(IO_1):
      proc_to_enqueue = FIFOq_dequeue(wait_queue_1);
      PCB_set_state(proc_to_enqueue, ready);
      FIFOq_enqueue(ready_queue, proc_to_enqueue); //return to ready queue
      break;
    case(IO_2):
      proc_to_enqueue = FIFOq_dequeue(wait_queue_2);
      PCB_set_state(proc_to_enqueue, ready);
      FIFOq_enqueue(ready_queue, proc_to_enqueue); //return to ready queue
      break;
    case(NEW_PROC_NEEDED):
      newproc = FIFOq_dequeue(ready_queue);
      current_process = newproc; // set current process to next process in ready queue
      PCB_set_state(current_process, running);
      break;
    default:
      break;
  }

  if (verbose && inter_type < NEW_PROC_NEEDED) {
    fprintf(outfile, "Now running: %s\n", PCB_toString(current_process, pcbString));
    fprintf(outfile, "Returned to Ready Queue: %s\n", PCB_toString(proc_to_enqueue, pcbString));

    int string_size = 32 + (10 * FIFOq_size(ready_queue)) + 1;    // 32 for header, 4 for each node, 1 for \0
    char* rq_string = (char*) malloc((size_t) string_size);
    FIFOq_toString(ready_queue, rq_string, string_size);

    fprintf(outfile, "%s\n", rq_string); // Print the ready queue
    free(rq_string);
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

  dispatcher(inter_type);
}

void pseudo_timer_isr(void) {
  // if (iteration % 4 == 0) {
  //   fprintf(outfile, "PCB: %s\n", PCB_toString(current_process, pcbString));
  // }
  current_process->state = interrupted; //Change state to interrupted
  current_process->pc = cpu_pc; // save cpu state to pcb

  scheduler(TIMER); //timer interrupt
}

void pseudo_IO_1_isr(void) {
  scheduler(IO_1);
}

void pseudo_IO_2_isr(void) {
  scheduler(IO_2);
}

void init_IO_1_device() {
  IO_1_downcounter = 900 + (rand() % 600);
}

void init_IO_2_device() {
  IO_2_downcounter = 900 + (rand() % 600);
}

void io_trap_handler(int trap_service_routine_number) {
  // take running process out of state & put it into appropriate waiting queue
  current_process->state = waiting;
  // push process to waiting queue of trap_service_routine_number
  if(trap_service_routine_number == 1) {
    FIFOq_enqueue(wait_queue_1, current_process);
    // also init the io timer if it isn't initialized already
    if(FIFOq_is_empty(wait_queue_1)) {
      init_IO_1_device();
    }
  } else if (trap_service_routine_number == 2) {
    FIFOq_enqueue(wait_queue_2, current_process);
    if(FIFOq_is_empty(wait_queue_2)) {
      init_IO_2_device();
    }
  }

  // dispatch a new process from ready queue
  dispatcher(NEW_PROC_NEEDED);
}

int timer_device() {
  static int downcounter = 300;
  downcounter--;
  if(downcounter == 0) {
    downcounter = 300;
    return 1;
  }
  return 0;
}

int IO_1_device() {
  if (IO_1_device_status) {
    IO_1_downcounter--;
    if(IO_1_downcounter == 0) {
      if(FIFOq_size(wait_queue_1) == 1) {
        init_IO_1_device();
      }
      return 1;
    }
  }
  return 0;
}

int IO_2_device() {
  if(IO_2_device_status) {
    IO_2_downcounter--;
    if(IO_2_downcounter == 0) {
      if(FIFOq_size(wait_queue_2) == 1) {
        init_IO_2_device();
      }
      return 1;
    }
  }
  return 0;
}

int main(void) {
  outfile = fopen("scheduleTrace.txt", "w");
  fprintf(outfile, "%s\n", fileHeader);
  setup();
  current_process = generate_random_pcb(); // Set initial process
  PCB_set_state(current_process, running);
  int i;

  sys_stack = 0;

  do { // Main Loop
    create_processes();
    cpu_pc = current_process->pc;
    // cpu_pc += rand() % 1001 + 3000; // Simulate running of process
    cpu_pc++;           // increment running process' PC by one
    sys_stack++;        // represents total cycles ran by the CPU
    iteration++;
    // check timer here, if not hit then look for io traps
    // sys_stack = cpu_pc; // Pseudo-push PC to system sys_stack

    if (timer_device()) {
      fprintf(outfile, "Timer expired\n");
      pseudo_timer_isr();
    } else {
      // increment the current process's pc and check for termination
      current_process->pc = cpu_pc;

      if(cpu_pc == current_process->max_pc) {
        cpu_pc = 0;
        current_process->pc = 0;
        current_process->term_count++;
        if (current_process->term_count == current_process->terminate) {
          current_process->termination = sys_stack;
        }
        FIFOq_enqueue(terminated_list, current_process);
        dispatcher(NEW_PROC_NEEDED);
      } 

      // loop through PCB I/O arrays
      for (i = 0; i < 4; i++) {
        // compare PC value with each value in PCB I/O arrays
        // call I/O trap handler
        if (current_process->IO_1_trap[i] == cpu_pc) {
            fprintf(outfile, "IO1 trap\n");
            io_trap_handler(1);
            break;
        } else if (current_process->IO_2_trap[i] == cpu_pc) {
            fprintf(outfile, "IO2 trap\n");
            io_trap_handler(2);
            break;
        }
      }
    }

    if(IO_1_device()){
      fprintf(outfile, "IO1 device completion\n");
      pseudo_IO_1_isr();
    }

    if(IO_2_device()){
      fprintf(outfile, "IO2 device completion\n");
      pseudo_IO_2_isr();
    }

    // Need to put pcbs from new list to ready queue
    scheduler(INVALID_INT_TYPE);

  } while (FIFOq_size(ready_queue) > 0);

  PCB_destruct(current_process);
  FIFOq_destruct(new_process_list);
  FIFOq_destruct(ready_queue);
  fclose(outfile);
}
