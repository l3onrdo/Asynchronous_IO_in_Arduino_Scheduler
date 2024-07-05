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

uint8_t w1=1;
uint8_t w2=1;


// interrupt che gestisce la ricezione di un carattere
ISR(USART0_RX_vect){
  w1=1;
  w2=1;
  char c = usart_getchar();

  buffer_put(&read_buffer, c);
  
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
        if(!w2){
          usart_putchar('\n');
        }        
        //stampa la frase print_w1
        for(int i=0; i<strlen(print_w1); i++){
          usart_putchar(print_w1[i]);
        }
        w1=0;
        w2=1;
      }
    }
    if(write_buffer.size > 0){
      usart_putchar(buffer_get(&write_buffer));
      //printf("%c", get_data(&write_buffer));
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
        if(!w1){
          usart_putchar('\n');
        }
        //stampa la frase print_w2
        for(int i=0; i<strlen(print_w2); i++){
          usart_putchar(print_w2[i]);
        }
        w2=0;
        w1=1;
      }
    }

    if(write_buffer.size > 0){
      //usart_putchar(buffer_get(&write_buffer));
      printf("%c", get_data(&write_buffer));
    } 
    sei();
    _delay_ms(10);
  }
}

TCB read1_tcp;
uint8_t read1_stack[THREAD_STACK_SIZE];
void read1_fn(uint32_t arg __attribute__((unused))){
  while(1){
    cli();
    
    //leggo dal buffer e scrivo sul buffer di scrittura
    if(read_buffer.size > 0){
      char read = buffer_get(&read_buffer);
      put_data(&write_buffer, read);
      write_wakeup();
    }
    sei();
    _delay_ms(10);
  }
}

TCB read2_tcp;
uint8_t read2_stack[THREAD_STACK_SIZE];
void read2_fn(uint32_t arg __attribute__((unused))){
  while(1){
    cli();
    //leggo dal buffer e scrivo sul buffer di scrittura
    if(read_buffer.size > 0){
      char read = buffer_get(&read_buffer);
      put_data(&write_buffer, read);
      write_wakeup();
    }
    
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
  TCBList_enqueue(&running_queue, &write2_tcb);
  TCBList_enqueue(&running_queue, &write1_tcb);
  
  //printf("starting\n");
  startSchedule();
}
