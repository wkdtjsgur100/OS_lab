/*
*	Operating System Lab
*	    Lab2 (Synchronization)
*	    Student id : 
*	    Student name : 
*
*   lab2_bst.c :
*       - thread-safe bst code.
*       - coarse-grained, fine-grained lock code
*
*   Implement thread-safe bst for coarse-grained version and fine-grained version.
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

    return print_inorder_recur(node->left) + print_inorder_recur(node->right)+1;
}
/*
 * TODO
 *  Implement funtction which traverse BST in in-order
 *  
 *  @param lab2_tree *tree  : bst to print in-order. 
 *  @return                 : status (success or fail)
 */
int lab2_node_print_inorder(lab2_tree *tree) {
    // You need to implement lab2_node_print_inorder function.
    return print_inorder_recur(tree->root);
}

/*
 * TODO
 *  Implement function which creates struct lab2_tree
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_tree )
 * 
 *  @return                 : bst which you created in this function.
 */
lab2_tree *lab2_tree_create() {
    // You need to implement lab2_tree_create function.
    lab2_tree* tree = (lab2_tree*)malloc(sizeof(lab2_tree));
    tree->root = NULL;
    pthread_mutex_init(&tree->global_lock, NULL);

    return tree;
}

/*
 * TODO
 *  Implement function which creates struct lab2_node
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param int key          : bst node's key to creates
 *  @return                 : bst node which you created in this function.
 */
