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
void get_data(buffer_t *buffer) {
    if(buffer->head == buffer->tail) {
        printf("Buffer vuoto\n");
    }
    else {
        uint8_t data = buffer_get(buffer);
        printf("Dato letto: %d\n", data);
    }

}

//funzione per scrivere dati nel buffer
void put_data(buffer_t *buffer, uint8_t data) {
    if((buffer->head + 1) % BUFFER_SIZE == buffer->tail) {
        printf("Buffer pieno\n");
    }
    else {
        buffer_put(buffer, data);
        printf("Dato scritto: %d\n", data);
    }
}



