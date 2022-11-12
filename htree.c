#include <stdio.h>     
#include <stdlib.h>   
#include <stdint.h>  
#include <inttypes.h>  
#include <errno.h>     // for EINTR
#include <fcntl.h>     
#include <unistd.h>    
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h>

#define BSIZE 4096

struct recursive_variables {   
  //uint32_t nblocks;
  uint8_t whole_file;
  struct stat file_data;          
  uint32_t total_nblocks_for_each_thread;   
  uint16_t thread_limit_before_extra_nblocks;      
  uint16_t threads;
  uint16_t thread_current_count;      
}; 

// Print out the usage of the program and exit.
void Usage(char*);
void *thread_testing(/*uint8_t *,*/ void* struct_data);

uint32_t jenkins_one_at_a_time_hash(uint8_t* , uint64_t );




int main(int argc, char** argv) {
  //////////////////////////////////////////////////////////

  int32_t fd;
  uint32_t nblocks;

  uint8_t *file_string;
  //uint64_t x = 0;

  uint32_t total_nblocks_for_each_thread = 0;
  uint16_t add_extra_nblock_thread = 0;

  //////////////////////////////////////////////////////////

  // input checking 
  if (argc != 3)
    Usage(argv[0]);

  // open input files
  fd = open(argv[1], O_RDWR);
  if (fd == -1) {
    perror("open failed");
    exit(EXIT_FAILURE);
  }
  // use fstat to get file size
  // calculate nblocks 

  //Use fset
  struct stat fileStat;
  fstat(fd, &fileStat);
  //Use mmap
  file_string = mmap(NULL, fileStat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);  

  // Get number of blocks, according to file size
  if (fileStat.st_size % BSIZE == 0){
    nblocks = fileStat.st_size/BSIZE;
  }
  else if(fileStat.st_size % BSIZE == fileStat.st_size){
    nblocks = 1;
  }
  else{
    nblocks = 1 + (fileStat.st_size/BSIZE);
  }


//////////////////////////////////////////////////////////////////////////
// Begin seperating nblocks to number of threads
  uint16_t threads = atoi(argv[2]);
  if(nblocks < threads ||  threads <= 0){
    perror("Number of threads are more than number of blocks...");
    exit(EXIT_FAILURE);
  }
  
  /* IGNORE THIS CODE, THIS CE IS TO SEE IF "nblock/threads" IS WORKING!!!
  total_nblocks_for_each_thread = nblocks/threads;
  add_extra_nblock_thread = nblocks%threads;
  
  uint32_t block_visualization[threads];

  printf("\nFile Size: %d\n", fileStat.st_size);    

  printf("\nNblocks:  %d", nblocks);
  printf("\nThreads needed:  %d", threads);
  printf("\nNblocks per thread %d", total_nblocks_for_each_thread);    
  printf("\nThreads that need 1 extra nblock %d\n", add_extra_nblock_thread);   


  for(int i = 0; i < threads; i++){
    block_visualization[i] = total_nblocks_for_each_thread;

      if(i > (threads - add_extra_nblock_thread - 1)){
        block_visualization[i] = total_nblocks_for_each_thread + 1;
      }
  }
  for(int i = 0; i < threads; i++){
    printf("[%d]", block_visualization[i]);
  }
  */
//////////////////////////////////////////////////////////////////////////


  total_nblocks_for_each_thread = nblocks/threads;
  add_extra_nblock_thread = nblocks%threads;

  struct recursive_variables temp_hold = {
  *file_string,
  fileStat,
  total_nblocks_for_each_thread, 
  (threads - add_extra_nblock_thread), 
  threads, 
  0};  

  printf("\n%p\n", &temp_hold);

  pthread_t p1; 

  pthread_create(&p1, NULL, thread_testing, (void *) &temp_hold); 
  pthread_join(p1, NULL);  



  /*  TEST, THIS WORKS AND PRODUCES THE CORRECT OUTPUT FOR 1 THREAD!!!!
  printf("\n%lld\n",fileStat.st_size);
  x = jenkins_one_at_a_time_hash(file_string, fileStat.st_size);
  printf("\n%lld\n",x);
  */
  /*
  printf(" no. of blocks = %u \n", nblocks);
  double start = GetTime();
  // calculate hash value of the input file
  double end = GetTime();
  printf("hash value = %u \n", hash);
  printf("time taken = %f \n", (end - start));
  */
/* Ignore this
  if(1 < 0){
    printf("%d",nblocks);
    printf("%hhn",file_string);
  }
*/

  close(fd);
  return EXIT_SUCCESS;
}

//////////////////////////////////////////////////////////////////////
void *thread_testing(/*uint8_t *file_data,*/ void* struct_data){
  
  struct recursive_variables *temp = malloc(sizeof(struct recursive_variables));
  temp = struct_data;
  int currrent_thread = temp->thread_current_count;

  if(currrent_thread < temp->threads){
    if(currrent_thread < temp->thread_limit_before_extra_nblocks){
      printf("\n[%d : %d]", currrent_thread, temp->total_nblocks_for_each_thread);
    }
    else{
      printf("\n[%d : %d]", currrent_thread, temp->total_nblocks_for_each_thread + 1);
    }

    struct recursive_variables temp1 = {
      temp->whole_file,
      temp->file_data,          
      temp->total_nblocks_for_each_thread,   
      temp->thread_limit_before_extra_nblocks,      
      temp->threads,
      (2 * temp->thread_current_count + 1)   
    };    


    struct recursive_variables temp2 = {
      temp->whole_file,
      temp->file_data,          
      temp->total_nblocks_for_each_thread,   
      temp->thread_limit_before_extra_nblocks,      
      temp->threads,
      (2 * temp->thread_current_count + 2)   
    };    

    pthread_t p1, p2;
    pthread_create(&p1, NULL, thread_testing, (void *) &temp1); 
    pthread_create(&p2, NULL, thread_testing, (void *) &temp2); 

    pthread_join(p1, NULL);  
    pthread_join(p2, NULL);  
  }
  return NULL;
}


//////////////////////////////////////////////////////////////////////




/*
void thread_testing(uint8_t *file_data, struct recursive_variables struct_data){
  
  int one_or_two = 0;
  int currrent_thread = struct_data.thread_current_count;

  if(currrent_thread < struct_data.threads){
    //printf("\n[current thread=%d : thread=%d]\n", currrent_thread, struct_data.threads);
    //printf("PASS\n");
    //printf("\nThread number: %d", currrent_thread);
    if(currrent_thread < struct_data.threads){
      printf("\n[%d : %d]", currrent_thread, struct_data.total_nblocks_for_each_thread);
    }
    else{
      printf("\n[%d : %d]", currrent_thread, struct_data.total_nblocks_for_each_thread + 1);
    }

      struct_data.thread_current_count = 2 * currrent_thread + 1;
      thread_testing(file_data, struct_data);

      struct_data.thread_current_count = 2 * currrent_thread + 2;
      thread_testing(file_data, struct_data);  
    }
}
  */










uint32_t jenkins_one_at_a_time_hash(uint8_t* key, uint64_t length) {
  uint64_t i = 0;
  uint32_t hash = 0;

  while (i != length) {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}


void Usage(char* s) 
{
  fprintf(stderr, "Usage: %s filename num_threads \n", s);
  exit(EXIT_FAILURE);
}
