/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

#include <stdio.h>
#include <math.h>
#include "bottle.h"

int sort(const void *x, const void *y) {
  return (*(int*)x - *(int*)y);
}

testbottle ()
{
        sequencetype seq[MAXMACHS];
        motype mo;
        int i, j, try_count, try_values[MAXMACHS];

        if (!simplebottle) {
                printf ("Enter backtracking depth: ");
                fflush (stdout);
                scanf ("%d", &try_count);
                if (try_count > 0) {
                        printf ("Enter vector: ");
                        fflush (stdout);
                        for (i = 0; i < try_count; i++)
                        scanf ("%d", &try_values[i]);
                }
        } else
                try_count = 0;

        mo.size = 0;
        for (j = 0; j < nmachs; j++)
                mo.machs[j] = j;
		
		
		
        for (j = 0; j < nmachs; j++)
                for (i = 0; i < njobs; i++) {
                        seq[j].job[i] = -1; 
                        job[i].next[j] = -1;
                        job[i].prev[j] = -1;
                }

        evaluate (seq);
        bottler (seq, &mo, try_count, try_values);
        showsol (bestmake);
}

bottler (seq, mo, try_count, try_values)
sequencetype *seq;
motype *mo;
int try_count, *try_values;
{
        int bot, best, ntry, order[MAXMACHS];
        int i, j, next, bsize, make, first = 1;
        motype oldmo;
        blisttype blist[MAXMACHS];
        neighbor oldneighbor[MAXJOBS];
        onemach_times one;
        sequencetype oldseq[MAXMACHS];

        if (cycflag) {
                printf ("CYCFLAG\n");
                return;
        }

        bsize = 0;
        for (next = mo->size; next < nmachs; next++) {
                for (j = 0; j < njobs; j++) {
                        one.etime[j] = job[j].etime[mo->machs[next]];
                        one.proctime[j] = job[j].proctime[mo->machs[next]];
                        one.ftime[j] = job[j].ftime[mo->machs[next]];
                }
                blist[bsize].make = onemach (one, blist[bsize].order);
                blist[bsize].mach = next;
                bsize++;
        }

        if (mo->size < try_count)
                ntry = try_values[mo->size];
        else
                ntry = 1;

        copy_mo (&oldmo, mo);
        copy_seq (oldseq, seq);
        copy_neighbor (oldneighbor);

        while (ntry && bsize) {        
                best = 0;
                for (next = 1; next < bsize; next++)
                        if (blist[next].make > blist[best].make)
                                best = next;
                if (first && (mo->size == 0)) {
                        printf ("LOWER BOUND: %d\n", blist[best].make);
                        first = 0;
                }
                bot = mo->machs[blist[best].mach];
                mo->machs[blist[best].mach] = mo->machs[mo->size];
                mo->machs[mo->size] = bot;
                mo->size++;
                setseq (seq, bot, blist[best].order);
                make = evaluate (seq);
                localopt (seq, mo, &make);
                twiddle (seq, mo, &make);
                if (mo->size == nmachs) {
                        if (make < bestmake) {
                                bestmake = make;
                                printf ("NEW BEST: %3d  SEQ: ", bestmake);
                                for (j = 0; j < nmachs; j++)
                                        printf ("%2d ", mo->machs[j]);
                                printf ("\n");
                                save_etimes ();
                        } else if (make == bestmake) {
                                printf ("TIE BEST: %3d  SEQ: ", bestmake);
                                for (j = 0; j < nmachs; j++)
                                        printf ("%2d ", mo->machs[j]);
                                printf ("\n");
                        }
                } else if (make < bestmake)
                        bottler (seq, mo, try_count, try_values); 

                copy_mo (mo, &oldmo);
                copy_seq (seq, oldseq);
                restore_neighbor (oldneighbor);

                evaluate (seq);
                bsize--;        
                blist[best].mach = blist[bsize].mach;
                blist[best].make = blist[bsize].make;
                for (i = 0; i < njobs; i++)
                        blist[best].order[i] = blist[bsize].order[i];
                ntry--;
        }
}

