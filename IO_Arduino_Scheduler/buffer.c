#include "buffer.h"

//funzione per inizializzare il buffer
void buffer_init(buffer_t *buffer) {
  buffer->head = 0;
  buffer->tail = 0;

}

//funzione per inserire un dato nel buffer
void buffer_put(buffer_t *buffer, uint8_t data) {
  buffer->data[buffer->head] = data;
  buffer->head = (buffer->head + 1) % BUFFER_SIZE;
}


//funzione per estrarre un dato dal buffer
uint8_t buffer_get(buffer_t *buffer) {
  uint8_t data = buffer->data[buffer->tail];
  buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
  return data;
}