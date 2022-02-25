// Charlie Cvitkovich

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
    char name[32]; // not specified so doing usual 32.
    char value[50]; // If the user enters a var larger than 50, it produces a sig fault error. Because the document says we do not need to handle errors,
    // we will not be handling the error condition.
} variable;

void addToVariables(variable variables[10], char * bufferParse, int * i) {

    // === name assignment ===
    char name[32] = " ";
    strcpy(name, bufferParse);
    strtok(name, " ");

    // === value assignment ===
    char bufferCopy[256] = " "; // not sure how big the full command will be so 256 to be safe.
    char * value;
    strcpy(bufferCopy, bufferParse);
    value = strstr(bufferCopy, "\"");
    value = value + 1; // removes first quote
    strtok(value, "\""); // removes second quote

    // === new variable ===
    variable newVar;
    strcpy(newVar.name, name);
    strcpy(newVar.value, value);

    // === adding to list ===
    if (*i == 0) {
        variables[*i] = newVar;
        (*i)++;
    }
    else {
        for (int j = 0; j < *i; j++) {
            // if the variable is already in our system, update it
            if (strcmp(name, variables[j].name) == 0) {
                // reassigning everything just because I would have to make a copy of the value to store it in variable.value if I did not.
                variables[j] = newVar;
                break;
            } else if (*i - j == 1 && *i != 10) {
                variables[*i] = newVar;
                (*i)++;
            }
        }
    }
}

void returnVariable(variable variables[10], char * bufferParse, const int * i) {
    // === copying variables ===
    char bufferCopy[256] = " ";
    char * name;
    strcpy(bufferCopy, bufferParse);
    name = strstr(bufferCopy, " ");
    name = name+1;

    // === checking for variable ===
    for (int j = 0; j < *i; j ++) {
        if (strcmp(name, variables[j].name) == 0) {
            printf("%s\n", variables[j].value);
            break;
            //if at the last interval, then return error
        } else if (*i - j == 1) {
            printf("ERROR: Variable '%s' does not exist\n", name);
        }
    }
}

void runProgram(char * buffer) {
    int status = 0;
    int pid = fork();
    if (pid == 0) {
        char * arguments[] = {NULL};
        char * envp[] = {NULL};
        execve(buffer, arguments, envp);
    } else {
        waitpid(-1, &status, 0);
    }
}

int main() {
    int i = 0; // to mark the current empty position on the variables list
    variable variables[10];

    while (true) {

        /* ============== init for shell ====================== */
        printf("mstrkrbs > ");
        char buffer[32];
        fgets(buffer, 32, stdin);

        /* =============== parsing buffer ===================== */
        char* bufferParse = buffer;
        while((bufferParse != NULL) && (*bufferParse != '\n')) {
            ++bufferParse;
        }
        *bufferParse = '\0';

        /* =============== check for command ================= */
        if (strstr(buffer, " = ")) {
            addToVariables(variables, buffer, &i);
            // add to variables
        } else if (strstr(buffer, "echo ")) {
            returnVariable(variables, buffer, &i);
        } else {
            runProgram(buffer);
        }
    }
}