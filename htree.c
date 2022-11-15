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
#include <string.h>

#define BSIZE 4096

struct recursive_variables {   
  //uint32_t nblocks;

  uint16_t threads;
  uint64_t nblocks_each_thread;   
  uint16_t thread_current_count;
}; 

// Print out the usage of the program and exit.
void Usage(char*);
void *thread_testing(void* struct_data);
uint32_t jenkins_one_at_a_time_hash(uint8_t* , uint64_t );

int number_of_integers(uint64_t);


uint8_t *file_string;


int main(int argc, char** argv) {


  //////////////////////////////////////////////////////////
  int32_t fd;
  uint64_t nblocks;

  struct stat fileStat;
  uint64_t nblocks_each_thread = 0;
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
  fstat(fd, &fileStat);
  //Use mmap
  file_string = mmap(NULL, fileStat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);  

  // Get number of blocks, according to file size
  if (fileStat.st_size % BSIZE == 0){
    nblocks = fileStat.st_size/BSIZE;
  }
  else{
    nblocks = 1 + (fileStat.st_size/BSIZE);
  }



//////////////////////////////////////////////////////////////////////////
// Begin seperating nblocks to number of threads
  uint16_t threads = atoi(argv[2]);
  if(threads == 0 || nblocks < threads){
    perror("Number of threads are more than number of blocks...");
    exit(EXIT_FAILURE);
  }
  else if (nblocks % threads != 0){
  perror("Nblocks not evenly distributing to number of threads...");
  exit(EXIT_FAILURE);
  }


  nblocks_each_thread = nblocks/threads;
  printf("\nnblocks for each thread: %d \n", nblocks_each_thread);

  struct recursive_variables *temp_hold = malloc(sizeof(struct recursive_variables));
  temp_hold->threads = threads;
  temp_hold->nblocks_each_thread = nblocks_each_thread;
  temp_hold->thread_current_count = 0;


  pthread_t p1; 
  pthread_create(&p1, NULL, thread_testing, temp_hold);
  pthread_join(p1, NULL);  
  free(temp_hold); 

//////////////////////////////////////////////////////////////////////////








/*
  printf("\n\n\n");
  uint32_t x = 0;
  printf("\n%d",nblocks_each_thread);
  printf("\n%d",fileStat.st_size);
  printf("\n%d",nblocks_each_thread * 4096);
  
  for(int i = 0; i < nblocks_each_thread * 4096; i++){
    x+= 1;
  }
  printf("\nTOTAL BYTES IN MAIN %d\n", x);
  //printf("\n%d",fileStat.st_size);
  //x = jenkins_one_at_a_time_hash(file_string, fileStat.st_size);
  //printf("\n%lld\n",x);
  
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
















































void *thread_testing(void* struct_data){
  
  // Set and create "struct_data" and assign it to "*thread_node" to hold
  struct recursive_variables *thread_node = (struct recursive_variables*)struct_data;

  // Assing the current thread num, to "currrent_thread"
  uint16_t currrent_thread = thread_node->thread_current_count;
  
  // "nblocks_each_thread" will hold num of nblocks each thread must have
  uint64_t nblocks_each_thread = thread_node->nblocks_each_thread;
  
  // Variable will be use to send hash value pack,
  // 0 sent back if child threads come back 0.
  uint64_t parent_hash = 0;


  // Check if "currrent_thread" is still valid
  // If we reach our thread max, skip all this and 
  // return a "parent_hash", which is just zero
  if(currrent_thread < thread_node->threads){   


    ////////////////////////////////////////////////////////////////////////////////////////////////
    struct recursive_variables *temp1 = malloc(sizeof(struct recursive_variables));
      temp1->nblocks_each_thread = nblocks_each_thread;
      temp1->threads = thread_node->threads;
      temp1->thread_current_count = (2 * currrent_thread + 1);   

    struct recursive_variables *temp2 = malloc(sizeof(struct recursive_variables));
      temp2->nblocks_each_thread = nblocks_each_thread;;
      temp2->threads = thread_node->threads;
      temp2->thread_current_count = (2 * currrent_thread + 2);   


    // Create two variables to hold returning data from threads
    uint64_t left_child_hash;
    uint64_t right_child_hash;
    
    // Intialize the threads and pass in previous allocated struct variables
    pthread_t p1, p2;

    pthread_create(&p1, NULL, thread_testing, temp1); 
    pthread_create(&p2, NULL, thread_testing, temp2); 
    pthread_join(p1, (void *) &left_child_hash);  
    pthread_join(p2, (void *) &right_child_hash);  

    // Free the previous struct variables when coming back from pthreads
    free(temp1); 
    free(temp2); 

    ////////////////////////////////////////////////////////////////////////////////////////////////






  ////////////////////////////////////////////////////////////////////////////////////////////////

    // Intialize a counter variable for "holdcharacters[count_i]"
    // Since we need to store the first element to the end
    uint64_t count_i = 0;
    uint8_t *holdcharacters = (uint8_t *) malloc ((nblocks_each_thread * BSIZE) *sizeof(uint8_t));

    // Get beginning and ending index according to the current thread
    uint64_t beginning_index = (BSIZE * nblocks_each_thread * currrent_thread);
    uint64_t ending_index = (BSIZE * ((currrent_thread + 1) * nblocks_each_thread));

    for(uint64_t i = beginning_index; i < ending_index; i++){
      holdcharacters[count_i++] = file_string[i]; 
    }
    
    parent_hash = jenkins_one_at_a_time_hash(holdcharacters, (BSIZE * nblocks_each_thread));


    int check_child_not_zero = 0;

    if(left_child_hash == 0 && right_child_hash == 0){
      printf("\nConcatenated String: %lld", parent_hash);
      check_child_not_zero = 1;
      //return (void *) parent_hash;
    }
  ////////////////////////////////////////////////////////////////////////////////////////////////



  ////////////////////////////////////////////////////////////////////////////////////////////////

    if (check_child_not_zero != 1){ 

      int parent_integer = number_of_integers(parent_hash);
      int left_integer = number_of_integers(left_child_hash);
      int right_integer = number_of_integers(right_child_hash);
      int total_count = 0;

      char parent_string[parent_integer+1];
      char l_child_string[left_integer+1];
      char r_child_string[right_integer+1];

      sprintf(parent_string, "%lld", parent_hash);
      sprintf(l_child_string, "%lld", left_child_hash);
      sprintf(r_child_string, "%lld", right_child_hash);




      char first_concat[100];

      if(left_integer != 0 && right_integer != 0){
        printf("\n1 left_integer != 0 && right_integer != 0");
        strcat(first_concat,parent_string);
        strcat(first_concat,l_child_string);
        strcat(first_concat,r_child_string);
        total_count = parent_integer + left_integer + right_integer;
      }
      else{
        if(left_integer == 0 && right_integer != 0){
        printf("\n2 left_integer == 0 && right_integer != 0");
          strcat(first_concat,parent_string);
          strcat(first_concat,r_child_string);
          total_count = parent_integer + right_integer;
        }
        else if(left_integer != 0 && right_integer == 0){
        printf("\n3 left_integer != 0 && right_integer == 0");
          strcat(first_concat,parent_string);
          strcat(first_concat,l_child_string);
          total_count = parent_integer + left_integer;
        }
        else{
        printf("\n4 JUST PARENT");
          strcat(first_concat,parent_string);
          total_count = parent_integer;
        }
      }
      printf("\ncurrent Thread: %d", currrent_thread);
      printf("\nConcatenated String: %s, AND Conca. value: %d", first_concat, total_count);
      parent_hash = jenkins_one_at_a_time_hash(first_concat, total_count);
      printf("\nConcatenated String: %lld", parent_hash);
      printf("\n\n");
      free(holdcharacters);

    } 
  ////////////////////////////////////////////////////////////////////////////////////////////////







  }

  return (void *) parent_hash;
}
//////////////////////////////////////////////////////////////////////













int number_of_integers(uint64_t integer){
    int number_of_integers = 0;
    uint64_t val = integer;
    while(val > 0){      
      val /= 10;
      number_of_integers++;
    }

  return number_of_integers;
}

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