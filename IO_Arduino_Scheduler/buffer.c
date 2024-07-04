#include "buffer.h"

//funzione per inizializzare il buffer
void buffer_init(buffer_t *buffer) {
  buffer->head = 0;
  buffer->tail = 0;
  buffer->size = 0;
}

//funzione per inserire un dato nel buffer
void buffer_put(buffer_t *buffer, char data) {
  buffer->data[buffer->head] = data;
  buffer->head = (buffer->head + 1) % BUFFER_SIZE;
  buffer->size++;
}


//funzione per estrarre un dato dal buffer
char buffer_get(buffer_t *buffer) {
  char data = buffer->data[buffer->tail];
  buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
  buffer->size--;
  return data;
}