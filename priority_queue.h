#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "pcb.h"
#include "fifo_queue.h"

#define PRIORITYRANGE 16

typedef struct priority_queue {
    FIFOq_p fifo_queues[PRIORITYRANGE];
} PRIORITYq;
typedef PRIORITYq * PRIORITYq_p;

PRIORITYq_p PRIORITYq_construct(void);
void PRIORITYq_destruct(PRIORITYq_p priorityqueue);
void PRIORITYq_enqueue(PRIORITYq_p queue, PCB_p pcb);
PCB_p PRIORITYq_dequeue(PRIORITYq_p queue);
char * PRIORITYq_toString(PRIORITYq_p priorityqueue, char * string, int size);

#endif
