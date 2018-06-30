
#define STACK_VAR (h->alloc_stack)
#include <common/norec.h>

#define KMALLOC_ALLOC_STACK_SIZE 32

struct kmalloc_heap {

   uptr vaddr;
   size_t size;
   size_t mem_allocated;
   void *metadata_nodes;

   size_t min_block_size;
   size_t alloc_block_size;

   virtual_alloc_and_map_func valloc_and_map;
   virtual_free_and_unmap_func vfree_and_unmap;

   /* -- pre-calculated values -- */
   size_t heap_data_size_log2;
   size_t alloc_block_size_log2;
   size_t metadata_size;
   uptr heap_over_end; /* addr + size == last_heap_byte + 1 */
   /* -- */

   bool linear_mapping;

   // Explicit stack used by internal_kmalloc()
   struct explicit_stack_elem2 alloc_stack[KMALLOC_ALLOC_STACK_SIZE];
};

