#include <avr/interrupt.h>
#include <avr/io.h>
#include <assert.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <util/atomic.h>
#include "tcb.h"
#include "tcb_list.h"
#include "uart.h"
#include "atomport_asm.h"
#include "scheduler.h"
#include "buffer.h"
#include "IO.h"
#define THREAD_STACK_SIZE 256
#define IDLE_STACK_SIZE 128

char* print_w1="W1:\n";
char* print_w2="W2:\n";
char* print_r1="R1\n";
char* print_r2="R2\n";
uint8_t w1=1;
uint8_t w2=1;
uint8_t new_line=1;



// interrupt che gestisce la ricezione di un carattere
ISR(USART0_RX_vect){
  //setta i valori per le variabili di controllo per la stampa
  w1=1;
  w2=1;
  new_line=1;
  //mette il carattere nel buffer di lettura
  char c = usart_getchar();
  buffer_write(&read_buffer, c);
  //mette il processo di scrittura in ready
  read_wakeup();
}

TCB write1_tcb;
uint8_t write1_stack[IDLE_STACK_SIZE];
void write1_fn(uint32_t thread_arg __attribute__((unused))){
  while(1) {
    cli();
    //stamapa print_w1 lo faccio atomico per evitare probleme di concorrenza
    ATOMIC_BLOCK(ATOMIC_FORCEON){
      if(w1 && write_buffer.size >0){
        if(!new_line){
          usart_putchar('\n');
        }
        //stampa la frase print_w1
        for(int i=0; i<strlen(print_w1); i++){
          usart_putchar(print_w1[i]);
        }
        w1=0;
        w2=1;
        new_line=0;
      }
    }
    //stampa il carattere dal buffer di scrittura
    if(write_buffer.size > 0){
      usart_putchar(getChar(&write_buffer));
    }
    
    read_wakeup();
    sei();
    _delay_ms(10);

  }
}

TCB write2_tcb;
uint8_t write2_stack[IDLE_STACK_SIZE];
void write2_fn(uint32_t thread_arg __attribute__((unused))){
  while(1) {
    cli();
    //stamapa print_w2 lo faccio atomico per evitare probleme di concorrenza
    ATOMIC_BLOCK(ATOMIC_FORCEON){
      if(w2 && write_buffer.size >0){
        if(!new_line){
          usart_putchar('\n');
        }
        //stampa la frase print_w2
        for(int i=0; i<strlen(print_w2); i++){
          usart_putchar(print_w2[i]);
        }
        w2=0;
        w1=1;
        new_line=0;
      }
    }

    if(write_buffer.size > 0){
      usart_putchar(getChar(&write_buffer));
    } 
    read_wakeup();
    sei();
    _delay_ms(10);

  }
}

TCB read1_tcp;
uint8_t read1_stack[THREAD_STACK_SIZE];
void read1_fn(uint32_t arg __attribute__((unused))){
  while(1){
    cli();
    ATOMIC_BLOCK(ATOMIC_FORCEON){
      if(read_buffer.size >0){
        //mette il carattere di new line se non è stato stampato
        if(!new_line){
          usart_putchar('\n');
        }
        
        for(int i=0; i<strlen(print_r1); i++){
          usart_putchar(print_r1[i]);
        }
        new_line=1;
        w1=1;
        w2=1;
      }
    }
    
    //leggo dal buffer e scrivo sul buffer di scrittura
    if(read_buffer.size > 0){
      char read = getChar(&read_buffer);
      putChar(&write_buffer, read);
    }

    write_wakeup();
    sei();
    _delay_ms(10);
  }
}

TCB read2_tcp;
uint8_t read2_stack[THREAD_STACK_SIZE];
void read2_fn(uint32_t arg __attribute__((unused))){
  while(1){
    cli();
    ATOMIC_BLOCK(ATOMIC_FORCEON){
      
      if(read_buffer.size >0){
        //mette il carattere di new line se non è stato stampato
        if(!new_line){
          usart_putchar('\n');
        }
        for(int i=0; i<strlen(print_r2); i++){
          usart_putchar(print_r2[i]);
        }
        new_line=1;
        w1=1;
        w2=1;
      }
    }
    //leggo dal buffer e scrivo sul buffer di scrittura
    if(read_buffer.size > 0){
      char read = getChar(&read_buffer);
      putChar(&write_buffer, read);
    }
    
    write_wakeup();
    sei();
    _delay_ms(10);
  }
}


int main(void){
  // we need printf for debugging
  printf_init();
  // inizializzo i buffer
  buffer_init(&read_buffer);
  buffer_init(&write_buffer);
  
  //creo i processi
  TCB_create(&write1_tcb,
             write1_stack+IDLE_STACK_SIZE-1,
             write1_fn,
             0);

  TCB_create(&write2_tcb,
              write2_stack+IDLE_STACK_SIZE-1,
              write2_fn,
              0);

  TCB_create(&read1_tcp,
              read1_stack+THREAD_STACK_SIZE-1,
              read1_fn,
              0);

  TCB_create(&read2_tcp,
              read2_stack+THREAD_STACK_SIZE-1,
              read2_fn,
              0);
  // metto i processi nella coda dei processi pronti
  TCBList_enqueue(&running_queue, &read1_tcp);
  TCBList_enqueue(&running_queue, &read2_tcp);
  TCBList_enqueue(&running_queue, &write1_tcb);
  TCBList_enqueue(&running_queue, &write2_tcb);


  startSchedule();
}
