/*
*	Operating System Lab
*	    Lab2 (Synchronization)
*	    Student id : 
*	    Student name : 
*
*   lab2_queue.c :
*       - thread-safe queue code.
*       - coarse-grained, fine-grained lock code
*
*   Implement thread-safe queue for coarse-grained version and fine-grained version.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>

#include "lab2_sync_types.h"

int print_inorder_recur(lab2_node* node)
{
    if(node == NULL)
        return 0;

    return print_inorder_recur(node->next)+1;
}
/*
 * TODO
 *  Implement funtction which traverse queue in in-order
 *  
 *  @param lab2_queue *queue  : queue to print in-order. 
 *  @return                 : status (success or fail)
 */
int lab2_node_print_inorder(lab2_queue *queue) {
    // You need to implement lab2_node_print_inorder function.
    return print_inorder_recur(queue->head);
}

/*
 * TODO
 *  Implement function which creates struct lab2_queue
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_queue )
 * 
 *  @return                 : queue which you created in this function.
 */
lab2_queue *lab2_queue_create() {
    // You need to implement lab2_queue_create function.
    lab2_queue* queue = (lab2_queue*)malloc(sizeof(lab2_queue));
    queue->head = queue->tail =  NULL;
    pthread_mutex_init(&queue->head_lock, NULL);
    pthread_mutex_init(&queue->tail_lock, NULL);

    return queue;
}

/*
 * TODO
 *  Implement function which creates struct lab2_node
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param int key          : queue node's key to creates
 *  @return                 : queue node which you created in this function.
 */
lab2_node * lab2_node_create(int key) {
    // You need to implement lab2_node_create function.
    lab2_node* node = (lab2_node*)malloc(sizeof(lab2_node));
    node->key = key;
    node->next = NULL;
    return node;
}

/* 
 * TODO
 *  Implement a function which enqueue nodes from the queue. 
 *  
 *  @param lab2_queue *queue      : queue which you need to enqueue new node.
 *  @param lab2_node *new_node  : queue node which you need to enqueue. 
 *  @return                 : satus (success or fail)
 */
int lab2_node_enqueue(lab2_queue *queue, lab2_node *new_node){
    // You need to implement lab2_node_enqueue function.
	if(queue->head == NULL)
		queue->head = queue->tail = new_node;
	else
	{
		queue->tail->next = new_node;
		queue->tail = new_node;
	}

    return LAB2_SUCCESS;
}  

/* 
 * TODO
 *  Implement a function which enqueue nodes from the queue in fine-garined manner.
 *
 *  @param lab2_queue *queue      : queue which you need to enqueue new node in fine-grained manner.
 *  @param lab2_node *new_node  : queue node which you need to enqueue. 
 *  @return                     : status (success or fail)
 */
int lab2_node_enqueue_fg(lab2_queue *queue, lab2_node *new_node){
	if(queue->head == NULL)
		queue->head = queue->tail = new_node;
	else
	{
		pthread_mutex_lock(&queue->tail_lock);
		queue->tail->next = new_node;
		queue->tail = new_node;
		pthread_mutex_unlock(&queue->tail_lock);
	}
    return LAB2_SUCCESS;
}

/* 
 * TODO
 *  Implement a function which enqueue nodes from the queue in coarse-garined manner.
 *
 *  @param lab2_queue *queue      : queue which you need to enqueue new node in coarse-grained manner.
 *  @param lab2_node *new_node  : queue node which you need to enqueue. 
 *  @return                     : status (success or fail)
 */
int lab2_node_enqueue_cg(lab2_queue *queue, lab2_node *new_node){
    // You need to implement lab2_node_enqueue_cg function.       
	pthread_mutex_lock(&queue->tail_lock);

	lab2_node_enqueue(queue, new_node);

    pthread_mutex_unlock(&queue->tail_lock);
    return LAB2_SUCCESS;
}

/* 
 * TODO
 *  Implement a function which dequeue nodes from the queue.
 *
 *  @param lab2_queue *queue  : queue tha you need to dequeue node from queue which contains key.
 *  @return                 : status (success or fail)
 */

int lab2_node_dequeue(lab2_queue *queue) {
    // You need to implement lab2_node_dequeue function.
	if(queue->head == NULL)
	{
		printf("queue is empty");
		return LAB2_ERROR;
	}

	lab2_node* tmp = queue->head;
	queue->head = queue->head->next;
	free(tmp);
	
    return LAB2_SUCCESS;
}

/* 
 * TODO
 *  Implement a function which dequeue nodes from the queue in fine-grained manner.
 *
 *  @param lab2_queue *queue  : queue tha you need to dequeue node in fine-grained manner from queue which contains key.
 *  @return                 : status (success or fail)
 */
int lab2_node_dequeue_fg(lab2_queue *queue) {
    // You need to implement lab2_node_dequeue_fg function.
	pthread_mutex_lock(&queue->head_lock);

	if(queue->head == NULL)
	{
		printf("queue is empty");

		pthread_mutex_unlock(&queue->head_lock);
		return LAB2_ERROR;
	}

	lab2_node* tmp = queue->head;
	queue->head = queue->head->next;
	pthread_mutex_unlock(&queue->head_lock);
	
	free(tmp);
    return LAB2_SUCCESS;
}


/* 
 * TODO
 *  Implement a function which dequeue nodes from the queue in coarse-grained manner.
 *
 *  @param lab2_queue *queue  : queue tha you need to dequeue node in coarse-grained manner from queue which contains key.
 *  @return                 : status (success or fail)
 */
int lab2_node_dequeue_cg(lab2_queue *queue) {
	pthread_mutex_lock(&queue->head_lock);
	lab2_node_dequeue(queue);
	pthread_mutex_unlock(&queue->head_lock);
    return LAB2_SUCCESS;
}


/*
 * TODO
 *  Implement function which delete struct lab2_queue
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param lab2_queue *queue  : queue which you want to delete. 
 *  @return                 : status(success or fail)
 */
void lab2_queue_delete(lab2_queue *queue) {
    // You need to implement lab2_queue_delete function.
    free(queue);
    queue = NULL;
}

/*
 * TODO
 *  Implement function which delete struct lab2_node
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param lab2_queue *queue  : queue node which you want to dequeue. 
 *  @return                 : status(success or fail)
 */
void lab2_node_delete(lab2_node *node) {
    // You need to implement lab2_node_delete function.
    free(node);
    node = NULL;
}

