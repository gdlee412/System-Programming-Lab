#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int count;
int points;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void *montecarlo(void *arg) {
    double x, y;
    unsigned int seed;
    /* your code here */
    for(int i = 0; i < points; i++)	 
    {
    	x = (double)rand_r(&seed) / RAND_MAX;
    	y = (double)rand_r(&seed) / RAND_MAX;
	x = pow(x, 2);
	y = pow(y, 2);
	double distance = sqrt(x + y);
	if(distance <= 1) {
		pthread_mutex_lock(&m);
		count++;
		pthread_mutex_unlock(&m);
	}
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: ./p13 [num_threads] [points per thread]\n");
        exit(1);
    }
    int n = atoi(argv[1]);
    points = atoi(argv[2]);
    /* your code here */
    pthread_t tid[n];
    count = 0;
		
    for(int t = 0; t < n; t++) {
    	if(pthread_create(&tid[t], NULL, &montecarlo, NULL)) {
		printf("Error: pthread creation failed.\n");
		exit(1);
	}

	pthread_join(tid[t], NULL);
    }	    
		
	
    double pi = (double)(4 * count) / (double)(n * points);
    printf("pi: %lf\n", pi);
    return 0;
}
