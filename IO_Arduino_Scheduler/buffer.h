#include <stdint.h>
#define BUFFER_SIZE 64

typedef struct {
    char data[BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t size;
} buffer_t;

void buffer_init(buffer_t *buffer);
void buffer_put(buffer_t *buffer, char data);
char buffer_get(buffer_t *buffer);


buffer_t read_buffer;
buffer_t write_buffer;