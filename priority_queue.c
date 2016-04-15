#include <stdlib.h>
#include <stdio.h>
#include "priority_queue.h"

PRIORITYq_p PRIORITYq_construct(void) {
    PRIORITYq_p newRQueue = malloc(sizeof(PRIORITYq));
    for (int i = 0; i < PRIORITYRANGE; i++) {
        newRQueue->fifo_queues[i] = FIFOq_construct();
        FIFOq_init(newRQueue->fifo_queues[i]);
    }
    return newRQueue;
}

void PRIORITYq_destruct(PRIORITYq_p priorityqueue) {
    for (int i = 0; i < PRIORITYRANGE; i++) {
        FIFOq_destruct(priorityqueue->fifo_queues[i]);
    }
    free(priorityqueue);
}

void PRIORITYq_enqueue(PRIORITYq_p priorityqueue, PCB_p pcb) {
    FIFOq_enqueue(priorityqueue->fifo_queues[pcb->priority], pcb);
}

PCB_p PRIORITYq_dequeue(PRIORITYq_p priorityqueue) {
    for (int priority = 0; priority < 16; priority++) {
        if (FIFOq_is_empty(priorityqueue->fifo_queues[priority]) == 0) {
            return FIFOq_dequeue(priorityqueue->fifo_queues[priority]);
        }
    }
    return NULL;
}

char * PRIORITYq_toString(PRIORITYq_p priorityqueue, char * string, int size) {
    int offset = 0;
    int consumption = 0;

    for (int priority = 0; priority < PRIORITYRANGE; priority++) {
        if (FIFOq_is_empty(priorityqueue->fifo_queues[priority]) != 1) {
            int string_size = 32 + (4 * FIFOq_size(priorityqueue->fifo_queues[priority])) + 1;    // 11 for header, 4 for each node, 1 for \0
            char* fq_string = (char*) malloc(string_size);
            FIFOq_toString(priorityqueue->fifo_queues[priority], fq_string, string_size);

            consumption = snprintf(string + offset, size, "Q%d: %s\n", priority, fq_string);
            offset+= consumption;
            size-= consumption;
        }
    }

    return string;
}
