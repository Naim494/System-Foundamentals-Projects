
/*
 * Normalize scores, using the computed statistics.
 */

#include<stddef.h>
#include<stdio.h>
#include "global.h"
#include "gradedb.h"
#include "stats.h"
#include "allocate.h"
#include "normal.h"

/*
 * Normalize scores:
 *      For each student in the course roster,
 *       for each score for that student,
 *              compute the normalized version of that score
 *              according to the substitution and normalization
 *              options set for that score and for the assignment.
 */

void normalize(c, s)
Course *c;
Stats *s;
{
        Student *stp;
        Score *rscp, *nscp;
        Classstats *csp;
        Sectionstats *ssp;

        for(stp = c->roster; stp != NULL; stp = stp->cnext) {
           stp->normscores = nscp = NULL;
           for(rscp = stp->rawscores; rscp != NULL; rscp = rscp->next) {
              csp = rscp->cstats;
              ssp = rscp->sstats;
              if(nscp == NULL) {
                stp->normscores = nscp = newscore();
                nscp->next = NULL;
              } else {
                nscp->next = newscore();
                nscp = nscp->next;
                nscp->next = NULL;
              }
              nscp->asgt = rscp->asgt;
              nscp->flag = rscp->flag;
              nscp->subst = rscp->subst;
              if(rscp->flag == VALID) {
                nscp->grade = normal(rscp->grade, csp, ssp);
              } else {
                switch(rscp->subst) {
                case USERAW:
                        nscp->grade = normal(rscp->grade, csp, ssp);
                        break;
                case USENORM:
                        if(rscp->asgt->npolicy == QUANTILE)
                                nscp->grade = rscp->qnorm;
                        else
                                nscp->grade = rscp->lnorm;
                        break;
                case USELIKEAVG:
                        nscp->grade = studentavg(stp, csp->asgt->atype);
                        break;
                case USECLASSAVG:
                        if(rscp->asgt->npolicy == QUANTILE)
                                nscp->grade = 50.0;
                        else
                                nscp->grade = rscp->asgt->mean;
                        break;
                }
              }
           }
        }
}

/*
 * Normalize a raw score according to the normalization policy indicated.
 */

float normal(s, csp, ssp)
double s;
Classstats *csp;
Sectionstats *ssp;
{
        Assignment *a;
        Freqs *fp;
        int n;

        a = csp->asgt;
        switch(a->npolicy) {
        case RAW:
                return(s);
        case LINEAR:
                switch(a->ngroup) {
                case BYCLASS:
                        if(csp->stddev < EPSILON) {
                           warning("Std. dev. of %s too small for normalization.",
                                 csp->asgt->name);
                           csp->stddev = 2*EPSILON;
                         }
                        return(linear(s, csp->mean, csp->stddev, a->mean, a->stddev));
                case BYSECTION:
                        if(ssp->stddev < EPSILON) {
                           warning("Std. dev. of %s, section %s too small for normalization.",
                                 ssp->asgt->name, ssp->section->name);
                           ssp->stddev = 2*EPSILON;
                         }
                        return(linear(s, ssp->mean, ssp->stddev, a->mean, a->stddev));
                }
        case SCALE:
                if(a->max < EPSILON) {
                  warning("Declared maximum score of %s too small for normalization.",
                        csp->asgt->name);
                  a->max = 2*EPSILON;
                }
                return(scale(s, a->max, a->scale));
        case QUANTILE:
                switch(a->ngroup) {
                case BYCLASS:
                        fp = csp->freqs;
                        n = csp->tallied;
                        if(n == 0) {
                           warning("Too few scores in %s for quantile normalization.",
                                   csp->asgt->name);
                           n = 1;
                         }
                        break;
                case BYSECTION:
                        fp = ssp->freqs;
                        n = ssp->tallied;
                        if(n == 0) {
                           warning("Too few scores in %s, section %s for quantile normalization.",
                                 ssp->asgt->name, ssp->section->name);
                           n = 1;
                         }
                        break;
                }
                /*
                 * Look for score s in the frequency tables.
                 * If found, return the corresponding percentile score.
                 * If not found, then use the percentile score corresponding
                 * to the greatest valid score in the table that is < s.
                 */

                for( ; fp != NULL; fp = fp->next) {
                   if(s < fp->score)
                        return((float)fp->numless*100.0/n);
                   else if(s == fp->score)
                        return((float)fp->numless*100.0/n);
                }
        }
}

/*
 * Perform a linear transformation to convert score s,
 * with sample mean rm and sample standard deviation rd,
 * to a normalized score with normalized mean nm and
 * normalized standard deviation nd.
 *
 * It is assumed that rd is not too small.
 */

float linear(s, rm, rd, nm, nd)
double s, rm, rd, nm, nd;
{
        return(nd*(s-rm)/rd + nm);
}

/*
 * Scale normalization rescales the score to a given range [0, scale]
 *
 * It is assumed that the declared max is not too small.
 */

float scale(s, max, scale)
double s, max, scale;
{
        return(s*scale/max);
}

/*
 * Compute a student's average score on all assignments of a given type.
 * If a weighting policy is set, we use the specified relative weights,
 * otherwise all the assignments of the type are weighted equally.
 */

float studentavg(s, t)
Student *s;
Atype t;
{
        int n, wp;
        double sum;
        Score *scp;
        float f, w;

        n = 0;
        wp = 0;
        sum = 0.0;
        w = 0.0;
        for(scp = s->rawscores; scp != NULL; scp = scp->next) {
           if(!strcmp(scp->asgt->atype, t) &&
              (scp->flag == VALID || scp->subst == USERAW)) {
                n++;
                f = normal(scp->grade, scp->cstats, scp->sstats);
                if(scp->asgt->wpolicy == WEIGHT) {
                   wp = 1;
                   sum += f * scp->asgt->weight;
                   w += scp->asgt->weight;
                } else {
                   sum += f;
                }
           }
        }
        if(n == 0 || w == 0.0) {
                warning("Student %s %s has no like scores to average,\n%s",
                        s->name, s->surname, "using raw 0.0.");
                return(0.0);
        } else {
                if(wp) return(sum/w);
                else return(sum/n);
        }
}

/*
 *  Compute composite scores:
 *      For each student in the course roster,
 *        For each assignment,
 *         Find the student's normalized score for that assignment,
 *              and include it in the weighted sum.
 */

void composites(c)
Course *c;
{
        Student *stp;
        Score *scp;
        Assignment *ap;
        float sum;
        int found;

        for(stp = c->roster; stp != NULL; stp = stp->cnext) {
           sum = 0.0;
           for(ap = c->assignments; ap != NULL; ap = ap->next) {
              found = 0;
              for(scp = stp->normscores; scp != NULL; scp = scp->next) {
                if(scp->asgt == ap) {
                   found++;
                   sum += scp->grade * (ap->wpolicy == WEIGHT? ap->weight: 1.0);
                }
              }
              if(!found) {
                warning("Student %s %s has no score for assignment %s.",
                        stp->name, stp->surname, ap->name);
              }
           }
           stp->composite = sum;
        }
}
