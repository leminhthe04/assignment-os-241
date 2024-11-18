#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

#endif

#ifndef MLQ_SCHED
#define MLQ_SCHED
#endif


int queue_empty(void);

#ifdef MLQ_SCHED
void decrease_slot(int used_slot, unsigned long prio);
/* Get the next process from ready queue and 
   return the slot of queue containing the process to [slot_of_queue_of_proc] */
struct pcb_t * get_proc(int *slot_of_queue_of_proc);
#else
/* Get the next process from ready queue */
struct pcb_t * get_proc(void);
#endif


void init_scheduler(void);
void finish_scheduler(void);


/* Put a process back to run queue */
void put_proc(struct pcb_t * proc);

/* Add a new process to ready queue */
void add_proc(struct pcb_t * proc);




