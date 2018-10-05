#include "gradedb.h"
#include "stats.h"
#include "deallocate.h"
#include <stdlib.h>
#include <stdio.h>


//STRUCTURES IN GRADEDB.H

void free_assignment(Assignment *assignment) {

    free(assignment -> name);
    free(assignment -> atype);
    free(assignment);
}

void free_all_assignments(Assignment *assignment) {

    Assignment *next = NULL;

    while(assignment != NULL){

        next = assignment -> next;
        free_assignment(assignment);
        assignment = next;
    }
}

void free_score(Score *score) {

    Score *temp = score;
    if((temp -> flag) != VALID)
        free(temp -> code);

    free(score);
}

void free_all_scores(Score *score){

    Score *next = NULL;

    while(score != NULL){

        next = score -> next;
        free_score(score);
        score = next;
    }
}

void free_student(Student *student) {

    free(student -> id);
    free(student -> name);
    free(student -> surname);
    free_all_scores(student -> rawscores);
    free_all_scores(student -> normscores);
    free(student);
}

void free_all_students(Student *student) {

    Student *next = NULL;

    while(student != NULL){

        next = student -> next;
        free_student(student);
        student = next;
    }
}

void free_assistant(Assistant *assistant) {

    if(assistant != NULL){

        free(assistant -> name);
        free(assistant -> surname);
        free(assistant);
    }
}

void free_section(Section *section) {

    free(section -> name);
    free_all_students(section -> roster);
    free(section);
}

void free_all_sections(Section *section) {

    Section *next = NULL;

    while(section != NULL) {

        next = section -> next;
        free_section(section);
        section = next;

    }
}


void free_professor(Professor *professor) {

    free(professor -> name);
    free(professor -> surname);
    free(professor);
}


//STRUCTURES IN STATS.H

void free_freqs(Freqs *freqs) {

    free(freqs);
}

void free_all_freqs(Freqs *freqs) {

    Freqs *next = NULL;

    while(freqs != NULL){

        next = freqs -> next;
        free_freqs(freqs);
        freqs = next;
    }
}

void free_sectionStats(Sectionstats *sstats) {

    free(sstats);

}

void free_all_sectionStats(Sectionstats *sstats) {

    Sectionstats *next = NULL;

    while(sstats != NULL){

        next = sstats -> next;
        free_all_freqs(sstats -> freqs);
        free_sectionStats(sstats);
        sstats = next;
    }
}

void free_classStats(Classstats *cstats) {

    free(cstats);

}

void free_all_classStats(Classstats *cstats) {

    Classstats *next = NULL;

    while(cstats != NULL){

        next = cstats -> next;
        free_all_freqs(cstats -> freqs);
        free_all_sectionStats(cstats -> sstats);
        free_classStats(cstats);
        cstats = next;
    }
}

void free_stats(Stats *stats) {

    free_all_classStats(stats -> cstats);

    free(stats);

}


void free_course(Course *course, Stats *stats) {

    free(course -> number);
    free(course -> title);
    free_professor(course -> professor);
    free_all_assignments(course -> assignments);
    free_all_sections(course -> sections);
    free(course);
    free_stats(stats);

}