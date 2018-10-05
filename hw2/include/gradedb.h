
/*
 * Data structures stored in grades database
 */

typedef char *Surname;                  /* Person's surname */
typedef char *Name;                     /* Course name or person's name */
typedef char *Id;                       /* Short identifying name or number */
typedef char *Atype;                    /* Assignment type */

typedef struct {
        Surname surname;                /* Professor's surname */
        Name name;                      /* Professor's given name */
} Professor;

typedef struct {
        Surname surname;                /* Assistant's surname */
        Name name;                      /* Assistant's given name */
} Assistant;

typedef enum { VALID, INVALID } Gflag;
typedef enum { USERAW, USENORM, USELIKEAVG, USECLASSAVG } Gsubst;

typedef struct Score {
        struct Assignment *asgt;        /* Assignment grade is for */
        float grade;                    /* Numeric grade */
        float qnorm;                    /* Substitute quantile score */
        float lnorm;                    /* Substitute linear score */
        float snorm;                    /* Substitute scale score */
        Gflag flag;                     /* Validity flag */
        Id code;                        /* Why score is invalid */
        Gsubst subst;                   /* Invalid grade substitution info */
        struct Classstats *cstats;      /* Pointer to statistics by class */
        struct Sectionstats *sstats;    /* Pointer to statistics by section */
        struct Score *next;             /* Next score for student */
} Score;

typedef struct Student {
        Id id;                          /* Student ID number */
        Surname surname;                /* Student's surname */
        Name name;                      /* Student's given name */
        Score *rawscores;               /* Student's raw scores */
        Score *normscores;              /* Student's normalized scores */
        float composite;                /* Student's composite score */
        struct Section *section;        /* Pointer to student's section */
        struct Student *next;           /* Next student in section roster */
        struct Student *cnext;          /* Next student in course roster */
} Student;

typedef struct Section {
        Name name;                      /* Name or number of section */
        Assistant *assistant;           /* Assistant in charge of section */
        Student *roster;                /* List of students in section */
        struct Section *next;           /* Next section in course */
} Section;

typedef enum { NOWEIGHT, WEIGHT } Wpolicy;
typedef enum { RAW, LINEAR, QUANTILE, SCALE } Npolicy;
typedef enum { BYCLASS, BYSECTION } Ngroup;

typedef struct Assignment {
        Id name;                        /* Identifying name of assignment */
        Atype atype;                    /* Assignment type (homework, exam) */
        Wpolicy wpolicy;                /* Is assignment weighted? */
        float weight;                   /* Weighting of this assignment */
        Npolicy npolicy;                /* Normalization to use */
        float max;                      /* Maximum possible score */
        float mean;                     /* Mean for LINEAR normalization */
        float stddev;                   /* Std dev for LINEAR normalization */
        float scale;                    /* New maximum for SCALE norm'zation */
        Ngroup ngroup;                  /* Group over which to normalize */
        struct Assignment *next;        /* Next assignment in course */
} Assignment;

typedef struct {
        Id number;                      /* Course number */
        Name title;                     /* Course title */
        Professor *professor;           /* Professor in charge of course */
        Assignment *assignments;        /* List of assignments in course */
        Section *sections;              /* List of sections in course */
        Student *roster;                /* List of students in course */
} Course;

