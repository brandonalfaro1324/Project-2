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

#define BSIZE 4096

struct nblock_and_thread {   
  uint32_t nblocks;          
  uint32_t nblocks_each_thread;   
  uint8_t threads_block_extra;          
  uint8_t threads;          
}; 

struct recursive_variables {   
  //uint32_t nblocks;          
  uint32_t nblocks_for_each_thread;   
  uint8_t threads_with_1more;          
  uint8_t threads;    
  uint8_t thread_id;      
}; 


// Print out the usage of the program and exit.
void Usage(char*);
void thread_testing(uint8_t *, struct recursive_variables);



uint32_t jenkins_one_at_a_time_hash(uint8_t* , uint64_t );

int main(int argc, char** argv) {
  //////////////////////////////////////////////////////////

  int32_t fd;
  uint32_t nblocks;

  uint8_t *file_string;
  uint64_t x = 0;

  uint32_t nblocks_each_thread = 0;
  uint16_t threads_block_extra = 0;

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
  uint8_t threads = atoi(argv[2]);
  if(nblocks < threads ||  threads <= 0){
    perror("Number of threads are more than number of blocks...");
    exit(EXIT_FAILURE);
  }
  
  /* IGNORE THIS CODE, THIS CE IS TO SEE IF "nblock/threads" IS WORKING!!!
  nblocks_each_thread = nblocks/threads;
  threads_block_extra = nblocks%threads;
  
  uint32_t block_visualization[threads];

  printf("\nFile Size: %d\n", fileStat.st_size);    

  printf("\nNblocks:  %d", nblocks);
  printf("\nThreads needed:  %d", threads);
  printf("\nNblocks per thread %d", nblocks_each_thread);    
  printf("\nThreads that need 1 extra nblock %d\n", threads_block_extra);   


  for(int i = 0; i < threads; i++){
    block_visualization[i] = nblocks_each_thread;

      if(i > (threads - threads_block_extra - 1)){
        block_visualization[i] = nblocks_each_thread + 1;
      }
  }
  for(int i = 0; i < threads; i++){
    printf("[%d]", block_visualization[i]);
  }
  */
//////////////////////////////////////////////////////////////////////////




  nblocks_each_thread = nblocks/threads;
  threads_block_extra = nblocks%threads;
  struct recursive_variables temp_struct_hold = {nblocks_each_thread, 
                                                          threads - threads_block_extra, 
                                                          threads, 
                                                          0};  

  
  thread_testing(file_string, temp_struct_hold);



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
void thread_testing(uint8_t *file_data, struct recursive_variables struct_data){
  

  //int thread_minus_one = struct_data.threads - struct_data.threads_with_1more - 1;

  int one_or_two = 0;
  int currrent_thread = struct_data.thread_id;

  if(currrent_thread < struct_data.threads){
    //printf("\n[current thread=%d : thread=%d]\n", currrent_thread, struct_data.threads);
    //printf("PASS\n");
    //printf("\nThread number: %d", currrent_thread);
    if(currrent_thread < struct_data.threads_with_1more){
      printf("\n[%d : %d]", currrent_thread, struct_data.nblocks_for_each_thread);
      //printf("\n NO EXTRA: [%d]\n", currrent_thread);

    }
    else{
      printf("\n[%d : %d]", currrent_thread, struct_data.nblocks_for_each_thread + 1);
      //printf("\n Threads that need one extra: [%d]\n", currrent_thread);
    }

      struct_data.thread_id = 2 * currrent_thread + 1;
      thread_testing(file_data, struct_data);

      struct_data.thread_id = 2 * currrent_thread + 2;
      thread_testing(file_data, struct_data);
  
  }  


}
//////////////////////////////////////////////////////////////////////

/*
struct recursive_variables {   
  //uint32_t nblocks;          
  uint32_t nblocks_for_each_thread;   
  uint16_t threads_with_1more;          
  uint16_t threads;    
  uint16_t thread_id;      
}; 
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
