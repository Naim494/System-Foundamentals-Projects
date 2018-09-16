
/*
 * Data structures to hold per-assignment statistics
 */

/*
 * Frequency information is stored as a linked list of "Freqs" buckets,
 * sorted from lowest raw score to highest raw score.  Quantiles can
 * be computed by traversing the list looking for the score of interest,
 * and then using the frequency information in that bucket.
 */

typedef struct Freqs {
        float score;                    /* The raw score */
        int count;                      /* Frequency for this score */
        int numless;                    /* Number of scores < this */
        int numlesseq;                  /* Number of scores <= this */
        struct Freqs *next;             /* Pointer to next higher score */
} Freqs;

/*
 * The statistical data for assignments are accumulated in the
 * "Classstats" and "Sectionstats' data structures.
 * The "Classstats" structure contains data on one assignment
 * for the whole class, and a pointer to a list of "Sectionstats"
 * structures, which have data on that same assignment, only
 * for each section separately.
 */

typedef struct Sectionstats {
        Assignment *asgt;               /* Assignment stats are for */
        Section *section;               /* Section stats are for */
        int valid;                      /* Number of valid scores */
        int tallied;                    /* Number of scores tallied */
        double sum;                     /* Sum of valid scores */
        double sumsq;                   /* Sum of squares of valid scores */
        float min;                      /* Minimum valid score */
        float max;                      /* Maximum valid score */
        float mean;                     /* Sample mean for valid scores */
        float stddev;                   /* Sample standard deviation */
        Freqs *freqs;                   /* Frequency information */
        struct Sectionstats *next;      /* Pointer to data for next section */
} Sectionstats;

typedef struct Classstats {
        Assignment *asgt;               /* The assignment stats are for */
        int valid;                      /* Number of valid scores */
        int tallied;                    /* Number of scores tallied */
        double sum;                     /* Sum of valid scores */
        double sumsq;                   /* Sum of squares of valid scores */
        float min;                      /* Minimum valid score */
        float max;                      /* Maximum valid score */
        float mean;                     /* Sample mean for valid scores */
        float stddev;                   /* Sample std deviation */
        Freqs *freqs;                   /* Frequency information */
        Sectionstats *sstats;           /* Per-section statistics */
        struct Classstats *next;        /* Pointer to data for next asgt */
} Classstats;

/*
 * The data for a whole class is headed by a "Stats" structure
 */

typedef struct Stats {
        Classstats *cstats;             /* List of per-assignment data */
} Stats;

Stats *statistics(Course *c);
Stats *buildstats(Course *c);
void do_links(Course *c, Stats *s);
void do_freqs(Course *c);
Freqs *count_score(Score *scp, Freqs *afp);
void do_quantiles(Stats *s);
void do_sums(Course *c);
void do_moments(Stats *s);
double stddev(int n, double sum, double sumsq);
