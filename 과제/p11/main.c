#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int row;
int col;
__thread int sum = 0;

typedef struct mult_val
{
	int *row_val;
	int *vector_val;
}mult_val;

void *thread(void *arg)
{
	mult_val *compute = (mult_val*)arg;
	for(int i = 0; i < col; i++)
	{
		sum += compute->row_val[i] * compute->vector_val[i];
	}
	printf("[ %d ]\n", sum);
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		printf("Error! There should be 3 arguments.\n");
		exit(1);
	}
	
	row = atoi(argv[1]);
	col = atoi(argv[2]);

	int matrix[row][col];
	int vector[row];
	pthread_t tid[row];

	srand(time(NULL));

	printf(" *** Matrix ***\n");
	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			matrix[i][j] = rand() % 10;
			printf("[ %d ] ", matrix[i][j]);
		}
		printf("\n");
	}

	printf(" *** Vector ***\n");
	for(int i = 0; i < col; i++)
	{
		vector[i] = rand() % 10;
		printf("[ %d ]\n", vector[i]);
	}

	printf(" *** Result ***\n");
	mult_val mult;
	for(int t = 0; t < row; t++)
	{
		mult.row_val = matrix[t];
		mult.vector_val = vector;
		if(pthread_create(&tid[t], NULL, thread, (void*)&mult))
		{
			printf("ERROR: pthread creation failed.\n");
			exit(1);
		}
		sleep(1);
	}
	for(int t = 0; t < row; t++)
	{
		pthread_join(tid[t], NULL);
	}
	return 0;
}
