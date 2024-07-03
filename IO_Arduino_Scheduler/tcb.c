/**
 * Implementazione delle funzioni per la gestione dei TCB.
 */

#include "tcb.h"
#include <avr/interrupt.h>

extern TCB* current_tcb;

/**
 * Funzione per avviare il thread.
 * 
 * Questa funzione viene chiamata all'avvio del thread e si occupa di
 * abilitare le interruzioni, chiamare il punto di ingresso del thread
 * e impostare lo stato del thread come "Terminated" una volta che la
 * funzione del thread è terminata.
 */
static void _trampoline(void){
  sei();
  /* Chiamare il punto di ingresso del thread */
  if (current_tcb && current_tcb->thread_fn) {
    (*current_tcb->thread_fn)(current_tcb->thread_arg);
  }

  // Impostare lo stato del thread come "Terminated" una volta che la funzione del thread è terminata
  current_tcb->status=Terminated;
}

/**
 * Crea un nuovo TCB.
 * 
 * Questa funzione inizializza un nuovo TCB con i valori forniti e prepara
 * lo stack per l'esecuzione.
 */
void TCB_create(TCB* tcb, Pointer stack_top, ThreadFn thread_fn, uint32_t thread_arg){
  // Inizializza le variabili del TCB
  tcb->thread_fn=thread_fn;
  tcb->thread_arg=thread_arg;
  tcb->prev=NULL;
  tcb->next=NULL;
  tcb->status=Ready;
  
  /** Prepara lo stack per il processo **/
  
  uint8_t *stack_ptr = (uint8_t *)stack_top;

  /** Scrivi l'indirizzo di ritorno della funzione chiamata (il trampoline) */
  *stack_ptr-- = (uint8_t)((uint16_t)_trampoline & 0xFF);
  *stack_ptr-- = (uint8_t)(((uint16_t)_trampoline >> 8) & 0xFF);

  // Questo è per i dispositivi con più di 64K di memoria di programma
#ifdef __AVR_3_BYTE_PC__
  *stack_ptr-- = 0;
#endif


  /**
   * Salva i valori dei registri di avvio per R2-R17, R28-R29
   */
  *stack_ptr-- = 0x00;    /* R2 */
  *stack_ptr-- = 0x00;    /* R3 */
  *stack_ptr-- = 0x00;    /* R4 */
  *stack_ptr-- = 0x00;    /* R5 */
  *stack_ptr-- = 0x00;    /* R6 */
  *stack_ptr-- = 0x00;    /* R7 */
  *stack_ptr-- = 0x00;    /* R8 */
  *stack_ptr-- = 0x00;    /* R9 */
  *stack_ptr-- = 0x00;    /* R10 */
  *stack_ptr-- = 0x00;    /* R11 */
  *stack_ptr-- = 0x00;    /* R12 */
  *stack_ptr-- = 0x00;    /* R13 */
  *stack_ptr-- = 0x00;    /* R14 */
  *stack_ptr-- = 0x00;    /* R15 */
  *stack_ptr-- = 0x00;    /* R16 */
  *stack_ptr-- = 0x00;    /* R17 */
  *stack_ptr-- = 0x00;    /* R28 */
  *stack_ptr-- = 0x00;    /* R29 */

    /**
     * Nei dispositivi con uno spazio di programma ampio, viene anche
     * eseguito il context switch di RAMPZ, EIND.
     */
#ifdef __AVR_3_BYTE_PC__
  *stack_ptr-- = 0x00;    /* RAMPZ */
  *stack_ptr-- = 0x00;    /* EIND */
#endif

  /**
     * Tutti i contesti dei thread sono stati inizializzati. Salva il
     * puntatore allo stack corrente nel TCB del thread in modo che
     * sappia da dove iniziare a cercare quando il thread viene avviato.
     */
  tcb->sp_save_ptr = stack_ptr;

}
