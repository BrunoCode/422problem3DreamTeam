#include <stdio.h>
#include <time.h>
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
    if (pcb == NULL) {
        return NULL_OBJECT;
    }
    pcb->pid = DEFAULT_PID;
    pcb->state = (State) DEFAULT_STATE;
    pcb->priority = DEFAULT_PRIORITY;
    pcb->pc = DEFAULT_PC;
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
