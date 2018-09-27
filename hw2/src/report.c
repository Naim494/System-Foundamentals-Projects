
/*
 * Generate reports from the computed data.
 */

#include <stdio.h>
#include "global.h"
#include "gradedb.h"
#include "stats.h"

#ifdef MSDOS
#include <time.h>
#else
#include <sys/types.h>
#include <time.h>
#endif

reportparams(fd, fn, c)
FILE *fd;
char *fn;
Course *c;
{
        char *today = NULL;
        time_t now;
        time(&now);
        today = ctime(&now);

        fprintf(fd, "GRADES FOR: %s\n", c->title);
        if(c->professor)
                fprintf(fd, "PROFESSOR : %s %s\n",
                        c->professor->name, c->professor->surname);
        fprintf(fd, "DATA FILE : %s\n", fn);
        fprintf(fd, "RUN DATE  : %s\n", today);
        fprintf(fd, "\n");
}

reportfreqs(fd, s)
FILE *fd;
Stats *s;
{
        Classstats *csp;
        Sectionstats *ssp;
        Freqs *fp;
        int i;

        fprintf(fd, "FREQUENCY TABLES\n\n");
        for(csp = s->cstats; csp != NULL; csp = csp->next) {
           fprintf(fd, "%s\n(whole class, %d tallied):\n", csp->asgt->name, csp->tallied);
           i = 1;
           fprintf(fd, "   ");
           for(fp = csp->freqs; fp != NULL; fp = fp->next) {
              if(i % 7 == 0) {
                i = 1;
                fprintf(fd, "\n   ");
              }
              i++;
              fprintf(fd, "(%6.2f,%3d)", fp->score, fp->count);
           }
           fprintf(fd, "\n");
           if(csp->sstats != NULL && csp->sstats->next == NULL)
                { fprintf(fd, "\n"); continue; }  /* Only one section */
           for(ssp = csp->sstats; ssp != NULL; ssp = ssp->next) {
              fprintf(fd, "(section %s, %d tallied):\n", ssp->section->name, ssp->tallied);
              i = 1;
              fprintf(fd, "   ");
              for(fp = ssp->freqs; fp != NULL; fp = fp->next) {
                 if(i % 7 == 0) {
                    i = 1;
                    fprintf(fd, "\n   ");
                 }
                 i++;
                 fprintf(fd, "(%6.2f,%3d)", fp->score, fp->count);
              }
              fprintf(fd, "\n");
           }
           fprintf(fd, "\n");
        }
        fprintf(fd, "\n");
}

float interpolatequantile(fp, n, q)
Freqs *fp;
int n;
float q;
{
  float pq, nq, ps, ns, qdiff, sdiff, s;

  if(fp == NULL)
    return 0.0;
  pq = nq = 0.0;
  ps = ns = fp->score;
  while(fp != NULL && fp->next != NULL) {
    nq = fp->numless * 100.0/n;
    ns = fp->score;
    if(pq <= q && q < nq)
      break;
    pq = nq;
    ps = ns;
    fp = fp->next;
  }
  if(fp->next == NULL) {
    return fp->score;
  }
  qdiff = nq - pq;
  sdiff = ns - ps;
  if(qdiff == 0.0)
    return ps;
  else {
    s = ps + (sdiff/qdiff)*(q - pq);
    return s;
  }
}

float quantiles[] = { 10.0, 25.0, 50.0, 75.0, 90.0 };
float scores[]    = {  0.0,  0.0,  0.0,  0.0,  0.0 };

