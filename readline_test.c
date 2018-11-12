#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

int is_white_line(char *line);


int main(int argc, char *argv[]) {

    char *line, *token, *del = " ";


    while(1) {

        line = readline("in> ");

        if(line && *line) {

            if(!is_white_line(line)) {
                add_history(line);
                token = strtok(line, " ");

                while( token != NULL ) {
                    printf( "%s\n", token );
                    token = strtok(NULL, del);
                }
            }
        }

        free(line);

    }

    return 0;
}

int is_white_line(char *line) {

    while(*line != '\0') {
        char c = *line;
        line++;
        if(!isspace(c))
            return 0;
    }
    return 1;
}