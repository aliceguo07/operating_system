#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int traceAll(char* exp, int s);
void error1(int status, pid_t child_pid);
void error2(int operand_count, pid_t pid, int result, int s, int *p);
int calculate(char operator, int result, int operand);
void master1(char *exp,int *p, int s, int e);
void master2(char *exp,int *p, int s, int e);
void evaluate(char *exp, int *p, int s, int e);


/*
void processInput(FILE * myfile, char * ultimateArray ){
	char whydoyoucare[100];
	char innerArray[100];
	int tracker = 0;
	int innerTracker = 0;
	bool alreadygood = false;
	bool processingInner = false;
	while (scanf(myfile, "&s", whydoyoucare)){
		if (isOperator(whydoyoucare) == 1 and alreadygood == false){
			char idontcare[100];
			memcpy(idontcare, &whydoyoucare[1], len(whydoyoucare));
			ultimateArray[tracker] == idontcare;
			alreadygood == true;
			tracker++;
			continue;
		}
		if (hasLeftPara(whydoyoucare) = 1 and alreadygood){
			char idontcare[100];
			memcopy(idontcare, &whydoyoucare[1], len(whydoyoucare));
			innerArray[] ==
		}
	}

}
*/

void master1(char *exp,int *p, int s, int e){
    pid_t pid = getpid();
    char op[20];
    int op_end = traceAll(exp, s + 1);
    memcpy(op, &exp[s + 1], (size_t)op_end - s - 1);
    op[op_end - s - 1] = '\0';
    if (op[0] != '+' && op[0] != '-' && op[0] != '*' && op[0] != '/' ) {
        printf("PROCESS %d: ERROR: unknown '%s' operator\n", pid, op);
        exit(EXIT_FAILURE);
    }

    printf("PROCESS %d: Starting '%s' operation\n", pid, op);

    int result = 0;
    int operand_count = 0;
    int j = 0;
    for (int i = op_end + 1;i < e;i=j+1) {
        j = traceAll(exp, i);
        if (j < e) { operand_count++;}
        else { break; }
        pid = fork();
        if (pid == 0) {
            evaluate(exp, p, i, j);
            return;
        }
        else {
            int status;
            pid_t child_pid = wait(&status);
            int operand;
            read(p[0], &operand, sizeof(int));
            if (operand_count == 1) {
                result = operand;
            }
            else {
                result = calculate(op[0], result, operand);
            }
            error1(status,child_pid);
        }
    }
    pid = getpid();
    error2(operand_count,pid,result,s,p);
}


void evaluate(char *exp, int *p, int s, int e)
{
    if (exp[s] == '(') { master1(exp,p,s,e); }

    else {  master2(exp,p,s,e);    }

}

void error1(int status, pid_t child_pid){
    if (WIFSIGNALED(status)) {
        printf("PARENT: child %d terminated abnormally\n", child_pid);
        exit(EXIT_FAILURE);
    }
    else if (WEXITSTATUS(status) != 0) {
        printf("PARENT: child %d terminated with nonzero "
         "exit status %d\n", child_pid, WEXITSTATUS(status));
        exit(EXIT_FAILURE);
    }
}

void error2(int operand_count, pid_t pid, int result, int s, int *p){
    if (operand_count < 2) {
        printf("PROCESS %d: ERROR: not enough operands\n", pid);
        exit(EXIT_FAILURE);
    }
    if (s == 0) {
        printf("PROCESS %d: Final answer is '%d'\n", pid, result);
    }
    else {
        write(p[1], &result, sizeof(int));
        printf("PROCESS %d: Sending '%d' on pipe to parent\n", pid, result);
    }
}


void master2(char *exp,int *p, int s, int e){
    int n = 0;
    char operand[20];
    memcpy(operand, &exp[s], (size_t)e - s);
    operand[e - s] = '\0';
    n = atoi(operand);

    close(p[0]);
    p[0] = -1;

    write(p[1], &n, sizeof(int));

    pid_t pid = getpid();
    printf("PROCESS %d: Sending '%d' on pipe to parent\n", pid, n);
}


int traceAll(char* exp, int s)
{
    int e = s;
    char c = exp[s];
    int brackets = 0;
    if (c == '(') {
        for (int i=0; i<100;i++){
            if (c == '(') { brackets++;}
            else if (c == ')') { brackets--;}
            e++;
            c = exp[e];
            if (brackets == 0){ break;}
        };
    } else { 
        while (c != ' ' && c != ')') { e++; c = exp[e];}
    }
    return e;
}

int calculate(char theoperator, int result, int operand) {
    switch (theoperator) {
        case '+': return result + operand;
        case '-': return result - operand;
        case '*': return result * operand;
        case '/':
            if (operand==0){printf("Can't be divided by 0");exit(1);}
            return result / operand;
        default:
            return result;
    }
}


/*
int main( int argc, char * argv[] )   
{   
    if (argc != 2){
        printf("ERROR: Invalid arguments\n");
        printf("USAGE: ./a.out <input-file> <chunk-size>\n");
        return EXIT_FAILURE;
    }
    FILE * myfile = fopen(argv[1], "r");
    char * inputarray[100];
    int tracker = 0;
    char myArray[26] = { 0 };
    fread(myArray, sizeof(myArray), myfile);
    testPrint(myArray);
}
*/

int main(int argc, char* argv[])
{
    if (argc != 2){
        printf("ERROR: Invalid arguments\n");
        printf("USAGE: ./a.out <input-file> <chunk-size>\n");
        return EXIT_FAILURE;
    }
    FILE* fp = fopen(argv[1], "r");
    int p[2];
    int rc = pipe(p);
    char exp[1024];
    fgets(exp, 1024, fp);
    evaluate(exp, p, 0, traceAll(exp, 0));
    fclose(fp);
    rc = 0;
    return EXIT_SUCCESS;
}

/*
int main( int argc, char * argv[] )   
{   
    if (argc != 2){
        printf("ERROR: Invalid arguments\n");
        printf("USAGE: ./a.out <input-file> <chunk-size>\n");
        return EXIT_FAILURE;
    }
    FILE * myfile = fopen(argv[1], "r");
    char exp[100];
    fgets(exp,100,myfile);
    char *token;
    const char s[3] = "()";
    token = strtok(exp,s);
    int really = 0;
    char *items[100];

    int iv = evaluate(token,s);
    printf("%d\n", iv);
    while(token!=NULL){
        printf("%s\n", token);
        token = strtok(NULL,s);
    }
    
    size_t count = 0;
    while (items[count] != NULL){
        printf("%s\n", items[count]);
        count++;
    }

}
*/


