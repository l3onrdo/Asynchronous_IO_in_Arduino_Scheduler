/**
 * Questo file contiene l'implementazione delle funzioni per la gestione dello scheduling 
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <assert.h>
#include "tcb.h"
#include "tcb_list.h"
#include "atomport_asm.h"
#include "timer.h"




// il processo in esecuzione
TCB* current_tcb=NULL;

// la coda dei processi in esecuzione
TCBList running_queue={
  .first=NULL,
  .last=NULL,
  .size=0
};

//aggiungo la coda dei processi di lettura e scrittra in attesa di essere eseguiti
TCBList read_queue={
  .first=NULL,
  .last=NULL,
  .size=0
};

TCBList write_queue={
  .first=NULL,
  .last=NULL,
  .size=0
};


/**
 * disabilita gli interrupt, estrae il primo processo dalla coda
 * avvia il timer e ripristina il primo processo.
 */
void startSchedule(void){
  cli();
  current_tcb=TCBList_dequeue(&running_queue);
  assert(current_tcb);
  timerStart();
  archFirstThreadRestore(current_tcb);
}

/**
 * mette il processo corrente nella coda estrae il prossimo processo e passa al suo contesto. 
 * Se il processo estratto è lo stesso non viene eseguito lo switch.
 */
void schedule(void) {
  TCB* old_tcb=current_tcb;
  // mettiamo il processo corrente nella coda dei processi in esecuzione
  TCBList_enqueue(&running_queue, current_tcb);

  // estraiamo il prossimo processo
  current_tcb=TCBList_dequeue(&running_queue);
  // passiamo al suo contesto (inutile se è l'unico processo)
  if (old_tcb!=current_tcb)
    archContextSwitch(old_tcb, current_tcb);
}

/**
 * mette in attesa il processo di lettura e prende il prossimo processo dalla coda di running
 */ 
void read_wait(void){
  TCB* old_tcb=current_tcb;
  // mettiamo il processo corrente nella coda dei processi in attaesa di lettura
  TCBList_enqueue(&read_queue, current_tcb);
  printf("metto in attesa il processo di lettura\n");
  // estraiamo il prossimo processo
  current_tcb=TCBList_dequeue(&running_queue);
  // passiamo al suo contesto (inutile se è l'unico processo)
  
  archContextSwitch(old_tcb, current_tcb);
}

/**
 * mette in attesa il processo di scrittura e prende il prossimo processo dalla coda di running
 */
void write_wait(void){
  TCB* old_tcb=current_tcb;
  // mettiamo il processo corrente nella coda dei processi in attesa di scrittura
  TCBList_enqueue(&write_queue, current_tcb);
  printf("metto in attesa il processo di scrittura\n");
  // estraiamo il prossimo processo
  current_tcb=TCBList_dequeue(&running_queue);
  // passiamo al suo contesto (inutile se è l'unico processo)
  
  archContextSwitch(old_tcb, current_tcb);
}

/**
 * mette il processo di lettura in esecuzione e mette in attesa il processo corrente
 */
void read_wakeup(void){
 // estraiamo il prossimo processo per la lettura
  TCB* temp_tcb=TCBList_dequeue(&read_queue); 
  // mettiamo il processo corrente nella coda dei processi in esecuzione
  TCBList_enqueue(&running_queue, current_tcb);
  //chiamo lo scheduler
  printf("risveglio il processo di lettura\n");
  schedule();
}

/**
 * mette il processo di scrittura in esecuzione e mette in attesa il processo corrente
 */
void write_wakeup(void){
  // estraiamo il prossimo processo per la scrittura
  TCB* temp_tcb=TCBList_dequeue(&write_queue); 
  // mettiamo il processo corrente nella coda dei processi in esecuzione
  TCBList_enqueue(&running_queue, current_tcb);
  //chiamo lo scheduler
  printf("risveglio il processo di scrittura\n");
  schedule();
}
