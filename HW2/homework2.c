#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

void lineread(char *expr, int *fd);
void lineread2(char *expr, int *fd, int *fp, int *rp, char * operationp);
int braketread(char *expr, int pos);
int braketexpr(char *expr, int i, char* operation);
int braketexpr2(char *expr, int i, int *fd, int *fp, int *rp, char * operationp);
int readdigit(char *expr, int s, int *fd, int *f, int *r, char * operation);
int nmberexpr(char *expr, int i, int end);
void calcuate(int *r, int number, char *operation);

int main( int argc, char * argv[] )
{
    if(argc != 2)
    {
        fprintf(stderr, "ERROR: Invalid arguments\nUSAGE: ./a.out <input-file>\n");
        return EXIT_FAILURE;
    }
    FILE *input_file;
    input_file = fopen(argv[1], "r");
    if (input_file == NULL)
    {
        exit(EXIT_FAILURE);
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    
    while ((read = getline(&line, &len, input_file)) != -1) {
        if(line[0]=='('){
            int fd[2];
            int recive = pipe(fd);
            lineread(line, fd);
        }
    }
    
    free(line);
    fclose(input_file);
    exit(EXIT_SUCCESS);
}

void lineread(char *expr, int *fd){
    int i;
    int first = 0;
    int *f = &first;
    int result;
    int *r = &result;
    char operation[1];
    for (i = 0; expr[i]!='\0'; i++){
        if(expr[i]=='(' && i == 0){
            braketexpr(expr, i, operation);
            i++;
        }
        if(expr[i]=='('&& i!= 0){
            i = braketexpr2(expr, i, fd, f , r, operation);
        }
        if(isdigit(expr[i])){
            i = readdigit(expr, i, fd, f , r, operation);
        }
    }
    printf("%d\n", result);
    int endp =  braketread(expr, 0);
    char expression[endp+2];
    memmove(expression, &expr[0], (size_t)(endp+1));
    expression[endp+1] = '\0';
    printf("PID %d: Processed \"%s\"; final answer is %d\n", getpid(), expression, result );
}

int braketread(char *expr, int pos){
    int i;
    int count = 0;
    int endpos;
    for(i = pos; expr[i]!='\0'; i++){
        if(expr[i] == '('){
            count++;
        }
        else if(expr[i]==')'){
            count--;
            if(count == 0){
                endpos = i;
                break;
            }
        }
    }
    return endpos;
}

void send(int * fd, int n){
    close(fd[0]);
    fd[0] = -1;
    write(fd[1], &n, sizeof(int));
    printf("PID %d: Sending \"%d\" on pipe to parent\n", getpid(), n);
}

int braketexpr(char *expr, int i, char* operation){
    int endp =  braketread(expr, i);
    char expression[endp-i+2];
    memmove(expression, &expr[i], (size_t)(endp-i+1));
    expression[endp-i+1] = '\0';
    printf("PID %d: My expression is \"%s\"\n", getpid(), expression);
    i++;
    if(expr[i]=='*'||expr[i]=='+'||expr[i]=='/'||expr[i]=='-')
    {
         printf("PID %d: Starting \"%c\" operation\n", getpid(), expr[i]);
         operation[0] = expr[i];
    }
    else
    {
        printf("error");
        exit(EXIT_FAILURE);
    }
    return endp;
}

int braketexpr2(char *expr, int i, int *fd, int *fp, int *rp, char * operationp){
    int endp =  braketread(expr, i);
    pid_t pid = fork();
    char expression[endp-i+2];
    if ( pid == -1 )
    {               
        perror( "fork() failed" );
        exit(EXIT_FAILURE);
    }
    if(pid ==0){
        memmove(expression, &expr[i], (size_t)(endp-i+1));
        expression[endp-i+1] = '\0';
        //lineread(expression,fd);
        printf("PID %d: My expression is \"%s\"\n", getpid(), expression);
        i++;
        if(expr[i]=='*'||expr[i]=='+'||expr[i]=='/'||expr[i]=='-')
        {
             printf("PID %d: Starting \"%c\" operation\n", getpid(), expr[i]);
        }
        else
        {
            printf("error");
            exit(EXIT_FAILURE);
        }
        lineread2(expression, fd, f, r, operation);
        exit(0);
    }
    else {
        int status;
        pid_t child_pid = wait(&status);
        //lineread(expression, fd);
        printf("%d\n", getpid);
    }
    return endp;
}

int readdigit(char *expr, int s, int *fd, int *f, int *r, char * operation){
    int end;
    int i;
    for (i = s; expr[i]!='\0'; i++){
        if(!isdigit(expr[i])){
            end = i;
            break;
        }
    }
    pid_t pid = fork();
    if ( pid == -1 )
    {               
        perror( "fork() failed" );
        exit(EXIT_FAILURE);
    }
    if(pid ==0){
        int n = nmberexpr(expr, s, end);
        send(fd,  n);
        exit(0);
    }
    else {
        int status;
        pid_t child_pid = wait(&status);
        int number;
        read(fd[0], &number, sizeof(int));
        printf("%d\n", number);
        if ((*f) == 0){
            (*f)++;
            *r = number;
        }
        else{
            calcuate(r, number, operation);
        }
    }
    return end;
}

int nmberexpr(char *expr, int i, int end){
    char expression[end-i];
    memmove(expression, &expr[i], (size_t)(end-i));
    expression[end-i+1] = '\0';
    int n = atoi(expression);
    printf("PID %d: My expression is \"%d\"\n", getpid(), n);
    return n;
}

void calcuate(int *r, int number, char *operation){
    if (operation[0] == '+'){
        (*r) = (*r)+number;
    }
    else if (operation[0] == '-'){
        (*r) = (*r)-number;
    }
    else if (operation[0] == '*'){
        (*r) = (*r)*number;
    }
    else if(operation[0] == '/'){
        if (number == 0){
            
        }
        else{
            (*r) = (*r)/number;
        }
    }
}
void lineread2(char *expr, int *fd, int *fp, int *rp, char * operationp){
    int i;
    int first = 0;
    int *f = &first;
    int result;
    int *r = &result;
    char operation[1];
    for (i = 0; expr[i]!='\0'; i++){
        if(expr[i]=='(' && i == 0){
            int endp =  braketread(expr, i);
            char expression[endp-i+2];
            memmove(expression, &expr[i], (size_t)(endp-i+1));
            expression[endp-i+1] = '\0';
            i++;
            if(expr[i]=='*'||expr[i]=='+'||expr[i]=='/'||expr[i]=='-')
            {
                operation[0] = expr[i];
            }
            else
            {
                printf("error");
                exit(EXIT_FAILURE);
            }
        }
        if(expr[i]=='('&& i!= 0){
            i = braketexpr2(expr, i, fd, f , r, operation);
        }
        if(isdigit(expr[i])){
            i = readdigit(expr, i, fd, f , r, operation);
        }
    }
    printf("%d\n", result);
    int endp =  braketread(expr, 0);
    char expression[endp+2];
    memmove(expression, &expr[0], (size_t)(endp+1));
    expression[endp+1] = '\0';
    close(fd[0]);
    fd[0] = -1;
    write(fd[1], &result, sizeof(int));
    printf("PID %d: Processed \"%s\"; sending \"%d\" on pipe to parent\n", getpid(), expression, result );
}
//printf("%d\n", len);
//printf("test\n")
