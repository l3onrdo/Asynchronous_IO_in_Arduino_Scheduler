#include <stdint.h>
#define BUFFER_SIZE 256

typedef struct {
    uint8_t data[BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
} buffer_t;

void buffer_init(buffer_t *buffer);
void buffer_put(buffer_t *buffer, uint8_t data);
uint8_t buffer_get(buffer_t *buffer);


buffer_t read_buffer;
buffer_t write_buffer;