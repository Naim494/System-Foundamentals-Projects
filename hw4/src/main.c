#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>

#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/time.h>

#include "helpers.h"

/*
 * "Imprimer" printer spooler.
 */

//array containing all imprimer commands
COMMAND commands[] = {
    {.name = "quit", .func = quit_cmd},
    {.name = "help", .func = help_cmd},
    {.name = "type", .func = type_cmd},
    {.name = "printer", .func = printer_cmd},
    {.name = "conversion", .func = conversion_cmd},
    {.name = "printers", .func = printers_cmd},
    {.name = "jobs", .func = jobs_cmd},
    {.name = "print", .func = print_cmd}
};

PRINTER printers[MAX_PRINTERS];

int printer_count = 0;

int job_count = 0;

//commands array length
#define L (sizeof(commands)/sizeof(COMMAND))

CONV_CELL conversion_matrix[MAX_FILE_TYPES][MAX_FILE_TYPES];
int row_cnt = 0;
int col_cnt = 0;


char *delimeter = " ";
int done = 0;
char *progname;

//buffer for error messages
char *buf;
size_t buf_size;
char *error_msg;

int main(int argc, char *argv[]) {

    progname = argv[0];

    char optval;
    while(optind < argc) {
       if((optval = getopt(argc, argv, "")) != -1) {
           switch(optval) {
               case '?':
                  fprintf(stderr, "Usage: %s [-i <cmd_file>] [-o <out_file>]\n", argv[0]);
                  exit(EXIT_FAILURE);
                  break;
               default:
                  break;
           }
       }
    }

    //set up CLL of arguments for each command
    set_up_cmd_args_CLL();

    //set up CLL for file types
    set_up_file_type_CLL();

    char *cmd_line;

    //set up completer
    initialize_readline();

    //read and execute command lines
    while(!done) {

        cmd_line = readline("imp> ");

        if(cmd_line && *cmd_line) {
            if(!is_white_line(cmd_line)){
                add_history(cmd_line);
                execute_cmd_line(cmd_line);
            }
        }

        free(cmd_line);
    }

    exit(EXIT_SUCCESS);
}



/* **************************************************************** */
/*                                                                  */
/*                  Interface to Readline Completion                */
/*                                                                  */
/* **************************************************************** */

char *command_generator (const char *, int);
char **imprimer_completion (const char *, int, int);

void initialize_readline() {

    //Allow conditional parsing of the ~/.inputrc file.
    rl_readline_name = "Imprimer";

    rl_attempted_completion_function = imprimer_completion;
}

char **imprimer_completion(const char *text, int start, int end) {

    char **matches;

    matches = (char **)NULL;

    if (start == 0)
        matches = rl_completion_matches (text, command_generator);

    return (matches);

}

char *command_generator(const char *text, int state) {

    static int list_index, len;
    char *name, *name_cpy;

    if (!state) {

      list_index = 0;
      len = strlen (text);
    }

    while ((name = commands[list_index].name)) {
        list_index++;

        if (strncmp (name, text, len) == 0) {
            name_cpy = malloc(len + 1);
            strcpy(name_cpy, name);
        return name_cpy;
        }
    }

    return ((char *)NULL);
}



/* **************************************************************** */
/*                                                                  */
/*                       imprimer Commands                          */
/*                                                                  */
/* **************************************************************** */

int quit_cmd(char *args) {

    done = 1;
    return 0;
}

int help_cmd(char *args) {

    usage(progname);
    return 0;
}

int type_cmd(char *args) {

    strtok(args, delimeter); //take cmd token out

    char *token = strtok(NULL, delimeter);

    if(token) {

        if(isDeclaredType(token)) {
            error_msg = "FILE_TYPE ALREADY DECLARED";
            print_cmd_error_msg(error_msg);
            return 0;
        }

        insert_file_type(token);
        initialize_conv_cells(token, row_cnt, col_cnt);
        row_cnt++;
        col_cnt++;
    }
    else {
        //use imp_format_error_message here instead
        error_msg = "FILE_TYPE REQUIRED";
        print_cmd_error_msg(error_msg);
    }

    return 0;
}

