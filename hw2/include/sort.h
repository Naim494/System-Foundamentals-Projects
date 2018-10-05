
/*
 * Function prototypes for sorting routines.
 */

Student *getnext(Student *s);
Student *getcnext(Student *s);
void setnext(Student *s, Student *n);
void setcnext(Student *s, Student *n);
void sortrosters(Course *c, int compare());
Student *sortroster(Student *s, Student *gtnxt(), void stnxt(), int compare());
void sorttable(Student **stab, int low, int high, int compare());
int partition(Student **stab, int low, int high, int compare());
int comparescore(Student *s1, Student *s2);
int comparename(Student *s1, Student *s2);
int compareid(Student *s1, Student *s2);
void checkfordups(Student *sp);
