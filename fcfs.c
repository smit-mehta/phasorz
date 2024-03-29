
#include <stdio.h>
#include <malloc.h>

#define MAXJOBS 30
#define MAXMACHS 30

typedef struct jobtype {
        int     order[MAXMACHS];
        int     proctime[MAXMACHS];
                
} jobtype;

void updatejob(int machine_no1, int machine_no2, int** machinejob, int** machinetime, int* cycle, int ***ans, int time, int njobs, int nmachs, jobtype* job, int** allotted)
{
	int i;
	if (machinejob[machine_no1][machine_no2] >= 0 && cycle[machinejob[machine_no1][machine_no2]] != nmachs)
		cycle[machinejob[machine_no1][machine_no2]]++;
	
	for(i=0; i<njobs; i++)
		//printf("+ %d ", cycle[i]);
	
	//printf("\n");
	
	for(i=0; i<njobs; i++)
	{
		if (cycle[i] == nmachs)
			continue;
		if (job[i].order[cycle[i]] == machine_no1 && allotted[i][cycle[i]] == 0)
		{
			machinetime[machine_no1][machine_no2] = job[i].proctime[machine_no1];
			machinejob[machine_no1][machine_no2] = i;
			allotted[i][cycle[i]] = 1;
			ans[i][machine_no1][machine_no2] = time;
		//	printf("*Job : %d Machine %d Part time %d Time %d\n", i, machine_no1, job[i].proctime[machine_no1], time);
			return;
		}
	}
	
	machinejob[machine_no1][machine_no2] = -1;
}
			
int check_over(int* cycle, int njobs, int nmachs)
{
	int i;
	
	for(i=0; i<njobs; i++)
	{
		if (cycle[i] != nmachs)
			return 0;
	}
	
	return 1;
}
			
int findmin(int* machinetime, int nmachs)
{

	int i, min = 0, j;
	
	for(i=0; i<nmachs; i++)
		if (machinetime[i]>0)
			break;
	
	min = machinetime[i];
	
	for(j=i+1; j<nmachs; j++)
	{
		if (machinetime[j] > 0 && machinetime[i] < min)
			min = machinetime[i];
	}
	
	return min;
}

int sort(const void *x, const void *y) {
  return (*(int*)x - *(int*)y);
}