int printer_cmd(char *args) {

    strtok(args, delimeter); //take cmd token out

    char *printer_name = strtok(NULL, delimeter);
    char *file_type = strtok(NULL, delimeter);

    if(!printer_name || !file_type) {     //use imp_format_error_message here instead
        error_msg = "PRINTER_NAME/FILE_TYPE REQUIRED";
        print_cmd_error_msg(error_msg);
    }
    else {
        initialize_printer(printer_name, file_type);
    }

    return 0;
}

int conversion_cmd(char *args) {
    char *args_cpy =  malloc(sizeof(char)*100);
    strcpy(args_cpy, args);

    strtok(args_cpy, delimeter); //take cmd token out

    char *file_type_1 = strtok(NULL, delimeter);
    char *file_type_2 = strtok(NULL, delimeter);
    char *conv_progname = strtok(NULL, delimeter);

    if(!file_type_1 || !file_type_2 || !conv_progname) {
        //use imp_format_error_message here instead
        usage(progname);
    }
    else {

        CONV_CELL *cell = find_conv_cell(file_type_1, file_type_2);

        if(cell) {
            cell -> prog = conv_progname;
            cell -> prog_args = args;

        }
        else {
            error_msg = "FILE TYPES WERE NOT FOUND IN CONVERSION MATRIX";
            print_cmd_error_msg(error_msg);
        }

    }

    free(args_cpy);

    return 0;
}

int printers_cmd(char *args) {

    char *buf;
    int i = 0;

    while(i < MAX_PRINTERS) {
        buf_size = sizeof(printers[i]) + sizeof(char)*50;
        buf = malloc(buf_size);
        printf("%s", imp_format_printer_status(&printers[i], buf, buf_size));
        i++;
    }

    free(buf);
    return 0;
}

int jobs_cmd(char *args) {



    return 0;
}

int print_cmd(char *args) {
    char *file_name_cpy = malloc(sizeof(char)*100);

    char *args_cpy =  malloc(sizeof(char)*100);
    strcpy(args_cpy, args);

    strtok(args_cpy, delimeter); //take cmd token out

    char *file_name = strtok(NULL, delimeter);
    strcpy(file_name_cpy, file_name);

    char *printers[MAX_PRINTERS];

    char *temp = strtok(NULL, delimeter);
    int i = 0;

    while(temp) {
        printers[i] = temp;
        temp = strtok(NULL, delimeter);
        i++;
    }

    if(!file_name) {

        //DISPLAY ERROR
    }
    else {
        strtok(file_name_cpy, ".");
        char *file_type = strtok(NULL, delimeter);

        if(file_type) {
            if(isDeclaredType(file_type)) {

                JOB new_job;
                gettimeofday(&(new_job.creation_time), NULL);
                JOB *njp = &new_job;

                initialize_job(njp, job_count, QUEUED, file_name, file_type, printers);

            }
            else{
                //DISPLAY ERROR
            }
        }
        else {
            //DISPLAY ERROR
        }
    }

    free(args_cpy);
    return 0;
}



/* **************************************************************** */
/*                                                                  */
/*                  Helper functions                                */
/*                                                                  */
/* **************************************************************** */

void initialize_job(JOB *jp, int id, JOB_STATUS status, char *file_name, char *file_type, char **ps) {

    jp -> jobid = id;
    jp -> status = status;
    gettimeofday(&(jp -> change_time), NULL);
    jp -> file_name = file_name;
    jp -> file_type = file_type;

    if(ps) {
        while(ps) {
            PRINTER *temp = find_printer(*ps);
            jp -> eligible_printers |= (1 << temp -> id);
            ps++;
        }
    }
    else{
        jp -> eligible_printers = ANY_PRINTER;
    }
}

