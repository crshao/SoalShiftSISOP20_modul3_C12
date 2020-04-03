#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/types.h>
#include <sys/stat.h>
#include<unistd.h>
#include<pthread.h>

int statt;
pthread_t tid[3];

struct struct1
{
    char *w[1], *x[1], *y[1], *z[1];
};

void* pindah(void *param)
{
    struct struct1 *par = (struct struct1*) param;
    
    int ch;

    FILE *file1 = fopen(*par->z, "r");
    FILE *file2 = fopen(*par->w, "w");

    //jika error
    if (!file1) {
            printf("file tidak bisa dibuka untuk di-read.\n");
            fclose(file2);
            return 0;
    }
    
    if (!file2) {
            printf("file tidak bisa dibuka untuk di-write\n");
            return 0;
    }
    
    //copy
    while ((ch = fgetc(file1)) != EOF) {
            fputc(ch, file2);
    }

    fclose(file1);
    fclose(file2);

    remove(*par->z);

	return NULL;
}

void *make_directory(void *a)
{
    struct struct1 *x = (struct struct1*)a ;
    statt = 1;
    mkdir(*x->y, 0777);
	return NULL;
}

static void spesial(char *nameFile, char *newFile, char *akhir)
{
    int titik, i, j;
 
    i = strlen(nameFile) - 1;
 
    while (i && nameFile[i] != '.') //cari titik
    {
        i--;
    }
 
    //cari titik
    if (i)
    {
        titik = i;
    }
    else
    {
        titik = strlen(nameFile); //tidak ketemu
 
        i = strlen(nameFile) - 1; //reset
    }
    while (i && nameFile[i] != '/')
    {
        if (nameFile[i] != '/')
            i--;
    }
    if (nameFile[i] == '/') //jika ada /
        i++;
         
    int x = 0;

    while (i < titik)
    {
        newFile[x] = nameFile[i];
        x++;
        i++;
    }

    int y = 0;
    j = strlen(nameFile) - 1;
    while (i<j)
    {
        akhir[y] = nameFile[i+1];
        y++;
        i++;
    }
    
    //terminate
    newFile[x] = '\0';
    akhir[y] = '\0';
}

int main(int argc, char* argv[])
{
    char nameFile[100], newFile[100];
    char akhir[10];
    char path_a[100], path_b[100];
    int i, j;

    if (strcmp (argv[1],"-f") == 0)
    {
        for (i = 2; i < argc; i++) //loop sesuai jlh argumen
        {
            memset(nameFile, 0, sizeof(nameFile));
            memset(newFile, 0, sizeof(newFile));
            memset(akhir, 0, sizeof(akhir));
            memset(path_a, 0, sizeof(path_a));
            memset(path_b, 0, sizeof(path_b));

            strcpy(nameFile, argv[i]);
            spesial(nameFile, newFile, akhir);

            strcat(newFile, ".");
            strcat(newFile, akhir);
            strcat(path_a, "/home/hao/Documents/Praktikum3/soal3/");
            strcat(path_a, newFile);

            strcat(path_b, "/home/hao/Documents/Praktikum3/soal3/");
            strcat(path_b, akhir);
            strcat(path_b, "/");
            strcat(path_b, newFile);

            // ditaruh dalam struct
            struct struct1 *temp = (struct struct1*) malloc (sizeof (struct struct1));
            *temp -> x = newFile;
            *temp -> y = akhir;
            *temp -> z = path_a;
            *temp -> w = path_b;

            int flag = 0;
            statt = 0;

            //membuat thread
            while (flag < 2)
            {
                if(flag == 0){
                    pthread_create(&(tid[flag]),NULL,&make_directory, (void *)temp);
                } else {
                    pthread_create(&(tid[flag]),NULL,&pindah,(void *)temp);
                }
                pthread_join(tid[flag],NULL);
                flag++;
            }
        }
    }
    return 0;
}