int main( int argc, char* argv[] )
{
		FILE *in;
        int i, j, ptime, machine, njobs, nmachs, t, k;
		char probname[100];

        if ((in = fopen (argv[1], "r")) == NULL) {
                fprintf (stderr, "Unable to open for input\n");
                
        }

        fgets (probname, 100, in);
        fscanf (in, "%d %d", &njobs, &nmachs);

		jobtype job[njobs];
		int machines[nmachs];
		int max = 1;
		
		for(i=0; i<nmachs; i++) {
			fscanf(in, "%d", &machines[i]);
			
			if (machines[i] > max)
				max = machines[i];
			
		}
        
        for (i = 0; i < njobs; i++)
                for (j = 0; j < nmachs; j++) {
                        fscanf (in, "%d %d", &machine, &ptime);
                        job[i].order[j] = machine;
                        job[i].proctime[machine] = ptime;
                }
        
        fclose (in);

        printf ("\n%s", probname);
        for (i = 0; i < njobs; i++) {
                for (j = 0; j < nmachs; j++) {
                        t = job[i].order[j];
                        printf ("%2d %3d ", job[i].order[j],
                                            job[i].proctime[t]);
                }
                printf ("\n");
        }
        
        int time = 0, flag = 0;
		int cycle[njobs];
		int*** ans;
		int** machinetime;
		int** machinejob;
		int** allotted;
		
		machinetime = (int**) (malloc (nmachs * sizeof(int*)));
		machinejob = (int**) (malloc (nmachs * sizeof(int*)));
		
		for(i=0; i<nmachs; i++) {
			machinetime[i] = calloc (max, sizeof(int));
			machinejob[i] = calloc (max, sizeof(int));
		}
		
		for(i=0; i<nmachs; i++)
		{
			for(j=0; j<max; j++)
				machinejob[i][j] = -1;
		}
		
		
		ans = (int***) (malloc (njobs * sizeof(int**)));
		allotted = (int**) (malloc (njobs * sizeof(int*)));
		
		for(i=0; i<njobs; i++)
		{
			ans[i] = (int**) (malloc (nmachs * sizeof(int*)));
			
			for(j=0; j<nmachs; j++)
				ans[i][j] = (int*) calloc (max, sizeof(int));
			
			allotted[i] = calloc (nmachs, sizeof(int));
			cycle[i] = 0;
		}	
		
		for(i=0 ; i<njobs; i++)
		{
			for(j=0; j<nmachs; j++)
			{
				for(k=0; k<max; k++)
					ans[i][j][k] = -1;
			}
		}
		
		int min = 0;
		
		while (flag==0)
		{
			
			
			for(i=0; i<nmachs; i++)
			{
				for(j=0; j<machines[i]; j++)
				{
					machinetime[i][j]--;
				//machinetime[i]-= min;
				if (machinetime[i][j]<=0)
					updatejob(i, j, machinejob, machinetime, cycle, ans, time, njobs, nmachs, job, allotted);
				
				//printf("Machine : %d %d, Machine job : %d, Machine time : %d\n", i, j, machinejob[i][j], machinetime[i][j]);
				}
					
				}
						
			//min = findmin(machinetime, nmachs);
			//time += min;
			
		//	printf("-----------------------------\n");
			
			time++;
			
			flag = check_over(cycle, njobs, nmachs);
			
		}
		
		FILE *out;
		out=fopen("fcfs.txt", "w");
		
		printf("\n\n");
		
		for (i = 0; i < njobs; i++) {
                
			printf("JOB %d: ", i);
				
				for (j = 0; j < nmachs; j++) {
						
					for(k=0; k<max; k++)
					{
       			                 printf("M%d_%d : %4d ", j, k, ans[i][j][k]);
        				if (ans[i][j][k]!=-1)
					fprintf(out, "Job%d\t%d\t%d\tMachine%d_%d\n", i, ans[i][j][k], ans[i][j][k]+job[i].proctime[j], j, k);
					}
				}
                printf ("\n");
        }
		
		printf("\n\n");
		
		FILE *out2;
		FILE *out3;
		out2=fopen("fcfs_mach.txt", "w");
		out3=fopen("fcfs_stats.txt", "w");
		int startarray[njobs], waste, kk, it;
		int endarray[njobs], starttime, endtime, wp1=0, wp2=0;
		float percentage, percentage2, wper1=0, wper2=0;
		
		for (i = 0; i < nmachs; i++) {
                
			for(kk = 0; kk<max; kk++){
				
				printf("MACHINE %d_%d :", i, kk);
							
				for (j = 0; j < njobs; j++) {
            
					printf("J%d: %4d ", j, ans[j][i][kk]);
						startarray[j] = ans[j][i][kk];
						if (startarray[j] == -1)
							endarray[j] = -1;
						else
						{	endarray[j] = ans[j][i][kk]+job[j].proctime[i];
						
					fprintf(out2, "Machine%d_%d\t%d\t%d\tJob%d\n", i, kk, ans[j][i][kk], ans[j][i][kk]+job[j].proctime[i], j); }
				}
				
				qsort(startarray, njobs, sizeof(int), sort);
				qsort(endarray, njobs, sizeof(int), sort);
				
				waste = 0;
				
				for(k=0; k<njobs-1; k++)
					waste += startarray[k+1] - endarray[k];
				
				
				for(it=0; it<njobs; it++)
				{
					if (startarray[it]>=0)
						break;
				}
				
				starttime = startarray[it];
				endtime = endarray[njobs-1];
				
				if (endtime != -1)
				{
				percentage = 100 - ((float)waste / (endtime-starttime)) * 100;
				percentage2 = 100 - ((float)(waste+starttime) / (endtime)) * 100;				
				
				printf("Per2 : %0.2f Per : %0.2f Start : %d End : %d\n", percentage2, percentage, starttime, endtime);
				
				wper1 += percentage*(endtime-starttime);
				wp1 += (endtime-starttime);
				wper2 += percentage2*(endtime);
				wp2 += endtime;
				}
				else
					printf("\n");
        }
		}
		time--;
		
		printf("\nWeighted per2 : %0.2f, Weighted per1 : %0.2f\n", wper1/wp1, wper2/wp2);
		
		printf("\nPower time : %5d\n", wp1);
		
		fprintf(out3, "%d %d", time, wp1);
		
		fclose(out);
		fclose(out2);
		fclose(out3);
		
		printf("\nTotal time : %d\n", time);
        
        return 0;
}
