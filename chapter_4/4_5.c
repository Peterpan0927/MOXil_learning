#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <malloc/malloc.h>

typedef struct interpose_s{
  void *new_func;
  void *orig_func;
}interpose_t;

void *my_malloc(int size);
void my_free (void *);

static const interpose_t interposing_functions[] \
               __attribute__ ((used,section("__DATA, __interpose"))) = {
                { (void *)my_free, (void *)free },
                { (void *)my_malloc, (void *)malloc },
        };

void *my_malloc(int size){
  void *returned = malloc(size);
  malloc_printf("+ %p %d\n", returned, size);
  return (returned);
}

void my_free(void *freed){
  malloc_printf("- %p\n", freed);
  free(freed);
}
