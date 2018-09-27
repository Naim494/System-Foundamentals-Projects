
/*
 * Type definitions for normalization functions.
 */

void normalize(Course *c, Stats *s);
float normal(double s, Classstats *csp, Sectionstats *ssp);
float linear(double s, double rm, double rd, double nm, double nd);
float scale(double s, double max, double scale);
float studentavg(Student *s, Atype t);
void composites(Course *c);
