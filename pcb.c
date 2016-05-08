/*
 *  TCSS 422 Spring 2016
 *  Team 4 Luis Solis-Bruno, Mat Sharff, Tempest Parr, Sara Vandandaigue
 *  Problem 3 Discontinuities
 */

#include <stdlib.h>
#include "pcb.h"

const char * STATE_NAMES[] = {"new", "ready", "running", "interrupted", "waiting", "halted"};

PCB_p PCB_construct (void) {
    return malloc(sizeof(PCB));
}

void PCB_destruct(PCB_p pcb) {
    if(pcb != NULL) free(pcb);
}

int PCB_init(PCB_p pcb) {
    int i = 0;
    long unsigned int current;
    srand(time(NULL));

    if (pcb == NULL) {
        return NULL_OBJECT;
    }
    pcb->pid = DEFAULT_PID;
    pcb->state = DEFAULT_STATE;
    pcb->priority = DEFAULT_PRIORITY;
    pcb->pc = DEFAULT_PC;
    pcb->sw = DEFAULT_SW;
    pcb->max_pc = rand() % 6000 + 2000;
    pcb->creation = time(NULL);
    pcb->termination = (long) DEFAULT_TERMINATION;
    pcb->terminate = rand() % 16;
    pcb->term_count = DEFAULT_TERM_COUNT;

    current = rand() % (pcb->max_pc - 7);
    pcb->IO_1_trap[0] = current;
    for(i = 1; i < 8; i++ ){
        if (i % 2 == 0) {
            pcb->IO_1_trap[i % 2] = current + (rand() % (pcb->max_pc - current - 7 + i)); 
            current = pcb->IO_1_trap[i % 2];
        } else {
            pcb->IO_2_trap[i % 2] = current + (rand() % (pcb->max_pc - current - 7 + i));
            current = pcb->IO_2_trap[i % 2];
        }
    }
    return SUCCESS;
}

int PCB_set_pid(PCB_p pcb, unsigned long pid) {
    if (pcb == NULL) {
        return NULL_OBJECT;
    }
    pcb->pid = pid;
    return SUCCESS;
}

unsigned long PCB_get_pid(PCB_p pcb) {
    //error checking in controller
    return pcb->pid;
}

int PCB_set_state(PCB_p pcb, State state) {
    if (pcb == NULL) {
        return NULL_OBJECT;
    }
    pcb->state = state;
    return SUCCESS;
}

State PCB_get_state(PCB_p pcb) {
    return pcb->state;
}

int PCB_set_priority(PCB_p pcb, unsigned short priority) {
    if (pcb == NULL) {
        return NULL_OBJECT;
    }
    pcb->priority = priority;
    return SUCCESS;
}

unsigned short PCB_get_priority(PCB_p pcb) {
    return pcb->priority;
}


char * PCB_toString(PCB_p pcb, char * string) {
    sprintf(string, "PID: 0x%0lX, State: %s, Priority: 0x%0X, PC: 0x%04lX",
            pcb->pid, STATE_NAMES[pcb->state], pcb->priority, pcb->pc);
    return string;
}