lab2_node * lab2_node_create(int key) {
    // You need to implement lab2_node_create function.
    lab2_node* node = (lab2_node*)malloc(sizeof(lab2_node));
    node->key = key;
    node->left = node->right = NULL;
    pthread_mutex_init(&node->mutex, NULL);
    return node;
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST. 
 *  
 *  @param lab2_tree *tree      : bst which you need to insert new node.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                 : satus (success or fail)
 */
lab2_node* insert_recur(lab2_node* root, lab2_node* new_node)
{
    if(root == NULL)
        return new_node;
    
    if(root->key > new_node->key) 
        root->left = insert_recur(root->left, new_node);
    else if(root->key < new_node->key)
        root->right = insert_recur(root->right, new_node);
     // if key is duplicated, just return root
    return root;
}
int lab2_node_insert(lab2_tree *tree, lab2_node *new_node){
    // You need to implement lab2_node_insert function.
    tree->root = insert_recur(tree->root, new_node);
    return LAB2_SUCCESS;
}  

/* 
 * TODO
 *  Implement a function which insert nodes from the BST in fine-garined manner.
 *
 *  @param lab2_tree *tree      : bst which you need to insert new node in fine-grained manner.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                     : status (success or fail)
 */
void insert_fg_recur(lab2_node* root, lab2_node* new_node)
{
    if(root == NULL)
		return;

    if(root->key > new_node->key)
	{
		if(root->left == NULL)
		{
			root->left = new_node;
			pthread_mutex_unlock(&root->mutex);
			printf("root unlocked!!! -leaf\n");
		}
		else
		{
			pthread_mutex_lock(&root->left->mutex);	
			printf("left locked!!!\n");
			pthread_mutex_unlock(&root->mutex);
			printf("root unlocked!!\n");
			insert_recur(root->left, new_node);
		}
	}
    else if(root->key < new_node->key)
	{
		if(root->right == NULL)
		{
			root->right = new_node;
			pthread_mutex_unlock(&root->mutex);
			printf("root unlocked! - leaf right\n");
		}
		else
		{
			pthread_mutex_lock(&root->right->mutex);
			printf("right locked!!!\n");

			pthread_mutex_unlock(&root->mutex);
			printf("root unlocked!!\n");
			insert_recur(root->right, new_node);
		}
	}
	else
	{
		printf("unlock!!!!!!!!\n");
		pthread_mutex_unlock(&root->mutex);
	}
}
int lab2_node_insert_fg(lab2_tree *tree, lab2_node *new_node){
    // You need to implement lab2_node_insert_fg function.
	
	if(tree->root == NULL)
	{
		pthread_mutex_lock(&tree->global_lock);
		tree->root = lab2_node_create(new_node->key);
		pthread_mutex_unlock(&tree->global_lock);
		return LAB2_SUCCESS;
	}
	else if(tree->root->left == NULL && tree->root->right == NULL) // if only root is filled
	{
		pthread_mutex_lock(&tree->root->mutex);
		printf("root is locked\n");
	}
	insert_fg_recur(tree->root, new_node);

    return LAB2_SUCCESS;
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST in coarse-garined manner.
 *
 *  @param lab2_tree *tree      : bst which you need to insert new node in coarse-grained manner.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                     : status (success or fail)
 */
int lab2_node_insert_cg(lab2_tree *tree, lab2_node *new_node){
    // You need to implement lab2_node_insert_cg function.       
    pthread_mutex_lock(&tree->global_lock);
    tree->root = insert_recur(tree->root, new_node);
    pthread_mutex_unlock(&tree->global_lock);

    return LAB2_SUCCESS;
}

/* 
 * TODO
 *  Implement a function which remove nodes from the BST.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */

lab2_node* find_min_node(lab2_node* root)
{
    lab2_node* t = root;
    while(t->left != NULL) 
        t = t->left;
    return t;
}
lab2_node* remove_recur(lab2_node* root, int key)
{
    lab2_node* t_node;
    if(root == NULL)
        return NULL;

    if(root->key == key)
    {
        if(root->left != NULL && root->right != NULL) // If both is not NULL
        {
             t_node = find_min_node(root->right);
			 root->key = t_node->key;
             root->right = remove_recur(root->right, t_node->key);
        }
        else
        {
             t_node = (root->left == NULL) ? root->right : root->left; // Decide returned node.
             free(root);
             return t_node;
        }
    }
    else if(root->key > key)
        root->left = remove_recur(root->left, key);
    else 
        root->right = remove_recur(root->right, key);

    return root;
}
int lab2_node_remove(lab2_tree *tree, int key) {
    // You need to implement lab2_node_remove function.
    tree->root = remove_recur(tree->root, key);
    return LAB2_SUCCESS;
}

/* 
 * TODO
 *  Implement a function which remove nodes from the BST in fine-grained manner.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node in fine-grained manner from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
lab2_node* remove_fg_recur(lab2_node* root, int key)
{
    lab2_node* t_node;
    if(root == NULL)
        return NULL;

	pthread_mutex_lock(&root->mutex);
    if(root->key == key)
    {
        if(root->left != NULL && root->right != NULL) // If both is not NULL
        {
             t_node = find_min_node(root->right);
			 pthread_mutex_unlock(&root->mutex);
             root->right = remove_recur(root->right, t_node->key);
        }
        else
        {
             t_node = (root->left == NULL) ? root->right : root->left; // Decide returned node.
			 pthread_mutex_unlock(&root->mutex);
             free(root);
             return t_node;
        }
    }
    else if(root->key > key)
	{
		pthread_mutex_unlock(&root->mutex);
        root->left = remove_recur(root->left, key);
	}
	else
	{
		pthread_mutex_unlock(&root->mutex);
        root->right = remove_recur(root->right, key);
	}
    return root;
}
int lab2_node_remove_fg(lab2_tree *tree, int key) {
    // You need to implement lab2_node_remove_fg function.
	remove_fg_recur(tree->root, key);
   
    return LAB2_SUCCESS;
}


/* 
 * TODO
 *  Implement a function which remove nodes from the BST in coarse-grained manner.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node in coarse-grained manner from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
int lab2_node_remove_cg(lab2_tree *tree, int key) {
    // You need to implement lab2_node_remove_cg function.
   pthread_mutex_lock(&tree->global_lock);
   tree->root = remove_recur(tree->root, key);
   pthread_mutex_unlock(&tree->global_lock);
   
   return LAB2_SUCCESS;
}


/*
 * TODO
 *  Implement function which delete struct lab2_tree
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param lab2_tree *tree  : bst which you want to delete. 
 *  @return                 : status(success or fail)
 */
void lab2_tree_delete(lab2_tree *tree) {
    // You need to implement lab2_tree_delete function.
    free(tree);
    tree = NULL;
}

/*
 * TODO
 *  Implement function which delete struct lab2_node
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param lab2_tree *tree  : bst node which you want to remove. 
 *  @return                 : status(success or fail)
 */
void lab2_node_delete(lab2_node *node) {
    // You need to implement lab2_node_delete function.
    free(node);
    node = NULL;
}

