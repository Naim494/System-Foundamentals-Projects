/**
 * === DO NOT MODIFY THIS FILE ===
 * If you need some other prototypes or constants in a header, please put them
 * in another header file.
 *
 * When we grade, we will be replacing this file with our own copy.
 * You have been warned.
 * === DO NOT MODIFY THIS FILE ===
 */
#ifndef IMPRIMER_H
#define IMPRIMER_H

#include <stdint.h>
#include <stdlib.h>

/* PRINTERS */

typedef struct printer {
    int id;            /* Determines bit in PRINTER_SET */
    char *name;        /* Name of the printer. */
    char *type;        /* Type of file printer can print. */
    int enabled;       /* Is printer enabled (0 = NO, nonzero = YES) */
    int busy;          /* Is printer busy (0 = NO, nonzero = YES) */
    void *other_info;  /* You may store other info in this field. */
} PRINTER;

#define MAX_PRINTERS 32       /* Maximum number of printers. */

/*
 * Bitmap to represent a set of printers.
 * If s is of type PRINTER_SET and p points to a PRINTER,
 * then the printer is a member of set s if the value s & (0x1 << p->id)
 * is nonzero.  The value ANY_PRINTER represents the set of all printers.
 */
typedef uint32_t PRINTER_SET;
#define ANY_PRINTER (0xffffffff)

/*
 * Function to format printer status line for output.
 * Caller must supply pointer "buf" to a buffer of size "size".
 * Return value is "buf".
 */
char *imp_format_printer_status(PRINTER *printer, char *buf, size_t size);

/*
 * Function to connect to the specified printer, which is started if it
 * is not already.  Flags modify behavior of printer on startup.
 * Return value is file descriptor to write to printer, if successful,
 * otherwise -1 if unsuccessful.
 */
int imp_connect_to_printer(PRINTER *printer, int flags);

/* Flags for imp_connect_to_printer() */
#define PRINTER_NORMAL (0x0)  /* "Normal" printer behavior. */
#define PRINTER_DELAY (0x1)   /* Printing may involve random delays. */
#define PRINTER_FLAKY (0x2)   /* Printer may randomly "disconnect". */

/* JOBS */

typedef enum { QUEUED, RUNNING, PAUSED, COMPLETED, ABORTED } JOB_STATUS;

typedef struct job {
    int jobid;                      /* Job ID of job. */
    JOB_STATUS status;              /* Status of job. */
    int pgid;                       /* Process group ID of conversion pipeline. */
    char *file_name;                /* Pathname of file to be printed. */
    char *file_type;                /* Type name of file to be printed. */
    PRINTER_SET eligible_printers;  /* Set of eligible printers for the job. */
    PRINTER *chosen_printer;        /* Printer chosen for this job. */
    struct timeval creation_time;   /* Time job was queued. */
    struct timeval change_time;     /* Time of last status change. */
    void *other_info;               /* You may store other info in this field. */
} JOB;

/*
 * Function to format job status line for output.
 * Caller must supply pointer "buf" to a buffer of size "size".
 * Return value is "buf".
 */
char *imp_format_job_status(JOB *job, char *buf, size_t size);

/*
 * Function to format error message for command response.
 * Caller must supply pointer "buf" to a buffer of size "size".
 * Return value is "buf".
 */
char *imp_format_error_message(char *msg, char *buf, size_t size);

#endif