localopt (seq, mo, make)
sequencetype *seq;
motype *mo;
int *make;
{
        int i, j, t, t1, flag;
        int improvement, newmake, oldmake;
        int tmake[MAXMACHS], oldorder[MAXJOBS], order[MAXJOBS];
        onemach_times one;

        oldmake = *make;
        do {
                newmake = oldmake;
                for (i = 0; i < mo->size; i++) {
                        clearseq (seq, mo->machs[i], oldorder);
                        evaluate (seq);
                        if (cycflag) {
                                setseq (seq, mo->machs[i], oldorder);
                                *make = evaluate (seq);
                                tmake[i] = 0;
                        } else {
                                for (j = 0; j < njobs; j++) {
                                        one.etime[j] =
                                                 job[j].etime[mo->machs[i]];
                                        one.proctime[j] =
                                                job[j].proctime[mo->machs[i]];
                                        one.ftime[j] =
                                                 job[j].ftime[mo->machs[i]];
                                }
                                tmake[i] = onemach (one, order);
                                setseq (seq, mo->machs[i], order);
                                *make = evaluate (seq);
                                if (*make < newmake) 
                                        newmake = *make;
                                else {
                                        setseq (seq, mo->machs[i], oldorder);
                                        *make = evaluate (seq);
                                }
                        }
                }        
                flag = 0;
                for (i = mo->size - 1; i > 0 && !flag; i--) {
                        flag = 1;
                        for (j = 0; j < i; j++) 
                                if (tmake[j] < tmake [j+1]) {
                                        flag = 0;
                                        t = tmake [j];
                                        tmake [j] = tmake [j+1];
                                        tmake [j+1] = t;
                                        t1 = mo->machs[j];
                                        mo->machs[j] = mo->machs[j+1];
                                        mo->machs[j+1] = t1;
                                }
                }
                improvement = oldmake - newmake;
                oldmake = newmake;
        } while (improvement > 0); 
}

twiddle (seq, mo, make)
sequencetype *seq;
motype *mo;
int *make;
{
        int oldmake, b, ndel, t, tmake, i, j;
        sequencetype oldseq[MAXMACHS];
        motype oldmo;
        neighbor oldneighbor[MAXJOBS];
        onemach_times one;
        int order[MAXJOBS];

        copy_mo (&oldmo, mo);
        copy_neighbor (oldneighbor);
        copy_seq (oldseq, seq);

        oldmake = *make;
        
        evaluate (seq);
        b = mo->size - (int) sqrt ((double) mo->size);
        for (i = oldmo.size - 1; i >= 0 && mo->size >= b; i--)
                if (!critical (mo->machs[i], *make)) {
                        clearseq (seq, mo->machs[i], (int *) NULL);
                        t = mo->machs[i];
                        for (j = i; j < mo->size - 1; j++)
                                mo->machs[j] = mo->machs[j+1];
                        mo->size--;
                        mo->machs[mo->size] = t;
                }
        *make = evaluate (seq);
        localopt (seq, mo, make);
        while (mo->size < oldmo.size) {
                if (cycflag) {
                        copy_mo (mo, &oldmo);
                        restore_neighbor (oldneighbor);
                        copy_seq (seq, oldseq);
                        *make = evaluate (seq);
                        return;
                }
                for (i = 0; i < njobs; i++) {
                        one.etime[i] = job[i].etime[mo->machs[mo->size]];
                       one.proctime[i] = job[i].proctime[mo->machs[mo->size]];
                        one.ftime[i] = job[i].ftime[mo->machs[mo->size]];
                }
                onemach (one, order);
                setseq (seq, mo->machs[mo->size], order);
                mo->size++;
                *make = evaluate (seq);
                localopt (seq, mo, make);
        }
        if (*make > oldmake) {
                copy_mo (mo, &oldmo);
                restore_neighbor (oldneighbor);
                copy_seq (seq, oldseq);
                *make = evaluate (seq);
        }
}

critical (mach, make)
int mach, make;
{
        int i;

        for (i = 0; i < njobs; i++) 
                if (job[i].etime[mach] + job[i].proctime[mach]
                                       + job[i].ftime[mach] == make)
                return 1;
        return 0;
}

copy_mo (new, original)
motype *new, *original;
{
        int j;

        new->size = original->size;
        for (j = 0; j < nmachs; j++)
                new->machs[j] = original->machs[j];
}

copy_seq (new, original)
sequencetype *new, *original;
{
        int i, j;

        for (j = 0; j < nmachs; j++)
                for (i = 0; i < njobs; i++)
                        new[j].job[i] = original[j].job[i];
} 

copy_neighbor (new)
neighbor *new;
{
        int i, j;
  
        for (i = 0; i < njobs; i++)
                for (j = 0; j < nmachs; j++) {
                        new[i].next[j] = job[i].next[j];
                        new[i].prev[j] = job[i].prev[j];
                }
}

save_etimes ()
{
        int i, j;

        for (i = 0; i < njobs; i++)
                for (j = 0; j < nmachs; j++)
                        job[i].start[j] = job[i].etime[j];
}

