#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/* Number of words that caused out-of-memory case: 16777216 run from cloud9*/
/*memory erroe test by valgrind */

int main(int argc, char* argv[])
{
    /*determine the input value*/
    
    if(argc != 3){
        fprintf(stderr,"ERROR: INVAILD INPUT\n");
        return EXIT_FAILURE;
    }
    
    FILE *hw_file;
    hw_file = fopen(argv[1], "r");
    
    /*words_size is name for word count, will count how many word it has*/
    int words_size = 0;
    int array_size = 8;
    
    char* *array = (char* *)calloc(array_size, sizeof( char* ) );
    printf("Allocated initial array of %d character pointers.\n", array_size);
    
    /*word lenth*/
    int a_word_size = 0;
    
    /*my algorithem is to read in each char, and stop for any symbol 
    that is not number or letter. */
    while(1)
    {
        int min = 8;
        
        if(feof(hw_file))
        {
            break;
        }
        
        if (words_size == array_size-1)
        {
            array_size = array_size * 2;
            array = (char* *)realloc( array, array_size * sizeof( char*) );
            printf("Re-allocated array of %d character pointers.\n",array_size );
        }
        
        int c = fgetc(hw_file);
        
        char * str =  (char *)calloc(min, sizeof( char ) );
        
        /* read each word*/
        while( isalnum(c) != 0 )
        {   
            str[a_word_size] = c;
            a_word_size++;
            c = fgetc(hw_file);
            if (a_word_size >= (min-1) )
            {
                min = min*2;
                str = (char *)realloc(str, min * sizeof( char ) );
            }
            /*
            There will be error without str[a_word_size+1] = '\0'
            when the word lenth is = 8
            Conditional jump or move depends on uninitialised value(s)
            at 0x4C31FCE: strstr (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
            by 0x400AC1: main (Homework1.c:89)
            Uninitialised value was created by a heap allocation
            at 0x4C2CE8E: realloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
            by 0x400A2D: main (Homework1.c:57)
            */
            str[a_word_size+1] = '\0';
        }
        
        /* put the word in to array*/
       if(a_word_size!= 0)
        {
            array[words_size] = str;
                
            a_word_size = 0;
            //free(str);
            words_size++;
        }
        else
        {
            /*free the empty word*/
            free(str);
        }
        
    }
    
    printf("All done (successfully read %d words).\n", words_size);
    /* To find words contain keyterm
        Using strstr() in here
    */
    
    printf("Words containing substring \"%s\" are:\n", argv[2]);
    int i;
    
    int count;
    
    for(i = 0; i <words_size; i++ )
    {
        
        if(strstr(array[i], argv[2] ) != NULL)
        {
            printf("%s\n", array[i]);
            count++;
        }
    }
    
    //printf("%d words contain '%s'\n", count, argv[2]);
    
    
    /* free all */
    
    for(i = 0; i <words_size; i++ )
    {
        free(array[i]);
    }
    
    free(array);
    
    fclose(hw_file);
    
    return EXIT_SUCCESS;
}