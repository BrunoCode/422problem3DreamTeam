#include "priority_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "priority_queue.h"

#define LOOPS 10

int priorityq_test(void) {
	char str[2000];
	PRIORITYq_p t1 = PRIORITYq_construct();
	int i;
	int j;
	int pcb_cnt = 0;
	PCB_p tmp_pcb;
	int rand_dequeue;

	for(i = 0; i < LOOPS; i++){
		// Enqueue 10 PCBs
		for(j = 0; j < LOOPS; j++){
			tmp_pcb = PCB_construct();
			PCB_init(tmp_pcb);
			PCB_set_pid(tmp_pcb, pcb_cnt);
			PCB_set_state(tmp_pcb, ready);
			PCB_set_priority(tmp_pcb, rand() % PRIORITYRANGE);

			PRIORITYq_enqueue(t1,tmp_pcb);
            pcb_cnt++;
		}

		// Print state
		printf("State after %d enqueues:\n", LOOPS);
		PRIORITYq_toString(t1, str, 2000);
		printf("%s\n", str);

		// Dequeue and print a random number of PCBs.
		rand_dequeue = 4 + rand() % 3;
		printf("Dequeue %d pcbs:\n", rand_dequeue);
		for(j = 0; j < rand_dequeue; j++){
			tmp_pcb = PRIORITYq_dequeue(t1);
			PCB_toString(tmp_pcb, str);
			printf("%s\n", str);
			PCB_destruct(tmp_pcb);
		}

		// Print state
		puts("\nState After Dequeue:");
		PRIORITYq_toString(t1, str, 2000);
		printf("%s\n", str);
	}

	PRIORITYq_destruct(t1);
    puts("End Test");
	return 0;

}