restore_neighbor (old)
neighbor *old;
{
        int i, j;

        for (i = 0; i < njobs; i++)
                for (j = 0; j < nmachs; j++) {
                        job[i].next[j] = old[i].next[j];
                        job[i].prev[j] = old[i].prev[j];
                }
}

trivial_lowerbound ()
{
        onemach_times one;
        sequencetype seq[MAXJOBS];
        int i, j, order[MAXJOBS], best = -1, val;

        for (j = 0; j < nmachs; j++)
                for (i = 0; i < njobs; i++)
                        seq[j].job[i] = -1;
        for (i = 0; i < njobs; i++)
                for (j = 0; j < nmachs; j++)
                        job[i].prev[j] = job[i].next[j] = -1;

        i = evaluate (seq);
        showtimes ();

        for (i = 0; i < nmachs; i++) {
                for (j = 0; j < njobs; j++) {
                        one.etime[j] = job[j].etime[i];
                        one.proctime[j] = job[j].proctime[i];
                        one.ftime[j] = job[j].ftime[i];
                }
                val = onemach (one, order);
                if (val > best)
                        best = val;
        }
        printf ("Trivial lowerbound: %d\n", best);
}

                

getprob (fname)
char *fname;
{
        FILE *in;
        int i, j, ptime, machine;

        if ((in = fopen (fname, "r")) == NULL) {
                fprintf (stderr, "Unable to open %s for input\n", fname);
                exit (1);
        }

        fgets (probname, 100, in);
        fscanf (in, "%d %d", &njobs, &nmachs);

        for (i = 0; i < njobs; i++)
                for (j = 0; j < nmachs; j++) {
                        fscanf (in, "%d %d", &machine, &ptime);
                        job[i].order[j] = machine;
                        job[i].proctime[machine] = ptime;
                        job[i].step[machine] = j;
                }
        fclose (in);
}

showprob ()
{
        int i, j, t;

        printf ("%s", probname);
        for (i = 0; i < njobs; i++) {
                for (j = 0; j < nmachs; j++) {
                        t = job[i].order[j];
                        printf ("%2d %3d ", job[i].order[j],
                                            job[i].proctime[t]);
                }
                printf ("\n");
        }
}


