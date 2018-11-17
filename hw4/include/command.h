
#include <readline/readline.h>
#include <readline/history.h>

//imprimer command function prototypes
int quit_cmd(char *args);
int help_cmd(char *args);
int type_cmd(char *args);
int printer_cmd(char *args);
int conversion_cmd(char *args);
int printers_cmd(char *args);
int jobs_cmd(char *args);
int print_cmd(char *args);
int cancel_cmd(char *args);
int pause_cmd(char *args);
int resume_cmd(char *args);
int disable_cmd(char *args);
int enable_cmd(char *args);


typedef struct ARGUMENT{
    char *name;
    struct ARGUMENT *next;
    struct ARGUMENT *prev;
} ARGUMENT;


//imprimer command structure
typedef struct {
    char *name;
    ARGUMENT args_head;      //arguments linked list
    rl_icpfunc_t *func;
} COMMAND;





