/**
 * Implementazione delle funzioni per la gestione di una lista di TCB
 */

#include "tcb_list.h"
#include <stdio.h>

/**
 * Rimuove il primo processo dalla coda e lo disconnette.
 */
TCB* TCBList_dequeue(TCBList* list){
  TCB* tcb=list->first;
  if (tcb==NULL)
    return tcb;
  
  if (list->size==1) {
    list->first = list->last = NULL;
  } else {
    TCB* next=tcb->next;
    list->first=next;
    next->prev=next;
  }
  --list->size;
  tcb->next=NULL;
  tcb->prev=NULL;
  return tcb;
}

/**
 * Aggiunge un processo alla fine della coda.
 */
uint8_t TCBList_enqueue(TCBList* list, TCB* tcb){
  if(tcb->prev!=NULL || tcb->next!=NULL)
    return ERROR;
  
  if (!list->size) {
    list->first=tcb;
    list->last=tcb;
    tcb->prev=tcb;
    tcb->next=NULL;
  } else {
    list->last->next=tcb;
    tcb->prev=list->last;
    tcb->next=NULL;
    list->last=tcb;
  }
  ++list->size;
  return OK;
}

/**
 * Stampa la lista di TCB.
 */
void TCBList_print(TCBList* list){
  TCB* aux=list->first;
  printf("start-list\n");
  while(aux!=NULL){
    printf("[c: %p, p: %p, n: %p]\n ",
           aux, aux->prev, aux->next);
    aux=aux->next;
  }
  printf("end-list\n");
}
