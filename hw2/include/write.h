
/*
 * Type definitions for functions in write.c
 */

void writeprofessor(FILE *fd, Professor *p);
void writeassistant(FILE *fd, Assistant *a);
void writescore(FILE *fd, Score *s);
void writestudent(FILE *fd, Student *s);
void writesection(FILE *fd, Section *s);
void writeassignment(FILE *fd, Assignment *a);
void writecourse(FILE *fd, Course *c);
void writefile(char *f, Course *c);
