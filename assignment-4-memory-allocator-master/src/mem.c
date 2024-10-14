#define _DEFAULT_SOURCE

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mem_internals.h"
#include "mem.h"
#include "util.h"

void debug_block(struct block_header* b, const char* fmt, ... );
void debug(const char* fmt, ... );

extern inline block_size size_from_capacity( block_capacity cap );
extern inline block_capacity capacity_from_size( block_size sz );

static bool            block_is_big_enough( size_t query, struct block_header* block ) { return block->capacity.bytes >= query; }
static size_t          pages_count   ( size_t mem )                      { return mem / getpagesize() + ((mem % getpagesize()) > 0); }
static size_t          round_pages   ( size_t mem )                      { return getpagesize() * pages_count( mem ) ; }

static void block_init( void* restrict addr, block_size block_sz, void* restrict next ) {
  *((struct block_header*)addr) = (struct block_header) {
    .next = next,
    .capacity = capacity_from_size(block_sz),
    .is_free = true
  };
}

static size_t region_actual_size( size_t query ) { return size_max( round_pages( query ), REGION_MIN_SIZE ); }

extern inline bool region_is_invalid( const struct region* r );



static void* map_pages(void const* addr, size_t length, int additional_flags) {
  return mmap( (void*) addr, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | additional_flags , -1, 0 );
}

static struct region alloc_region(void const* addr, size_t query) {
    size_t reg_size = region_actual_size(size_from_capacity((block_capacity) { query }).bytes);
    void* alloc_adr = map_pages(addr, reg_size, MAP_FIXED_NOREPLACE);
    if (alloc_adr == MAP_FAILED) {
        alloc_adr = map_pages(addr, reg_size, 0);
        if (alloc_adr == MAP_FAILED) {
            return REGION_INVALID;
        }
    }
    struct region result = (struct region){
            .size = reg_size,
            .extends = addr == alloc_adr,
            .addr = alloc_adr
    };
    block_init(alloc_adr, (block_size) { reg_size }, NULL);
    return result;
}


static void* block_after( struct block_header const* block )         ;

void* heap_init( size_t initial ) {
  const struct region region = alloc_region( HEAP_START, initial );
  if ( region_is_invalid(&region) ) return NULL;

  return region.addr;
}

/*  освободить всю память, выделенную под кучу */

static bool blocks_continuous (
                               struct block_header const* fst,
                               struct block_header const* snd ) {
  return (void *)snd == block_after(fst);
}

void heap_term( ) {
  struct block_header *current_block = (struct block_header *) HEAP_START;
    while (current_block != NULL) {
        struct block_header *next_block = current_block->next;
        while (next_block && blocks_continuous(current_block, next_block)) {
            current_block->capacity.bytes += next_block->capacity.bytes;
            current_block->next = next_block->next;
            next_block = current_block->next;
        }
        if (munmap(current_block, size_from_capacity(current_block->capacity).bytes) == -1) {
  fprintf(stderr, "Failed to munmap\n");
        }
        current_block = next_block;
    }
}

#define BLOCK_MIN_CAPACITY 24

/*  --- Разделение блоков (если найденный свободный блок слишком большой )--- */

static bool block_splittable( struct block_header* restrict block, size_t query) {
  return block-> is_free && query + offsetof( struct block_header, contents ) + BLOCK_MIN_CAPACITY <= block->capacity.bytes;
}

static bool split_if_too_big( struct block_header* block, size_t query ) {
  if (block && block_splittable(block, query)) {
    block_size first_block_size = size_from_capacity((block_capacity){query});
    block_size second_block_size = {size_from_capacity(block->capacity).bytes - first_block_size.bytes};
	
    void *second_block = (void *)block + first_block_size.bytes;

    block_init(second_block, second_block_size, block->next);
    block_init(block, first_block_size, second_block);
    return true;
  }
  return false;
}


/*  --- Слияние соседних свободных блоков --- */

static void* block_after( struct block_header const* block )              {
  return  (void*) (block->contents + block->capacity.bytes);
}

static bool mergeable(struct block_header const *restrict fst, struct block_header const *restrict snd) {
  return fst->is_free && snd->is_free && blocks_continuous( fst, snd ) ;
}

static bool try_merge_with_next(struct block_header *block) {
    struct block_header *next_block = block->next;

    if (next_block == NULL || !mergeable(block, next_block)) {
        return false;
    }

    block->capacity = (block_capacity) {.bytes = block->capacity.bytes + size_from_capacity(next_block->capacity).bytes}; 
    block->next = next_block->next; 

    return true;
}



/*  --- ... ecли размера кучи хватает --- */

struct block_search_result {
  enum {BSR_FOUND_GOOD_BLOCK, BSR_REACHED_END_NOT_FOUND, BSR_CORRUPTED} type;
  struct block_header* block;
};


static struct block_search_result find_good_or_last(struct block_header* restrict block, size_t sz) {
    if (!block) return (struct block_search_result) { .type = BSR_CORRUPTED };
    sz = size_max(sz, BLOCK_MIN_CAPACITY);
    struct block_header* current = block;
    struct block_header* found = NULL;
    while (current) {
        while (try_merge_with_next(current)) {}
        if (block_is_big_enough(sz, current) && current->is_free)
            return (struct block_search_result) { .type = BSR_FOUND_GOOD_BLOCK, .block = current };
        found = current;
        current = current->next;
    }
    return (struct block_search_result) { .type = BSR_REACHED_END_NOT_FOUND, .block = found };
}


/*  Попробовать выделить память в куче начиная с блока `block` не пытаясь расширить кучу
 Можно переиспользовать как только кучу расширили. */
static struct block_search_result try_memalloc_existing ( size_t query, struct block_header* block ) {
  struct block_search_result result = find_good_or_last(block, query);
    if (result.type == BSR_FOUND_GOOD_BLOCK) {
        split_if_too_big(result.block, query);
        result.block->is_free = false;
        return (struct block_search_result) {.type = BSR_FOUND_GOOD_BLOCK, .block = result.block};
    }
    
    return result;
}



static struct block_header* grow_heap( struct block_header* restrict last, size_t query ) {
  struct region next_region = alloc_region(block_after(last), query);
    if (region_is_invalid(&next_region)) {
        return NULL;
    }
    last->next = next_region.addr;
    if (try_merge_with_next(last)) {
        return last;
    }
    return next_region.addr;
}

/*  Реализует основную логику malloc и возвращает заголовок выделенного блока */
static struct block_header* memalloc( size_t query, struct block_header* heap_start) {

  query = size_max(query, BLOCK_MIN_CAPACITY);
  struct block_search_result res = try_memalloc_existing(query, heap_start);
	if (res.type == BSR_CORRUPTED)
        return NULL;
    if (res.type == BSR_REACHED_END_NOT_FOUND) {
        grow_heap(res.block, query);
        res = try_memalloc_existing(query, heap_start);
        if (res.type != BSR_FOUND_GOOD_BLOCK)
            return NULL;
        else
            return res.block;
    }
return res.block;

}

void* _malloc( size_t query ) {
  struct block_header* const addr = memalloc( query, (struct block_header*) HEAP_START );
  if (addr) return addr->contents;
  else return NULL;
}

static struct block_header* block_get_header(void* contents) {
  return (struct block_header*) (((uint8_t*)contents)-offsetof(struct block_header, contents));
}

void _free( void* mem ) {
  if (!mem) return ;
  struct block_header* header = block_get_header( mem );
  header->is_free = true;
  while (try_merge_with_next(header));
}

