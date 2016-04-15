#include "pcb.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Checks if construction is successful.
void testConstructor(){
	PCB_p t1 = PCB_construct();
    if(t1){
		puts("Constructor passed");
		PCB_destruct(t1);
    } else {
        puts("Constructor failed");
    }
}

// Checks if initialized correctly.
void testInitializer() {
	PCB_p t1 = PCB_construct();
	PCB_init(t1);
    if(t1->pid == DEFAULT_PID
        && t1->state == DEFAULT_STATE
        && t1->priority == DEFAULT_PRIORITY
        && t1->pc == DEFAULT_PC
      ) {
        puts("Initializer passed");
    } else {
		puts("Initializer failed");
	}
	PCB_destruct(t1);
}

// Checks if setting pid works.
void testSetPid() {
	PCB_p t1 = PCB_construct();
	PCB_init(t1);
	PCB_set_pid(t1, 123456789UL);
    if(t1->pid == 123456789UL){
        puts("Setting pid passed");
    } else {
        puts("setting pid failed");
    }
	PCB_destruct(t1);
}

// Test if the get_pid retrieves correct pid.
void testGetPid() {
	PCB_p t1 = PCB_construct();
	PCB_init(t1);
	PCB_set_pid(t1, 123456789UL);
	unsigned long temp_pid = PCB_get_pid(t1);
	if(temp_pid == 123456789UL){
		puts("Get pid passed");
	} else {
		puts("Get pid failed");
	}
	PCB_destruct(t1);
}

// Checks if toString correctly represents the pcb state.
void testToString(char * theStr){
	PCB_p t1 = PCB_construct();
	PCB_init(t1);
	PCB_set_pid(t1, 123456789UL);
	PCB_toString(t1, theStr);
	//printf("%s\n",theStr);
	if (strcmp(theStr, "PID: 75BCD15, State: 0, Priority: 0, PC: 0")) {
		puts("toString failed");
	} else {

		puts("toString passed");
	}
	PCB_destruct(t1);
}

// Driver for testing all methods of pcb.
int pcb_test(void){
  char str[100];

	testConstructor();
  testInitializer();
	testSetPid();
	testGetPid();

	//test from specs screen shot this output
	puts("\n\n\n\n\n\n\n\n\n\n\n");
	puts("PCB TEST");
	PCB_p t1 = PCB_construct();
	PCB_init(t1);
	PCB_toString(t1, str);
	printf("%s\n",str);
	PCB_set_state(t1, ready);
	PCB_set_pid(t1, 1234);
	t1->pc = 12345;
	PCB_set_priority(t1, rand()%32);
	PCB_toString(t1, str);
	printf("%s\n",str);
	puts("-----------------------------------------");
	PCB_destruct(t1);
}
