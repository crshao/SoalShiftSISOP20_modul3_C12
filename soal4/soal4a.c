#include <stdio.h> 
#include <pthread.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 

void *multiply(void* arg) 
{ 
  int *data = (int *)arg; 
  int k = 0, i = 0; 

  int x = data[0]; 
  for (i = 1; i <= x; i++) 
    k += data[i]*data[i+x]; 

  int *p = (int*)malloc(sizeof(int)); 
  *p = k; 

  pthread_exit(p); 
} 

int main() 
{ 
    int m1[4][2] = {{1, 2},
                    {3, 4},
                    {5, 6},
                    {7, 8}};
    int m2[2][5] = {{1, 2, 3, 4, 5},
                    {6, 7, 8, 9, 10}};

  int   row1=4,
        col1=2,
        row2=2,
        col2=5,
        i,j,k;

  pthread_t *threads; 
  threads = (pthread_t*)malloc(row1*col2*sizeof(pthread_t)); 

  int count = 0; 
  int* data = NULL; 

  for (i = 0; i < row1; i++) 
    for (j = 0; j < col2; j++) 
    { 
      data = (int *)malloc((20)*sizeof(int)); 
      data[0] = col1; 

      for (k = 0; k < col1; k++) 
      data[k+1] = m1[i][k]; 

      for (k = 0; k < row2; k++) 
      data[k+col1+1] = m2[k][j]; 

      pthread_create(&threads[count++], NULL,  
        multiply, (void*)(data)); 
    } 

  key_t key = 1234;
  int *value;

  int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
  value = shmat(shmid, NULL, 0);

  printf("Hasil Perkalian Matriks:\n"); 
  for (i = 0; i < row1*col2; i++)  
  { 
    void *k; 

    pthread_join(threads[i], &k); 
    
    int *p = (int *)k;
    value[i] = *p;
    printf("%-2d ", value[i]); 
    if ((i + 1) % col2 == 0) 
      printf("\n"); 
  }
  shmdt((void *) value);

  return 0; 
} 