reportquantilesummaries(fd, s)
FILE *fd;
Stats *s;
{
  Classstats *csp;
  Sectionstats *ssp;
  int i, n;
  float q, r, min, max;

  fprintf(fd, "QUANTILE SUMMARIES\n\n");
        for(csp = s->cstats; csp != NULL; csp = csp->next) {
           fprintf(fd, "%s\n(whole class, %d tallied):\n", csp->asgt->name, csp->tallied);
           n = sizeof(quantiles)/sizeof(*quantiles);
           for(i = 0; i < n; i++)
             scores[i] = interpolatequantile(csp->freqs, csp->tallied,
                                             quantiles[i]);
           min = scores[0];
           max = scores[n-1];
           for(i = 0; i < sizeof(quantiles)/sizeof(*quantiles); i++) {
             fprintf(fd, "   ");
             fprintf(fd, "%6.2f  ", quantiles[i]);
             fprintf(fd, "%6.2f  ", scores[i]);
             if(min < max) {
               r = (scores[i] - min) / (max - min);
               fprintf(fd, "%6.2f", r);
             } else {
               fprintf(fd, "***.**", r);
             }
             fprintf(fd, "\n");
           }
           fprintf(fd, "\n");
           if(csp->sstats != NULL && csp->sstats->next == NULL)
                { fprintf(fd, "\n"); continue; }  /* Only one section */
           for(ssp = csp->sstats; ssp != NULL; ssp = ssp->next) {
              fprintf(fd, "(section %s, %d tallied):\n", ssp->section->name, ssp->tallied);
              for(i = 0; i < n; i++)
                scores[i] = interpolatequantile(ssp->freqs, ssp->tallied,
                                                quantiles[i]);
              min = scores[0];
              max = scores[n-1];
              for(i = 0; i < sizeof(quantiles)/sizeof(*quantiles); i++) {
                fprintf(fd, "   ");
                fprintf(fd, "%6.2f  ", quantiles[i]);
                fprintf(fd, "%6.2f  ", scores[i]);
                if(min < max) {
                  r = (scores[i] - min) / (max - min);
                  fprintf(fd, "%6.2f", r);
                } else {
                  fprintf(fd, "***.**", r);
                }
                fprintf(fd, "\n");
              }
              fprintf(fd, "\n");
           }
           fprintf(fd, "\n");
        }
        fprintf(fd, "\n");
}

reportquantiles(fd, s)
FILE *fd;
Stats *s;
{
        Classstats *csp;
        Sectionstats *ssp;
        Freqs *fp;
        int i;

        fprintf(fd, "QUANTILE DATA\n\n");
        for(csp = s->cstats; csp != NULL; csp = csp->next) {
           fprintf(fd, "%s\n(whole class, %d tallied):\n", csp->asgt->name, csp->tallied);
           i = 1;
           fprintf(fd, "   ");
           for(fp = csp->freqs; fp != NULL; fp = fp->next) {
              if(i % 6 == 0) {
                i = 1;
                fprintf(fd, "\n   ");
              }
              i++;
              if(csp->tallied) {
                fprintf(fd, "(%6.2f,%6.2f)", fp->score,
                        (float)fp->numless * 100.0/csp->tallied);
              } else {
                fprintf(fd, "(%6.2f,***.**)", fp->score,
                        (float)fp->numless * 100.0/csp->tallied);
              }
           }
           fprintf(fd, "\n");
           if(csp->sstats != NULL && csp->sstats->next == NULL)
                { fprintf(fd, "\n"); continue; }  /* Only one section */
           for(ssp = csp->sstats; ssp != NULL; ssp = ssp->next) {
              fprintf(fd, "(section %s, %d tallied):\n", ssp->section->name, ssp->tallied);
              i = 1;
              fprintf(fd, "   ");
              for(fp = ssp->freqs; fp != NULL; fp = fp->next) {
                 if(i % 6 == 0) {
                    i = 1;
                    fprintf(fd, "\n   ");
                 }
                 i++;
                 if(ssp->tallied) {
                    fprintf(fd, "(%6.2f,%6.2f)", fp->score,
                            (float)fp->numless * 100.0/ssp->tallied);
                 } else {
                    fprintf(fd, "(%6.2f,***.**)", fp->score,
                            (float)fp->numless * 100.0/ssp->tallied);
                 }
              }
              fprintf(fd, "\n");
           }
           fprintf(fd, "\n");
        }
        fprintf(fd, "\n");
}

