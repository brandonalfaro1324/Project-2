#include <stdio.h>     
#include <stdlib.h>   
#include <stdint.h>  
#include <inttypes.h>  
#include <errno.h>
#include <fcntl.h>     
#include <unistd.h>    
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>

#include "common.h"

#define BSIZE 4096

struct recursive_variables {   
  //uint32_t nblocks;

  uint64_t nblocks_each_thread;   
  uint16_t threads;
  uint16_t thread_current_count;
}; 

// Print out the usage of the program and exit.
void Usage(char*);
void *binary_threads(void* struct_data);
uint32_t jenkins_one_at_a_time_hash(uint8_t* , uint64_t );

int number_of_integers(uint32_t);

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
  printf("\nnblocks for each thread: %llu \n\n\n", nblocks_each_thread);

  struct recursive_variables *temp_hold = malloc(sizeof(struct recursive_variables));
  temp_hold->threads = threads;
  temp_hold->nblocks_each_thread = nblocks_each_thread;
  temp_hold->thread_current_count = 0;
  



////////////////////////////////////////////////////////////////////////////////////////////////////

  uint32_t *main_parent_hash = NULL;
  

  printf(" no. of blocks = %llu \n\n\n\n", nblocks);
  double start = GetTime();


  pthread_t p1; 
  pthread_create(&p1, NULL, binary_threads, temp_hold);
  pthread_join(p1, (void *) &main_parent_hash);  



////////////////////////////////////////////////////////////////////////////////////////////////////

  free(temp_hold); 

  // calculate hash value of the input file
  double end = GetTime();
  printf("\n\n\nhash value = %u \n", *main_parent_hash);
  printf("time taken = %f \n", (end - start));
  
  close(fd);
  return EXIT_SUCCESS;
}





