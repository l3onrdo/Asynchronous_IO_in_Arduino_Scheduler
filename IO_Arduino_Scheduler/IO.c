#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include "tcb.h"
#include "tcb_list.h"
#include "uart.h"
#include "scheduler.h"
#include "buffer.h"
#include "IO.h"


//funzione per leggere dati dal buffer
char getChar(buffer_t *buffer) {
    if(buffer->size == 0) {
        //deve mettere il processo di lettura in attesa
        write_wait();
    }
    return buffer_read(buffer);
}


//funzione per scrivere dati nel buffer
void putChar(buffer_t *buffer, char data) {
    if(buffer->size == BUFFER_SIZE) {
        //deve mettere il processo di scrittura in attesa
        read_wait();
        
    }
    buffer_write(buffer, data);
}



