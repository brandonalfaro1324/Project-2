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
  uint16_t threads;
  uint32_t total_nblocks_for_each_thread;   
  uint16_t thread_limit_before_extra_nblocks;      
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
  
  total_nblocks_for_each_thread = nblocks/threads;
  add_extra_nblock_thread = nblocks%threads;


  struct recursive_variables *temp_hold = malloc(sizeof(struct recursive_variables));
  

  
  temp_hold->whole_file = *file_string;
  temp_hold->file_data = fileStat;
  temp_hold->threads = threads;
  temp_hold->total_nblocks_for_each_thread = total_nblocks_for_each_thread;
  temp_hold->thread_limit_before_extra_nblocks = (threads - add_extra_nblock_thread);
  temp_hold->thread_current_count = 0;


  pthread_t p1; 
  pthread_create(&p1, NULL, thread_testing, temp_hold);
  //free(temp_hold); 
  pthread_join(p1, NULL);  
//////////////////////////////////////////////////////////////////////////


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
  
  struct recursive_variables *thread_node = (struct recursive_variables*)struct_data;
  int currrent_thread = thread_node->thread_current_count;

  if(currrent_thread < thread_node->threads){
    if(currrent_thread < thread_node->thread_limit_before_extra_nblocks){
      printf("\n[%d : %d]", currrent_thread, thread_node->total_nblocks_for_each_thread);
    }
    else{
      printf("\n[%d : %d]", currrent_thread, thread_node->total_nblocks_for_each_thread + 1);
    }
  
    struct recursive_variables *temp1 = malloc(sizeof(struct recursive_variables));
      temp1->whole_file = thread_node->whole_file;
      temp1->file_data = thread_node->file_data;
      temp1->total_nblocks_for_each_thread = thread_node->total_nblocks_for_each_thread;
      temp1->thread_limit_before_extra_nblocks = (thread_node->thread_limit_before_extra_nblocks);
      temp1->threads = thread_node->threads;
      temp1->thread_current_count = (2 * thread_node->thread_current_count + 1);   


    struct recursive_variables *temp2 = malloc(sizeof(struct recursive_variables));
      temp2->whole_file = thread_node->whole_file;
      temp2->file_data = thread_node->file_data;
      temp2->total_nblocks_for_each_thread = thread_node->total_nblocks_for_each_thread;
      temp2->thread_limit_before_extra_nblocks = (thread_node->thread_limit_before_extra_nblocks);
      temp2->threads = thread_node->threads;
      temp2->thread_current_count = (2 * thread_node->thread_current_count + 2);   


    pthread_t p1, p2;

    pthread_create(&p1, NULL, thread_testing, temp1); 
    pthread_create(&p2, NULL, thread_testing, temp2); 

    pthread_join(p1, NULL);  
    pthread_join(p2, NULL);  
  
  }
  
  return NULL;
}
//////////////////////////////////////////////////////////////////////














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
