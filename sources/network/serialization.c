#include "processus.h"
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h> 
#include <stdio.h>
#include "serialization.h"

int process_to_string(const processus_t *proc, char *buffer, size_t max_size) {

  int recup = snprintf(buffer, max_size, 
        "%d|%s|%c|%s|%llu|%lu|%lu\n", 
        proc->pid,          
        proc->name,         
        proc->state,        
        proc->user,         
        proc->ram,      
        proc->utime,        
        proc->stime         
    );
    return recup;
}

char* serialize_process_list(processus_array_t *array) {

  if (!array || array->size == 0) {
    return NULL;
  }

  size_t estimated_size = 512 * array->size;
  char *buffer = malloc(estimated_size);
  
  if (!buffer) return NULL; 

  char *cursor = buffer;
  size_t remaining_size = estimated_size; 

  for (size_t i = 0; i < array->size; i++) {
    
    int written = process_to_string(&array->data[i], cursor, remaining_size);
    
    if (written < 0 || (size_t)written >= remaining_size) { // verifier si la valeur du de la fonction n'est pas erroné 
        break; 
    }

    cursor += written;
    remaining_size -= written; 

  }
  return buffer;
}


int send_data(int socket_fd, processus_array_t *array) {

  char *buffer = serialize_process_list(array);
  if (!buffer) return -1;

  size_t size_package = strlen(buffer);
  
  int test1 = write(socket_fd,&size_package,sizeof(size_t));
  int test2 =write(socket_fd,buffer,size_package);

  free(buffer);
  if (test1 < 0 || test2 < 0) return -1;

  return 0;
}
