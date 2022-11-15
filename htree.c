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

  struct stat file_data;          
  uint16_t threads;
  uint32_t nblocks_each_thread;   
  uint16_t thread_current_count;
}; 

// Print out the usage of the program and exit.
void Usage(char*);
void *thread_testing(void* struct_data);
uint32_t jenkins_one_at_a_time_hash(uint8_t* , uint64_t );

int number_of_integers(uint64_t);


uint8_t *file_string;
struct stat fileStat;


int main(int argc, char** argv) {


  //////////////////////////////////////////////////////////
  int32_t fd;
  uint32_t nblocks;

  uint32_t nblocks_each_thread = 0;
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
  temp_hold->file_data = fileStat;
  temp_hold->threads = threads;
  temp_hold->nblocks_each_thread = nblocks_each_thread;
  temp_hold->thread_current_count = 0;


  pthread_t p1; 
  pthread_create(&p1, NULL, thread_testing, temp_hold);
  pthread_join(p1, NULL);  
  free(temp_hold); 

//////////////////////////////////////////////////////////////////////////







  /*  TEST, THIS WORKS AND PRODUCES THE CORRECT OUTPUT FOR 1 THREAD!!!!*/
  //for(int i = (fileStat.st_size - 1); i > (fileStat.st_size) - 100; i--){
  //  printf("%c",file_string[i]);
  //}


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
  struct recursive_variables *thread_node = (struct recursive_variables*)struct_data;
  int currrent_thread = thread_node->thread_current_count;

  uint64_t parent_value = 0;
  if(currrent_thread < thread_node->threads){

    struct recursive_variables *temp1 = malloc(sizeof(struct recursive_variables));
      temp1->file_data = thread_node->file_data;
      temp1->nblocks_each_thread = thread_node->nblocks_each_thread;
      temp1->threads = thread_node->threads;
      temp1->thread_current_count = (2 * thread_node->thread_current_count + 1);   

    struct recursive_variables *temp2 = malloc(sizeof(struct recursive_variables));
      temp2->file_data = thread_node->file_data;
      temp2->nblocks_each_thread = thread_node->nblocks_each_thread;
      temp2->threads = thread_node->threads;
      temp2->thread_current_count = (2 * thread_node->thread_current_count + 2);   


    uint64_t left_child;
    uint64_t right_child;
    
    pthread_t p1, p2;

    pthread_create(&p1, NULL, thread_testing, temp1); 
    pthread_join(p1, (void *) &left_child);  
    pthread_create(&p2, NULL, thread_testing, temp2); 
    pthread_join(p2, (void *) &right_child);  
    

    
    uint64_t beginning = (BSIZE * thread_node->nblocks_each_thread * thread_node->thread_current_count);
    uint64_t ending = (BSIZE * ((thread_node->thread_current_count + 1) * thread_node->nblocks_each_thread));

    //printf("\n%d < %d", beginning, ending);
    //printf("\n%d\n", thread_node->nblocks_each_thread * BSIZE);

    uint64_t x = 0;
    uint8_t *holdcharacters = (uint8_t *) malloc ((thread_node->nblocks_each_thread * BSIZE) *sizeof(uint8_t));


    //printf("\nTEST1\n");
    //printf("\nStarting point: %d, Ending point: %d", beginning/BSIZE, ending/BSIZE);
    //printf("\nStarting point CHAR: %c, Ending point CHAR: %c\n", file_string[beginning], file_string[ending - 1]);
    
  
    uint64_t count_i = 0;
    for(int i = beginning; i < ending; i++){
      holdcharacters[count_i++] = file_string[i];
    }


    //printf("\nNode %d: count: %d\n", currrent_thread, x / BSIZE);
    //printf("\nNODE: %d Beginning and end index [%d : %d]", thread_node->thread_current_count, beginning, ending);
    //printf("\n%d\n", thread_node->nblocks_each_thread * BSIZE);

    


    parent_value = jenkins_one_at_a_time_hash(holdcharacters, (BSIZE * thread_node->nblocks_each_thread));

    int parent_integer = number_of_integers(parent_value);
    int left_integer = number_of_integers(left_child);
    int right_integer = number_of_integers(right_child);

    printf("\n\nCURRENT THREAD: %d\n", currrent_thread);
    printf("\nPARENT VALUE: %lld, AND INTEGER: %d", parent_value, parent_integer);
    printf("\nLEFT CHILD VALIE: %lld, AND INTEGER: %d", left_child, left_integer);
    printf("\nLEFT CHILD VALIE: %lld, AND INTEGER: %d", right_child, right_integer);

    char parent_string[parent_integer+1];
    char l_child_string[left_integer+1];
    char r_child_string[right_integer+1];

    sprintf(parent_string, "%lld", parent_value);
    sprintf(l_child_string, "%lld", left_child);
    sprintf(r_child_string, "%lld", right_child);
    printf("\nPARENT: %s, LEFT CHILD: %s, RIGHT CHILD: %s", parent_string, l_child_string, r_child_string);
    printf("\n\n");
  






    free(holdcharacters);
    free(temp1); 
    free(temp2); 
  }

  //printf("\nreturning value: %lld\n", y);
  return (void *) parent_value;
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