showsol (bestmake)
int bestmake;
{
        int i, j;
		
		FILE *out;
		out=fopen("bottle.txt", "w");
		
		
        printf ("OBJECTIVE VLAUE: %d\n", bestmake);
        printf ("TABLE OF STARTING TIMES:\n");
        for (i = 0; i < njobs; i++) {
                printf ("JOB %d  ", i);
                for (j = 0; j < nmachs; j++){
					
                        printf ("%3d ", job[i].start[j]);
						
						fprintf(out, "Job%d\t%d\t%d\tMachine%d\n", i, job[i].start[j], job[i].start[j]+job[i].proctime[j], j);
				}
                printf ("\n");
        }
        
        FILE *out2;
		out2=fopen("bottle_mach.txt", "w");
		int startarray[njobs], waste, k;
		int endarray[njobs], starttime, endtime, wp1=0, wp2=0;
		float percentage, percentage2, wper1=0, wper2=0;
		
		
        printf ("OBJECTIVE VLAUE: %d\n", bestmake);
        printf ("TABLE OF STARTING TIMES:\n");
        for (i = 0; i < nmachs; i++) {
                printf ("MACHINE %d  ", i);
                for (j = 0; j < njobs; j++){
					
                        printf ("%3d ", job[j].start[i]);
						startarray[j] = job[j].start[i];
						endarray[j] = job[j].start[i]+job[j].proctime[i];
						
						fprintf(out2, "Machine%d\t%d\t%d\tJob%d\n", i, job[j].start[i], job[j].start[i]+job[j].proctime[i], j);
				}
				
				
				qsort(startarray, njobs, sizeof(int), sort);
				qsort(endarray, njobs, sizeof(int), sort);
				
				waste = 0;
				
				for(k=0; k<njobs-1; k++)
					waste += startarray[k+1] - endarray[k];
				
				
				starttime = startarray[0];
				endtime = endarray[njobs-1];
				
				percentage = 100 - ((float)waste / (endtime-starttime)) * 100;
				percentage2 = 100 - ((float)(waste+starttime) / (endtime)) * 100;
				
				printf("Per2 : %0.2f Per : %0.2f Start : %d End : %d\n", percentage2, percentage, starttime, endtime);
				
				wper1 += percentage*(endtime-starttime);
				wp1 += (endtime-starttime);
				wper2 += percentage2*(endtime);
				wp2 += endtime;
				
        }
		
		printf("Weighted per2 : %0.2f, Weighted per1 : %0.2f\n", wper1/wp1, wper2/wp2);
		
		printf("\nPower : %5d\n", wp1);
		
		FILE *in2;
        
        if ((in2 = fopen ("fcfs_stats.txt", "r")) == NULL) {
                fprintf (stderr, "Unable to open file for input\n");
                exit (1);
        }
        
        int fcfs_time, fcfs_power, imp_time2, imp_power2;
		float imp_time, imp_power;
        
        fscanf(in2, "%d %d", &fcfs_time, &fcfs_power);
        
		imp_time2 = fcfs_time - bestmake;
		imp_power2 = fcfs_power - wp1;
		
		imp_time = (float)(fcfs_time - bestmake)*100 / fcfs_time;
		imp_power = (float)(fcfs_power - wp1)*100 / fcfs_power;
		
		printf("Improvement in timespan : %0.2f | %5d\nImprovement in power : %0.2f | %5d\n", imp_time, imp_time2, imp_power, imp_power2);
		
		FILE *out3;
        
        if ((out3 = fopen ("statistics.txt", "w")) == NULL) {
                fprintf (stderr, "Unable to open file for input\n");
                exit (1);
        }
        
        fprintf(out3, "Dataset used : \n\n");
		int t; 
		fprintf (out3, "%s", probname);
        for (i = 0; i < njobs; i++) {
                for (j = 0; j < nmachs; j++) {
                        t = job[i].order[j];
                        fprintf (out3, "%2d %3d ", job[i].order[j],
                                            job[i].proctime[t]);
                }
               fprintf (out3, "\n");
        }

		fprintf(out3, "\nJobs : %d, Machines : %d\n", njobs, nmachs);
		fprintf(out3, "\nTimespan :\tFCFS:%5d\tBN:%5d\tDiff:%5d\tDiff (\%):%0.2f\n", fcfs_time, bestmake, imp_time2, imp_time);
        fprintf(out3, "Power :\t\tFCFS:%5d\tBN:%5d\tDiff:%5d\tDiff (\%):%0.2f\n", fcfs_power, wp1, imp_power2, imp_power);
		fprintf(out3, "Power2 : %4d\n", imp_time2 * nmachs);
			
}
        
        

writesol ()
{
        int i, j;
        FILE *out;

        if ((out = fopen (etimesfilename, "w")) == NULL) {
                fprintf (stderr, "Unable to open %s for output\n",
                         etimesfilename);
                exit (1);
        }
        fprintf (out, "%2d %2d\n", njobs, nmachs);
        for (i = 0; i < njobs; i++) {
                for (j = 0; j < nmachs; j++)
                        fprintf (out, "%3d ", job[i].start[j]);
                fprintf (out, "\n");
        }
        fclose (out);
}        

showtimes ()
{
        int i, j;

        printf ("ETIMES: \n");
        for (i = 0; i < njobs; i++) {
                for (j = 0; j < nmachs; j++)
                        printf ("%5d ", job[i].etime[j]);
                printf ("\n");
        }
        printf ("FTIMES:\n");
        for (i = 0; i < njobs; i++) {
                for (j = 0; j < nmachs; j++)
                        printf ("%5d ", job[i].ftime[j]);
                printf ("\n");
        }
}

showorder (order)
int *order;
{
        int i;

        printf ("ORDER: ");
        for (i = 0; i < njobs; i++)
                printf ("%d ", order[i]);
}

showneighbors ()
{
        int i, j;
        
        printf ("NEIGHBORS:\n");
        for (i = 0; i < njobs; i++) {
                printf ("JOB %d: ", i);
                for (j = 0; j < nmachs; j++)
                        printf ("%2d %2d  ", job[i].next[j], job[i].prev[j]);
                printf ("\n");
        }
}

showseq (seq)
sequencetype *seq;
{
        int i, j;

        printf ("SEQUENCES:\n");
        for (j = 0; j < nmachs; j++) {
                printf ("MACH %d: ", j);
                for (i = 0; i < njobs; i++)
                        printf ("%d ", seq[j].job[i]);
                printf ("\n");
        }
}

setupnames (n1, n2)
char *n1;
char *n2;
{
        char *p;
        char *strrchr();
 
        if (n2)
                p = n2;
        else {
                p = strrchr(n1, '/');
                if (!p) p = n1;
                else p++;
        }
        sprintf (etimesfilename, "etimes.out.%s",p);
}
