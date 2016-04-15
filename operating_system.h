

void genProcesses(FIFOq_p newProcessList);
void schedulePCBs(FIFOq_p newProcessList, FIFOq_p readyQueue);
void dispatcher(FIFOq_p readyQueue);
void interuptSR(FIFOq_p readyQueue);
