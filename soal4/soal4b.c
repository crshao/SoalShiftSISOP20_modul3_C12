#include <pthread.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

void *jumlah(void* arg) 
{ 
  int data = *(int*)arg;
  unsigned long long x = 1; 
  
  x=data*(data+1)/2;

  unsigned long long *p = (unsigned long long*)malloc(sizeof(unsigned long long)); 
  *p = x; 

  pthread_exit(p); 
}

int main()
{
  key_t key = 1234;
  int *value;

  int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
  value = shmat(shmid, NULL, 0);
  
  pthread_t threads[20]; 
  for (int i = 0; i < 20; i++)  
  {
    int *new_val = &value[i];
    pthread_create(&threads[i], NULL, jumlah, (void *)new_val);
  }
  
  for (int i = 0; i < 20; i++)  
  { 
    void *k; 
    pthread_join(threads[i], &k);
    
    unsigned long long *p = (unsigned long long *)k;
    printf("%-5llu ",*p);
    if ((i + 1) % 5 == 0)
      printf("\n"); 
  }

  shmdt(value);
  shmctl(shmid, IPC_RMID, NULL);
}