int isDeclaredType(char *file_type) {

    CONV_CELL *cell;
    int i = 0;
    while(i < MAX_FILE_TYPES) {
        cell = &conversion_matrix[0][i];
        if(!strcmp(cell -> col_name, file_type))
            return 1;
    }
    return 0;
}

void set_up_file_type_CLL() {
    file_type_cll_head.next = &file_type_cll_head;
    file_type_cll_head.prev = &file_type_cll_head;
}

void insert_file_type(char *name) {
    FILE_TYPE new_ft;
    new_ft.name = name;
    new_ft.next = file_type_cll_head.next;
    new_ft.prev = &file_type_cll_head;

    //if sentinel is alone
    if(!(file_type_cll_head.next -> name)) {
        file_type_cll_head.next = &new_ft;
        file_type_cll_head.prev = &new_ft;
    }
    else {
        file_type_cll_head.next -> prev = &new_ft;
        file_type_cll_head.next = &new_ft;
    }
}

void execute_cmd_line(char *line) {
    char *args = malloc(sizeof(char)*100);
    strcpy(args, line);

    char *arg_name;

    char *cmd_name = strtok(args, delimeter);

    COMMAND *command = find_cmd(cmd_name);

    //do something here
    if(!command) {
        error_msg = "COMMAND REQUIRED";
        print_cmd_error_msg(error_msg);
    }

    arg_name = strtok(NULL, delimeter);

    //populate args linked list
    while(arg_name != NULL) {

        //the arg parameter should be the args head prev; which will always be the last arg in the list
        insert_arg(arg_name, command -> args_head.prev);

        arg_name = strtok(NULL, delimeter);
    }

    //call the command's function
    (*(command -> func)) (line);

    free(args);
}

COMMAND *find_cmd(char *name) {

    int i;

    for (i = 0; (i < L); i++)
        if (strcmp (name, commands[i].name) == 0)
            return (&commands[i]);

    return ((COMMAND *)NULL);
}

void set_up_cmd_args_CLL() {

    int i;

    for (i = 0; (i < L); i++) {
        commands[i].args_head.next = &(commands[i].args_head);
        commands[i].args_head.prev = &(commands[i].args_head);
    }
}