reportmoments(fd, s)
FILE *fd;
Stats *s;
{
        Classstats *csp;
        Sectionstats *ssp;

        fprintf(fd, "ASSIGNMENT STATISTICS\n\n");
        for(csp = s->cstats; csp != NULL; csp = csp->next) {
           fprintf(fd, "%-12s (whole class     ) ", csp->asgt->name);
           fprintf(fd, "Valid %3d; Mean %6.2f; Std. Dev. %6.2f\n",
                   csp->valid,
                   (float)csp->mean, (float)csp->stddev);
           if(csp->sstats != NULL && csp->sstats->next == NULL)
                { continue; }  /* Only one section */
           for(ssp = csp->sstats; ssp != NULL; ssp = ssp->next) {
              fprintf(fd, "%-12s (section %8s) ",
                          csp->asgt->name, ssp->section->name);
                 fprintf(fd, "Valid %3d; Mean %6.2f; Std. Dev. %6.2f\n",
                         ssp->valid,
                         (float)ssp->mean, (float)ssp->stddev);
           }
           fprintf(fd, "\n");
        }
        fprintf(fd, "\n");
}

reportscores(fd, c, nm)
FILE *fd;
Course *c;
{
        Assignment *ap;
        Student *stp;
        Score *rscp, *nscp;

        fprintf(fd, "STUDENT INDIVIDUAL SCORES\n\n");
        for(stp = c->roster; stp != NULL; stp = stp->cnext) {
           fprintf(fd, "%-16.16s%c  %-16.16s (%-12.12s, Section %-8.8s)\n",
                       nm ? "" : stp->surname, nm ? ' ' : ',',
                       nm ? "" : stp->name, stp->id, stp->section->name);
           fprintf(fd, "Composite score: %6.2f\n", stp->composite);
           nscp = stp->normscores;
           fprintf(fd, "\n   %-13.13s  %6.6s  %6.6s  %8.8s  %8.8s  %-s\n",
                "Assignment", "Raw", "Norm", "ClassAvg", "SectAvg", "Note");
           for(rscp = stp->rawscores; rscp != NULL; rscp = rscp->next) {
              fprintf(fd, "   %-13.13s", rscp->asgt->name);
              if(rscp->flag == VALID || rscp->subst == USERAW)
                fprintf(fd, "  %6.2f", rscp->grade);
              else fprintf(fd, "  ***.**");
              fprintf(fd, "  %6.2f", nscp->grade);
              fprintf(fd, "    %6.2f", rscp->cstats->mean);
              fprintf(fd, "    %6.2f", rscp->sstats->mean);
              if(rscp->flag == INVALID) fprintf(fd, "  %-s", rscp->code);
              fprintf(fd, "\n");
              nscp = nscp->next;
           }
              fprintf(fd, "\n");
        }
        fprintf(fd, "\n");
}

reportcomposites(fd, c, nm)
FILE *fd;
Course *c;
{
        Student *stp;
        Score *scp;
        Assignment *ap;

        fprintf(fd, "STUDENT COMPOSITE SCORES\n\n");
        for(stp = c->roster; stp != NULL; stp = stp->cnext) {
          fprintf(fd, "%6.2f %-16.16s%c %-16.16s (%-12.12s, Section %-8.8s)\n",
                   stp->composite, nm ? "" : stp->surname, nm ? ' ' : ',',
                   nm ? "" : stp->name, stp->id, stp->section->name);
        }
        fprintf(fd, "\n");
}

/*
 * Print histograms of the assignment data so that each histogram fits
 * in a 20x50 character region.  This is done by using the frequency data,
 * working in bins of width dependent on the maximum score, and normalizing
 * the vertical axis to a scale of 0 to 20.
 */

