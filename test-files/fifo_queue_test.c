#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "fifo_queue.h"

unsigned short generate_priority();

void testConstructor(void) {
    FIFOq_p fq = FIFOq_construct();
    if (fq != NULL) {
        puts("FIFOq_construct passed");
    } else {
        puts("FIFOq_construct failed");
    }
    FIFOq_destruct(fq);
}

void testInitializer() {
    FIFOq_p fq = FIFOq_construct();
    FIFOq_init(fq);
    if (fq->size == 0 && fq->front == NULL && fq->rear == NULL) {
        puts("FIFOq_init passed");
    } else {
        puts("FIFOq_init failed");
    }
    FIFOq_destruct(fq);
}

void testisEmpty() {
    FIFOq_p fq = FIFOq_construct();
    FIFOq_init(fq);
    if (FIFOq_is_empty(fq)) {
        puts("FIFOq_is_empty passed");
    } else {
        puts("FIFOq_is_empty failed");
    }
    FIFOq_destruct(fq);
}

void testEnqueue(PCB_p testPCB) {
    char pcbteststring[100];
    char pcbqstring[100];
    FIFOq_p fq = FIFOq_construct();
    FIFOq_init(fq);
    if (FIFOq_enqueue(fq, testPCB) == 1) {
        puts("enqueued");
    }

    if (FIFOq_is_empty(fq) == 0) {
        int stringtest = strcmp(PCB_toString(testPCB, pcbteststring), PCB_toString(FIFOq_dequeue(fq), pcbqstring));
        if (stringtest == 0) {
            puts("FIFOq_enqueue passed");
        } else {
            puts("FIFOq_enqueue failed");
        }
    } else {
        puts("FIFOq_enqueue failed");
    }
    FIFOq_destruct(fq);
}

void testDequeue(PCB_p testPCB) {
    char pcbteststring[100];
    char pcbqstring[100];
    PCB_toString(testPCB, pcbteststring);
    FIFOq_p fq = FIFOq_construct();
    FIFOq_init(fq);
    FIFOq_enqueue(fq, testPCB);

    PCB_p dqPCB = FIFOq_dequeue(fq);
    PCB_toString(dqPCB, pcbqstring);

    if (strcmp(pcbqstring, pcbteststring) == 0) {
        puts("FIFOq_dequeue passed");
    } else {
        puts("FIFOq_dequeue failed");
    }
}

void stressTest() {
    FIFOq_p fq = FIFOq_construct();
    FIFOq_init(fq);

    // Generate a test size between 10 and 30.
    int test_size = (rand() % 21) + 10;
    printf("Running a stress test of size %d:\n", test_size);

    // Enqueue test_size amount of randomly generated PCBs.
    for(int i = 0; i < test_size; i++) {
        PCB_p pcb = PCB_construct();
        PCB_init(pcb);
        PCB_set_pid(pcb, (unsigned long) i);
        PCB_set_priority(pcb, generate_priority());
        FIFOq_enqueue(fq, pcb);

        int string_size = 32 + (4 * FIFOq_size(fq)) + 1;    // 32 for header, 4 for each node, 1 for \0
        char* fq_string = (char*) malloc((size_t) string_size);
        FIFOq_toString(fq, fq_string, string_size);

        char pcb_string[100];
        PCB_toString(pcb, pcb_string);

        printf("Q:%s : contents: %s\n", fq_string, pcb_string);

        free(fq_string);
    }
    puts("Enqueue FIFOq complete.");

    puts("Dequeue FIFOq started.");
    // Dequeue test_size amount of randomly generated PCBs.
    int checkPID = -1;   // PCBs should be dequeued in ascending order (they were enqueued as such).
    for(int i = 0; i < test_size; i++) {
        int string_size = 32 + (4 * FIFOq_size(fq)) + 1;    // 32 for header, 4 for each node, 1 for \0
        char* fq_string = (char*) malloc(string_size);
        FIFOq_toString(fq, fq_string, string_size);

        PCB_p pcb = FIFOq_dequeue(fq);
        checkPID++;
        char pcb_string[100];
        PCB_toString(pcb, pcb_string);
        // Set checkPID to -1 if the incorrect PCB was dequeued.
        if(checkPID != PCB_get_pid(pcb)) {
            checkPID = -1;
        }

        printf("Q:%s : contents: %s\n", fq_string, pcb_string);

        free(fq_string);
        PCB_destruct(pcb);
    }

    // Determine whether the amount of PCBs dequeued are the amount of PCBs enqueued.
    // Also check whether PCBs were dequeued in correct order.
    if(FIFOq_is_empty(fq) && checkPID == test_size - 1) {
        puts("FIFOq stress test passed.");
    } else {
        printf("FIFOq stress test failed with checkPID = %d.\n", checkPID);
    }
}

unsigned short generate_priority() {
    return (unsigned short) (rand() % 32);
}

int fifoq_test(void) {
    PCB_p testPCB = PCB_construct();
    PCB_init(testPCB);
    PCB_set_pid(testPCB, 15UL);
    PCB_set_priority(testPCB, 2);

    testConstructor();
    testInitializer();
    testisEmpty();
    testEnqueue(testPCB);
    testDequeue(testPCB);

    srand((unsigned int) time(NULL));
    stressTest();

    PCB_destruct(testPCB);
    puts("End FIFO_queue Test");
}
