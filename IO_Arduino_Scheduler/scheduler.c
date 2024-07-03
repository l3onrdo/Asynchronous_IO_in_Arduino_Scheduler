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