void insert_arg(char *name, ARGUMENT *last_arg) {
    ARGUMENT new_arg;
    new_arg.name = name;
    new_arg.next = last_arg -> next;
    new_arg.prev = last_arg;

    //if the prev arg is the sentine;
    if(!(last_arg -> name)) {
        last_arg -> next = &new_arg; //set sentinel's next arg to the new arg
        last_arg -> prev = &new_arg;  //set sentinel's prev arg to the new arg
    }
    else{
        last_arg -> next -> prev = &new_arg;  //set sentinel's prev arg to the new arg
        last_arg -> next = &new_arg;   //set prev arg's next to the new arg
    }
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

void initialize_conv_cells(char *file_type, int start_row, int start_col) {

    int i;

    //initialize row name for cells along start row
    for(i = 0; i < MAX_FILE_TYPES; i++) {
        conversion_matrix[start_row][i].row_index = start_row;
        conversion_matrix[start_row][i].col_index = i;
        conversion_matrix[start_row][i].row_name = file_type;
    }

    //initialize col name for cells along start col
    for(i = 0; i < MAX_FILE_TYPES; i++) {
        conversion_matrix[i][start_col].row_index = i;
        conversion_matrix[i][start_col].col_index = start_col;
        conversion_matrix[i][start_col].col_name = file_type;
    }
}

void initialize_printer(char *name, char *file_type) {

    printers[printer_count].id = printer_count;
    printers[printer_count].name = name;
    printers[printer_count].type = file_type;
    printers[printer_count].enabled = 0;
    printers[printer_count].busy = 0;

    //printer_set |= (1 << printer_count);

    printer_count++;
}

void usage(char *progname) {

    COMMAND *cmd;

    fprintf(stderr, "       Usage: imp> [command] <args ...>\n");
    fprintf(stderr, "       Valid options are:\n");

    for(int i = 0; i < L; i++) {

        cmd = &commands[i];

        fprintf(stderr, "           %s\n", cmd -> name);

    }
}

CONV_CELL *find_conv_cell(char *row_name, char *col_name) {

    int i;
    int row = find_rowname_index(row_name);
    int col = -1;

    //find col
    if(row >= 0) {
        for(i = 0; i < MAX_FILE_TYPES; i++) {
            if(!strcmp(conversion_matrix[row][i].col_name, col_name)) {
                col = i;
                return &conversion_matrix[row][col];
            }
        }
    }

    return (CONV_CELL*)NULL;
}

int find_rowname_index(char *row_name) {

    int i;
    int row = -1;

    for(i = 0; i < MAX_FILE_TYPES; i++) {
        if(!strcmp(conversion_matrix[i][0].row_name, row_name)) {
            row = i;
            break;
        }
    }

    return row;
}

void print_cmd_error_msg(char *msg) {

    buf_size = strlen(msg) + 50;
    buf = malloc(buf_size);
    printf("%s",imp_format_error_message(msg, buf, buf_size));
    free(buf);
}

QUEUE *createQueue() {
    QUEUE* q = malloc(sizeof(QUEUE));
    q->front = -1;
    q->rear = -1;
    return q;
}

int isEmpty(QUEUE *q) {
    if(q->rear == -1)
        return 1;
    else
        return 0;
}

void enqueue(QUEUE *q, CONV_CELL *item){
    if(q->rear == MAX_FILE_TYPES-1)
        printf("\nQueue is Full!!");
    else {
        if(q->front == -1)
            q->front = 0;
        q->rear++;
        q->items[q->rear] = item;
    }
}

CONV_CELL *dequeue(QUEUE *q){
    CONV_CELL *item;
    if(isEmpty(q)){
        //printf("Queue is empty\n");
        //item = NULL;
    }
    else{
        item = q->items[q->front];
        q->front++;
        if(q->front > q->rear){
            //printf("Resetting queue\n");
            q->front = q->rear = -1;
        }
    }
    return item;
}

int isAdjacent(CONV_CELL *cell) {

    if(cell -> prog)
        return 1;
    return 0;
}

CONV_CELL *getAdjacentCell(CONV_CELL *cell) {

    CONV_CELL *temp = cell;
    int row = cell -> row_index;
    int col = cell -> col_index;

    while(temp) {

        if(++col < MAX_FILE_TYPES) {
            temp = &conversion_matrix[row][col];
            if(isAdjacent(temp))
                return temp;
        } else return (CONV_CELL *)NULL;
    }

    return (CONV_CELL *)NULL;
}

void bfs(char *start_row, char *end_col) {

    //initialie parent index of visited matrix
    initialize_parent_index();

    QUEUE *q = createQueue();

    //find row
    int start_row_index = find_rowname_index(start_row);

    CONV_CELL *root = &conversion_matrix[start_row_index][0];

    if(start_row_index >= 0) {

         //if the first conversion cell satisfies the conversion,
        if(!strcmp(root -> col_name, end_col)) {

            //if there is a conversion program available
            if(isAdjacent(root)) {

                visited_matrix[start_row_index][0].visited = 1;
                lastVisitedCell = &visited_matrix[start_row_index][0];
                lastVisitedCell -> row_index = start_row_index;
                lastVisitedCell -> col_index = 0;
            }

            return;
        }

        if(isAdjacent(root)) {
            visited_matrix[start_row_index][0].visited = 1;
            lastVisitedCell = &visited_matrix[start_row_index][0];
            lastVisitedCell -> row_index = start_row_index;
            lastVisitedCell -> col_index = 0;
        }

        enqueue(q, root);

        do {

            CONV_CELL *current_conv_cell = dequeue(q);

            CONV_CELL *temp;

            while(1) {

                temp = getAdjacentCell(current_conv_cell);

                if(!temp) {
                    break;
                }

                int row_index = temp -> row_index;
                int col_index = temp -> col_index;

                if(visited_matrix[row_index][col_index].visited == 0) {
                    visited_matrix[row_index][col_index].visited = 1;

                    //SETTING PARENT
                    visited_matrix[row_index][col_index].parent_row_index = visited_matrix[row_index][0].parent_row_index;

                    visited_matrix[row_index][col_index].row_index = row_index;
                    visited_matrix[row_index][col_index].col_index = col_index;

                    CONV_CELL *result_cell = &conversion_matrix[temp -> col_index][0];

                    //SETTING PARENT
                    visited_matrix[temp -> col_index][0].parent_row_index = temp -> row_index;

                    lastVisitedCell = &visited_matrix[row_index][col_index];

                    enqueue(q, result_cell);
                }

                //if the conversion cell satisfies the conversion,
                if(!strcmp(temp -> col_name, end_col)) {
                    return;
                }
                current_conv_cell = temp;
            }
        }
        while(!isEmpty(q));
    }

    free(q);

}

void getConversionSteps() {

    recurssive_func(lastVisitedCell, 0, pipeline_steps);

    // for(int i = MAX_FILE_TYPES-1; i >= 0; i--) {

    //     if(pipeline_steps[i]) {
    //         printf("%s ", pipeline_steps[i] -> prog);

    //         if(i > 0)
    //         printf("-> ");
    //     }

    // }
}

void recurssive_func(VISITED_CELL *vc, int pos, CONV_CELL *steps[MAX_FILE_TYPES]) {

    int array_pos = pos;
    int parent_row_index = vc -> parent_row_index;

    if(parent_row_index < 0) {

        steps[array_pos] = &conversion_matrix[vc -> row_index][vc -> col_index];
        return;
    }

    recurssive_func(getParentCell(vc), ++pos, steps);
    steps[array_pos] = &conversion_matrix[vc -> row_index][vc -> col_index];
    return;
}

VISITED_CELL *getParentCell(VISITED_CELL *cell) {

    CONV_CELL *conv_cell = &conversion_matrix[cell -> row_index][cell -> col_index];

    int parent_row_index = cell -> parent_row_index;
    int parent_col_index;

    if(parent_row_index >= 0) {

        parent_col_index = conv_cell -> row_index;
        return &visited_matrix[parent_row_index][parent_col_index];
    }

    return (VISITED_CELL *)NULL;
}

void initialize_parent_index() {

    int i;
    int j;
    for(i = 0; i < MAX_FILE_TYPES; i++) {

        for(j = 0; j < MAX_FILE_TYPES; j++) {

            visited_matrix[i][j].parent_row_index = -1;
        }
    }
}

void initialize_job_queue_head() {

    job_queue_head.next_jnode = &job_queue_head;
    job_queue_head.prev_jnode = &job_queue_head;
}

void jq_enqueue(JOB_NODE *item) {

    item -> next_jnode = job_queue_head.next_jnode;
    item -> prev_jnode = &job_queue_head;

    if(job_queue_head.next_jnode == &job_queue_head) {
        job_queue_head.prev_jnode = item;
    }
    else job_queue_head.next_jnode -> prev_jnode = item;

    job_queue_head.next_jnode = item;
}

int jq_isEmpty() {

    if(job_queue_head.next_jnode == &job_queue_head)
        return 1;
    return 0;
}

JOB_NODE *jq_dequeue() {

    JOB_NODE *temp = job_queue_head.next_jnode;

    if(temp -> next_jnode == &job_queue_head) {
        initialize_job_queue_head();
    }
    else {
        temp -> next_jnode -> prev_jnode = &job_queue_head;
        job_queue_head.next_jnode = temp -> next_jnode;
    }

    return temp;
};

PRINTER *find_printer(char *name) {

    int i = 0;

    while(i < MAX_PRINTERS) {
        if(!strcmp(printers[i].name, name)) {
            return &printers[i];
        }
        i++;
    }

    return (PRINTER *)NULL;
}

