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



// interrupt che gestisce la ricezione di un carattere
ISR(USART0_RX_vect){
    char c = usart_getchar();
  
    buffer_put(&read_buffer, c);
    
    read_wakeup();
}

TCB write1_tcb;
uint8_t write1_stack[IDLE_STACK_SIZE];
void write1_fn(uint32_t thread_arg __attribute__((unused))){
  while(1) {
    cli();
    
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
