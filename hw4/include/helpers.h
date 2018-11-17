#include "command.h"
#include "imprimer.h"

#define MAX_FILE_TYPES 64

//functions prototypes
char *removeWhite(char *line);
void execute_cmd_line(char *line);
void initialize_readline();
int is_white_line(char *line);
COMMAND *find_cmd(char *name);
void set_up_cmd_args_CLL();
void insert_arg(char *name, ARGUMENT *last_arg);
void usage(char *progname);
void initialize_printer(char *name, char *file_type);
void initialize_conv_cells(char *file_type, int start_row, int start_col);
void print_cmd_error_msg(char *msg);
int isDeclaredType(char *file_type);
int isDeclaredPrinter(char *printer_name);

void print_all_types();

JOB *find_job_with_pgid(int pgid);
void sigchildHandler(int sig);
void fork_pipe_processes(JOB *job, int conv_flag, char *envp[]);
PRINTER *choosePrinterFromPS(JOB *job);



typedef struct FILE_TYPE {
    char *name;
    struct FILE_TYPE *next;
    struct FILE_TYPE *prev;
} FILE_TYPE;

FILE_TYPE file_type_cll_head;

void set_up_file_type_CLL();
void insert_file_type(char *name);


typedef struct CONV_CELL {
    int row_index;
    int col_index;
    char *prog;
    char *prog_args; //remember to strtok() three times to remove cmd, file_type1, file_type2
    char *row_name;
    char *col_name;
    char *parent_row;
} CONV_CELL;

CONV_CELL *find_conv_cell(char *row_name, char *col_name);
int find_rowname_index(char *row_name);
int isAdjacent(CONV_CELL *cell);
CONV_CELL *getAdjacentCell(CONV_CELL *cell);


/* **************************************************************** */
/*                                                                  */
/*                  BFS STRUCT(S) AND FUNCTIONS                     */
/*                                                                  */
/* **************************************************************** */

typedef struct QUEUE {
    CONV_CELL *items[MAX_FILE_TYPES];
    int front;
    int rear;
} QUEUE;

QUEUE *createQueue();
void enqueue(QUEUE *q, CONV_CELL *item);
CONV_CELL *dequeue(QUEUE *q);
int isEmpty(QUEUE *q);

typedef struct VISITED_CELL {
    int visited;
    int parent_row_index;
    int row_index;
    int col_index;
} VISITED_CELL;

void initialize_parent_index();
void initialize_visited();

VISITED_CELL visited_matrix[MAX_FILE_TYPES][MAX_FILE_TYPES];
VISITED_CELL *lastVisitedCell;

CONV_CELL *pipeline_steps[MAX_FILE_TYPES];

void bfs(char *start_row, char *end_row);

void getConversionSteps();

VISITED_CELL *getParentCell(VISITED_CELL *cell);

void recurssive_func(VISITED_CELL *vc, int pos, CONV_CELL *steps[MAX_FILE_TYPES]);

int find_pipeline(char *start_row, char *end_col);




/* **************************************************************** */
/*                                                                  */
/*                  JOB QUEUE STRUCT(S) AND FUNCTIONS               */
/*                                                                  */
/* **************************************************************** */
typedef struct JOB_NODE
{
    JOB *job;
    struct JOB_NODE *next_jnode;
    struct JOB_NODE *prev_jnode;
}JOB_NODE;

JOB_NODE job_queue_head;

void initialize_job_queue_head();

void jq_enqueue(JOB_NODE *item);

JOB_NODE *jq_dequeue();

int jq_isEmpty();

void initialize_job(JOB *jp, int id, JOB_STATUS, char *file_name, char *file_type, char *ps[], int i);

PRINTER *find_printer(char *name);

