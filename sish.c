#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/*
#include <sys/wait.h>
*/


int main(int argc, char *argv[]){

////////////////////////////////////////
    // Variables for "getline()"
    char *string;
    size_t bufsize = 32;
    size_t character;
////////////////////////////////////////
    // Parsing
    char *pillar;
    char *pillarPars;
    char *spacePars;
    char *clean_string;

    int8_t total_number_of_char = 0;

////////////////////////////////////////



// GETLINE ////////////////////////////////////////
    string = (char *)malloc(bufsize * sizeof(char));
    if( string == NULL)
    {
        perror("Unable to allocate string");
        exit(1);
    }

    printf("sish> ");
    character = getline(&string,&bufsize,stdin); 
    string[character-1] = '\0';
        
// TOKENIZING PILLAR ////////////////////////////////////////


    for (pillar = strtok_r(string, "|", &pillarPars); 
    pillar; 
    pillar = strtok_r(NULL, "|", &pillarPars)){
    
        char *command = NULL;
        char *com_argument= NULL;
        int i = 0;

        for (clean_string = strtok_r(pillar, " ", &spacePars); 
        clean_string; 
        clean_string = strtok_r(NULL, " ", &spacePars)){
                        
            if(i == 0){
                command = clean_string;
            }
            else{
                com_argument = clean_string;
            }      
            i++;

        }
    
        // printf("%s\n",command);
        // printf("%s\n",com_argument);
        // printf("\n\n");



        char* myargs[] = {command, argv[1], NULL}; 
        execvp(myargs[0], myargs);
        //execvp(myargs[1], myargs);
  

    }