void *binary_threads(void* struct_data){  

  // Set and create "struct_data" and assign it to "*thread_node" to hold
  struct recursive_variables *thread_node = (struct recursive_variables*)struct_data;
  // Assing the current thread num, to "currrent_thread"
  uint16_t currrent_thread = thread_node->thread_current_count;
  // "nblocks_each_thread" will hold num of nblocks each thread must have
  uint64_t nblocks_each_thread = thread_node->nblocks_each_thread;
 
 

////////////////////////////////////////////////////////////////////////////////////////////////////

  // Variable will be use to send hash value pack,
  // 0 sent back if child threads come back 0.
  uint32_t parent_hash = 0;
  uint32_t *returnig_hash_value = malloc(sizeof(uint32_t *));





////////////////////////////////////////////////////////////////////////////////////////////////////



  // Check if "currrent_thread" is still valid
  // If we reach our thread max, skip all this and 
  // return a "parent_hash", which is just zero
  if(currrent_thread < thread_node->threads){  

    // Allocate 2 "struct recursive_variables" variables to pass to "pthread_create"
    struct recursive_variables *temp1 = malloc(sizeof(struct recursive_variables));
      temp1->nblocks_each_thread = nblocks_each_thread;
      temp1->threads = thread_node->threads;
      temp1->thread_current_count = (2 * currrent_thread + 1);   

    struct recursive_variables *temp2 = malloc(sizeof(struct recursive_variables));
      temp2->nblocks_each_thread = nblocks_each_thread;;
      temp2->threads = thread_node->threads;
      temp2->thread_current_count = (2 * currrent_thread + 2);   


////////////////////////////////////////////////////////////////////////////////////////////////////





    // Create two variables to hold returning data from threads
    uint32_t right_child_hash = 0;
    uint32_t left_child_hash = 0;
    
    uint32_t *left_value = NULL;
    uint32_t *right_value = NULL;




////////////////////////////////////////////////////////////////////////////////////////////////////

    // Intialize the threads and pass in previous allocated struct variables
    pthread_t p1, p2;

////////////////////////////////////////////////////////////////////////////////////////////////////

      
      if(currrent_thread+1 < thread_node->threads){
        pthread_create(&p1, NULL, binary_threads, temp1); 
        pthread_create(&p2, NULL, binary_threads, temp2);
      }

      
 







    // Intialize a counter variable for "holdcharacters[count_i]"
    // Since we need to store from first element to the last element
    uint64_t count_i = 0;

    // Allocate "holdcharacters" to hold parts of the main file
    uint8_t *holdcharacters = (uint8_t *) malloc ((nblocks_each_thread * BSIZE) *sizeof(uint8_t));

    // Get beginning and ending index according to the current thread
    uint64_t beginning_index = (BSIZE * nblocks_each_thread * currrent_thread);
    uint64_t ending_index = (BSIZE * ((currrent_thread + 1) * nblocks_each_thread));

    // Assign every 
    for(uint64_t i = beginning_index; i < ending_index; i++){
      holdcharacters[count_i++] = file_string[i]; 
    }
    
    // Begin the jenkins function
    parent_hash = jenkins_one_at_a_time_hash(holdcharacters, (BSIZE * nblocks_each_thread));

    free(holdcharacters);


    if(currrent_thread+1 < thread_node->threads){
      pthread_join(p1, (void *) &left_value);  
      pthread_join(p2, (void *) &right_value);  

      left_child_hash = *left_value;
      right_child_hash = *right_value;
    }

    free(left_value);
    free(right_value);

    
////////////////////////////////////////////////////////////////////////////////////////////////////




    // Free the previous struct variables when coming back from pthreads
    free(temp1); 
    free(temp2); 



    int check_child_not_zero = 0;

    // If both childs return 0, everything below
    // is skipped, we only return the parents hash
    if(left_child_hash == 0 && right_child_hash == 0){
      check_child_not_zero = 1;
    }

    // Using "check_child_not_zero"
    // we check that, if both childs are 0
    // we skipp everything below
    if (check_child_not_zero == 0){ 

      // We count and set how many indexes each integer has
      int parent_integer = number_of_integers(parent_hash);
      int left_integer = number_of_integers(left_child_hash);
      int right_integer = number_of_integers(right_child_hash);

      // Use this to count how many total 
      // indexes for parent and childs combined
      int total_count = 0;

      // Assing char variable for each child and parent,
      // this is needed to convert integers to strings
      // and concat strings.

      char parent_string[parent_integer+1];
      char l_child_string[left_integer+1];
      char r_child_string[right_integer+1];

      // Set integers to string
      sprintf(parent_string, "%u", parent_hash);
      sprintf(l_child_string, "%u", left_child_hash);
      sprintf(r_child_string, "%u", right_child_hash);

      // This is going to be use to add every string together
      uint8_t *first_concat = (uint8_t *) malloc ((60) *sizeof(uint8_t));

      // If both childs are not 0, then we concat everything
      if(left_integer != 0 && right_integer != 0){
        
        total_count = parent_integer + left_integer + right_integer;
        
        strcat((char *) first_concat, parent_string);
        strcat((char *) first_concat, l_child_string);
        strcat((char *) first_concat, r_child_string);
      
      }

      // Go here if one of the childs is 0
      else{

        // If left is 0, concat parent and right child,
        // and vice versa for the block in the bottom
        if(left_integer == 0 && right_integer != 0){
  
          //printf("\nGOING 2...");
          total_count = parent_integer + right_integer;
          strcat((char *) first_concat, parent_string);
          strcat((char *) first_concat, r_child_string);

        }
        if(left_integer != 0 && right_integer == 0){
          total_count = parent_integer + left_integer;

          //printf("\nGOING 3...");

          strcat((char *) first_concat, parent_string);
          strcat((char *) first_concat, l_child_string);

        }
      }      
      //printf("\n%s", first_concat);
      // Use Jenkins!!! and get the hash from parent and childs
      parent_hash = jenkins_one_at_a_time_hash(first_concat, total_count);

      // Free the allocated char "holdcharacters"
      free(first_concat);
    } 
  }

  // Return 0, if we reach our max thread,
  // or hash value

////////////////////////////////////////////////////////////////////////////////////////////////////




  printf("\n%u\n", parent_hash);
  printf("\n%u\n", *returnig_hash_value);

  *returnig_hash_value = parent_hash;
  pthread_exit((void *) (uintptr_t) returnig_hash_value);



////////////////////////////////////////////////////////////////////////////////////////////////////

}



int number_of_integers(uint32_t integer){
    int number_of_integers = 0;
    uint32_t val = integer;
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

void Usage(char* s) {
  fprintf(stderr, "Usage: %s filename num_threads \n", s);
  exit(EXIT_FAILURE);
}