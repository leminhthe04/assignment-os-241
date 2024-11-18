#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t * q) {
        if (q->size == 0)
                return 1;
        return 0;
}

int full(struct queue_t * q) {
        if (q->size == MAX_QUEUE_SIZE)
                return 1;
        return 0;
}

void enqueue(struct queue_t * q, struct pcb_t * proc) {
        /* TODO: put a new process to queue [q] */
        if (full(q)){
                printf("Queue is full, cannot enqueue process.\n");                
                return;
        }

        q->proc[q->size++] = proc;
}

struct pcb_t * dequeue(struct queue_t * q) {
        /* TODO: return a pcb whose prioprity is the highest
         * in the queue [q] and remember to remove it from q
         * */

        if (empty(q)) {
                printf("Queue is empty, cannot dequeue process.\n");                
                return NULL;
        }

        struct pcb_t * ret_proc = q->proc[0];

        // left-shifting the rest queue
        for(int i = 1; i < q->size; ++i){
                q->proc[i-1] = q->proc[i];
        } 

        q->size--;
        return ret_proc;
}


void update_slot(struct queue_t * q, int new_val) {
        if(new_val < 0){
                new_val = 0;
        }
        q->slot = new_val;
}

