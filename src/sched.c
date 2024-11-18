
#include "queue.h"
#include "sched.h"
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
static struct queue_t ready_queue; // case only one queue (outdate)
static struct queue_t run_queue; // no need, legacy/outdated code, but still be kept for avoiding bug tracking
static pthread_mutex_t queue_lock;

#ifdef MLQ_SCHED
// each element is a queue, case using multi-level queue. Index of a queue in this array is the priority of the queue
static struct queue_t mlq_ready_queue[MAX_PRIO]; 
#endif

int queue_empty(void) {
#ifdef MLQ_SCHED
	// case multi-level queue, check if exist any queue is not empty
	for (unsigned long prio = 0; prio < MAX_PRIO; ++prio)
		if(!empty(&mlq_ready_queue[prio])) 
			return 0;
	return 1;
#endif
	// case only one queue (outdate), check if the queue is empty
	if (empty(&ready_queue) && empty(&run_queue))
		return 1;
	return 0;
}

void init_scheduler(void) {
#ifdef MLQ_SCHED
	for (unsigned long prio = 0; prio < MAX_PRIO; ++prio){
		mlq_ready_queue[prio].size = 0;
		// Addition: init slot for each queue (slot time slice mechanism)
		mlq_ready_queue[prio].slot = MAX_PRIO - prio;
	}
#endif
	ready_queue.size = 0;
	run_queue.size = 0;
	pthread_mutex_init(&queue_lock, NULL);
}

#ifdef MLQ_SCHED
/* 
 *  Stateful design for routine calling
 *  based on the priority and our MLQ policy
 *  We implement stateful here using transition technique
 *  State representation   prio = 0 .. MAX_PRIO, curr_slot = 0..(MAX_PRIO - prio)
 */
void check_no_more_slots(void){
	/* if all non-empty queues have no more slot, reset their slot value */

	for(unsigned long prio = 0; prio < MAX_PRIO; ++prio)
		if(!empty(&mlq_ready_queue[prio]) && mlq_ready_queue[prio].slot > 0){
			/* exist a non-empty queue that has slot => no need to update => return */
			return;
		}
	
	/* reset all slots in all queues */
	for(unsigned long prio = 0; prio < MAX_PRIO; ++prio){
		update_slot(&mlq_ready_queue[prio], MAX_PRIO - prio);
	}
}

struct pcb_t * get_mlq_proc(int* slot_of_queue_of_proc) {
	/*TODO: get a process from PRIORITY [ready_queue].
	 * Remember to use lock to protect the queue.
	 * */
	struct pcb_t * ret_proc = NULL;

	pthread_mutex_lock(&queue_lock);

	/* first reset their slot value if all non-empty queues have no more slot */
	check_no_more_slots();

	for (unsigned long prio = 0; prio < MAX_PRIO; ++prio) {
		if (!empty(&mlq_ready_queue[prio]) && 
						mlq_ready_queue[prio].slot > 0) {
			ret_proc = dequeue(&mlq_ready_queue[prio]);
			// return parameter
			*slot_of_queue_of_proc = mlq_ready_queue[prio].slot;
			break;
		}
	}

	pthread_mutex_unlock(&queue_lock);
	return ret_proc;
}


void put_mlq_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&mlq_ready_queue[proc->prio], proc);
	pthread_mutex_unlock(&queue_lock);
}

void add_mlq_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&mlq_ready_queue[proc->prio], proc);
	pthread_mutex_unlock(&queue_lock);	
}

struct pcb_t * get_proc(int *slot_of_queue_of_proc) {
	return get_mlq_proc(slot_of_queue_of_proc);
}

void put_proc(struct pcb_t * proc) {
	return put_mlq_proc(proc);
}

void add_proc(struct pcb_t * proc) {
	return add_mlq_proc(proc);
}

void decrease_slot(int used_slot, unsigned long prio) {
	pthread_mutex_lock(&queue_lock);
	update_slot(&mlq_ready_queue[prio], mlq_ready_queue[prio].slot - used_slot);
	pthread_mutex_unlock(&queue_lock);
}


#else
struct pcb_t * get_proc(void) {
	struct pcb_t * proc = NULL;
	/*TODO: get a process from [ready_queue].
	 * Remember to use lock to protect the queue.
	 * */

	pthread_mutex_lock(&queue_lock);
	if (!empty(&ready_queue)) {
		proc = dequeue(&ready_queue);
	}

	pthread_mutex_unlock(&queue_lock);
	return proc;
}

void put_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&run_queue, proc);
	pthread_mutex_unlock(&queue_lock);
}

void add_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&ready_queue, proc);
	pthread_mutex_unlock(&queue_lock);	
}
#endif


