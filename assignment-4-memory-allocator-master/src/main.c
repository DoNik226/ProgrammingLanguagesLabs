#define _DEFAULT_SOURCE

#include "mem.h"
#include "mem_internals.h"
#include "util.h"
#include <assert.h>
#include <sys/mman.h>

void debug(const char *fmt, ...);

struct block_header *block_get_header(void *contents) {
    return (struct block_header *) (((uint8_t *) contents) - offsetof(struct block_header, contents));
}

void _test_end(size_t test_number) {
    printf(">>> Test #%zu passed...\n", test_number);
}

//успешное выделение памяти
void _test_simple_memory_allocation() {
    size_t test_number = 1;
    void *heap = heap_init(REGION_MIN_SIZE);
    assert(heap);

    void* data = _malloc(100);
    struct block_header *block = block_get_header(data);

    assert(!block->is_free);


    heap_term();
    _test_end(test_number);
}
//освобождение одного блока из нескольких выделенных
void _test_free_one_block_from_many_allocated() {
    size_t test_number = 2;
    void *heap = heap_init(REGION_MIN_SIZE);
    assert(heap);

    void* data1 = _malloc(100);
    void* data2 = _malloc(100);

    struct block_header *block1 = block_get_header(data1);
    struct block_header *block2 = block_get_header(data2);

    assert(!block1->is_free);
    assert(!block2->is_free);

    _free(block1);

    assert(block1->is_free);
    assert(!block2->is_free);

    heap_term();
    _test_end(test_number);
}
//освобождение двух блоков из нескольких выделенных
void _test_free_two_block_from_many_allocated() {
    size_t test_number = 3;
    void *heap = heap_init(REGION_MIN_SIZE);
    assert(heap);

    void* data1 = _malloc(100);
    void* data2 = _malloc(100);
    void* data3 = _malloc(100);

    struct block_header *block1 = block_get_header(data1);
    struct block_header *block2 = block_get_header(data2);
    struct block_header *block3 = block_get_header(data3);

    assert(!block1->is_free);
    assert(!block2->is_free);
    assert(!block3->is_free);

    _free(block1);

    assert(block1->is_free);
    assert(!block2->is_free);
    assert(!block3->is_free);

    _free(block2);

    assert(block1->is_free);
    assert(block2->is_free);
    assert(!block3->is_free);

    heap_term();
    _test_end(test_number);
}
//память закончилась
void _test_grow_heap() {
    size_t test_number = 4;

   void *heap = heap_init(REGION_MIN_SIZE);
    assert(heap);

    void *pointer = _malloc(REGION_MIN_SIZE);

    struct block_header *header = block_get_header(pointer);
    assert(header->capacity.bytes == REGION_MIN_SIZE);
    assert(size_from_capacity(header -> capacity).bytes > REGION_MIN_SIZE);

    heap_term();
    _test_end(test_number);
}




int main() {
    debug("Running tests...\n");
    _test_simple_memory_allocation();
    _test_free_one_block_from_many_allocated();
    _test_free_two_block_from_many_allocated();
    _test_grow_heap();
    debug("Tests is passed.\n");
    return 0;

}