reporthistos(fd, c, s)
FILE *fd;
Course *c;
Stats *s;
{
        Classstats *csp;
        Sectionstats *ssp;
        Student *stp;
        Freqs *fp;
        int col, pct, cnt;
        int bins[50];
        float min, max, diff;

        fprintf(fd, "HISTOGRAMS\n\n");
        /*
         * First do composite scores.
         */
        fprintf(fd, "Composite Scores:\n");
        /*
         * Tally the scores into bins
         */
        min = max = 0.0;
        cnt = 0;
        for(stp = c->roster; stp != NULL; stp = stp->cnext) {
          if(stp->composite < min) min = stp->composite;
          if(stp->composite > max) max = stp->composite;
          cnt++;
        }
        for(col = 0; col < 50; col++) bins[col] = 0;
        diff = (max-min == 0.0) ? 1.0 : (max-min);
        for(stp = c->roster; stp != NULL; stp = stp->cnext) {
          pct = 49*(stp->composite-min)/diff;
          bins[pct] += 1;
        }
        /*
         * Print histogram.
         */
        histo(fd, bins, min, max, cnt);
        /*
         * Now do assignment scores.
         */
        for(csp = s->cstats; csp != NULL; csp = csp->next) {
           fprintf(fd, "%s (whole class, %d tallied):\n",
                   csp->asgt->name, csp->tallied);
           /*
            * Tally the scores into bins.
            */
           if(csp->asgt->max == 0.0) {
                if(csp->max == 0.0) max = 1.0;
                else max = csp->max;
           } else max = csp->asgt->max;
           for(col = 0; col < 50; col++) bins[col] = 0;
           for(fp = csp->freqs; fp != NULL; fp = fp->next) {
                if(fp->score > max) pct = 49;
                else pct = 49*fp->score/max;
                bins[pct] += fp->count;
           }
           /*
            * Print histogram
            */
           histo(fd, bins, 0.0, max, csp->tallied);
        }
}

histo(fd, bins, min, max, cnt)
FILE *fd;
int bins[50], cnt;
float min, max;
{
    int row, col, pct, cmax;
    /*
     * Determine the bin with the most tallies.
     */
    if(cnt < EPSILON) {
      cnt = 1;  /* Don't divide by zero */
    }
    cmax = 0;
    for(col = 0; col < 50; col++)
      if(bins[col] > cmax) cmax = bins[col];
    /*
     * Now display the histogram.
     */
    for(row = 20; row >= 0; row--) {
      if(row == 20)
        fprintf(fd, "        ");
      else if(row%4 == 3) {
        fprintf(fd, "%5.1f%% |", (float)(100*cmax/cnt)*(row+1)/20);
      } else {
        fprintf(fd, "       |");
      }
      for(col = 0; col < 50; col++) {
        if(20*bins[col] > row*cmax)
          fprintf(fd, "%s", (row==20)?"^":"*");
        else
          fprintf(fd, " ");
      }
      fprintf(fd, "\n");
    }
    fprintf(fd, "    0%% -+------------------------------------------------+\n");
    fprintf(fd, "     %6.2f                                         %6.2f\n\n",
            min, max);
}    

void
reporttabs(FILE *fd, Course *c, int nm)
{
        Assignment *ap;
        Student *stp;
        Score *rscp, *nscp;

        fprintf(fd, "STUDENT INDIVIDUAL SCORES\n\n");
        fprintf(fd, "STUDENT\t");
        for(ap = c->assignments; ap != NULL; ap = ap->next)
          fprintf(fd, "%s\t", ap->name);
        fprintf(fd, "COMPOSITE\n");
        for(stp = c->roster; stp != NULL; stp = stp->cnext) {
           fprintf(fd, "%s\t", stp->id);
           for(ap = c->assignments; ap != NULL; ap = ap->next) {
             for(rscp = stp->rawscores; rscp != NULL; rscp = rscp->next) {
               if(rscp->asgt == ap) {
                 if(rscp->flag == VALID || rscp->subst == USERAW)
                   fprintf(fd, "%6.2f\t", rscp->grade);
                 else fprintf(fd, "***.**\t");
                 goto next;
               }
             }
             fprintf(fd, "   ***.**\t");
           next:
             continue;
           }
           fprintf(fd, "%6.2f\n", stp->composite);
        }
        fprintf(fd, "\n");
}
