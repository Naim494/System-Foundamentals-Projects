#include <stdio.h>
#include <criterion/criterion.h>
#include "gradedb.h"
#include "read.h"
#include "write.h"
#include "sort.h"
#include "stats.h"
#include "normal.h"
#include "report.h"

#define TEST_FILE "cse307.dat"
#define COLLATED_REF "rsrc/cse307.collated"
#define TABSEP_REF "rsrc/cse307.tabsep"
#define COLLATED_OUTPUT "cse307.collated"
#define TABSEP_OUTPUT "cse307.tabsep"

extern int errors, warnings;

Test(basic_suite, read_file_test) {
    Course *c;
    c = readfile(TEST_FILE);
    cr_assert_eq(errors, 0, "There were errors reported when reading test data.\n");
    cr_assert_neq(c, NULL, "NULL pointer returned from readfile().\n");
}

Test(basic_suite, stats_test) {
    Course *c;
    Stats *s;
    c = readfile(TEST_FILE);
    cr_assert_eq(errors, 0, "There were errors reported when reading test data.\n");
    cr_assert_neq(c, NULL, "NULL pointer returned from readfile().\n");
    s = statistics(c);
    cr_assert_neq(s, NULL, "NULL pointer returned from statistics().\n");
}

Test(basic_suite, collate_test) {
    Course *c;
    c = readfile(TEST_FILE);
    cr_assert_eq(errors, 0, "There were errors reported when reading test data.\n");
    cr_assert_neq(c, NULL, "NULL pointer returned from readfile().\n");
    FILE *f = fopen(COLLATED_OUTPUT, "w");
    cr_assert_neq(f, NULL, "Error opening test output file.\n");
    statistics(c);
    sortrosters(c, comparename);
    writecourse(f, c);
    fclose(f);
    char cmd[100];
    sprintf(cmd, "cmp %s %s", COLLATED_OUTPUT, COLLATED_REF);
    int err = system(cmd);
    cr_assert_eq(err, 0, "Output file doesn't match reference output.\n");
}

Test(basic_suite, tabsep_test) {
    Course *c;
    Stats *s;
    c = readfile(TEST_FILE);
    cr_assert_eq(errors, 0, "There were errors reported when reading test data.\n");
    cr_assert_neq(c, NULL, "NULL pointer returned from readfile().\n");
    s = statistics(c);
    cr_assert_neq(s, NULL, "NULL pointer returned from statistics().\n");
    normalize(c);
    composites(c);
    sortrosters(c, comparename);
    FILE *f = fopen(TABSEP_OUTPUT, "w");
    cr_assert_neq(f, NULL, "Error opening test output file.\n");
    reporttabs(f, c);
    fclose(f);
    char cmd[100];
    sprintf(cmd, "cmp %s %s", TABSEP_OUTPUT, TABSEP_REF);
    int err = system(cmd);
    cr_assert_eq(err, 0, "Output file doesn't match reference output.\n");
}

//############################################
// STUDENT UNIT TESTS SHOULD BE WRITTEN BELOW
// DO NOT DELETE THESE COMMENTS
//############################################

