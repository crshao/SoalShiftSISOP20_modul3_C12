# SoalShiftSISOP20_modul3_C12

Soal Shift Sistem Operasi 2020
#
- Rofita Siti Musdalifah / 05111840000034
- Calvin Wijaya / 0511184000086

#
1. [Soal1](#soal1)
2. [Soal2](#soal2)
3. [Soal3](#soal3)
4. [Soal4](#soal4)
#

## Soal2
Membuat game menggunakan socket dan threading untuk mengkomunikasikan antara server dan client, serta thread agar game dapat berjalan secara paralel untuk masing-masing player.
Soal ini terdiri dari 2 sisi,client side dan server side


Pada Client Side, terdapat 2 screen tampilan, yaitu **Screen 1** untuk menangani login dan register akun, sedangkan **Screen 2** untuk mencari player lawan agar game dapat dimulai.

Berikut source code Screen 1, untuk menangani login dan register:
```c
int main()
    ....
    screen1:
    printf("1. Login\n2. Register\nChoices : ");
    scanf("%s", cmd);
    if(strcmp(cmd, "login") == 0)
    {
        strcpy(username, "l ");
        printf("Username : ");
        getchar();
        scanf("%[^\n]", username);
        strcat(username, temp);
        printf("Password : ");
        getchar();
        scanf("%[^\n]", pass);
        strcat(username, "\t");
        strcat(username, pass);
        send(sock, username, strlen(username), 0);
        int feedback;
        read(sock, &feedback, sizeof(feedback));
        if(feedback)
        {
            printf("login success\n");
            send(sock, "sukses", 6, 0);
            screen2:
	    ...
        }
        else
        {
            printf("login failed\n");
            goto screen1;
        }
    }
    else if(strcmp(cmd, "register") == 0)
    {
        strcpy(username, "r ");
        printf("Username : ");
        getchar();
        scanf("%[^\n]", temp);
        strcat(username, temp);
        printf("Password : ");
        getchar();
        scanf("%[^\n]", pass);
        strcat(username, "\t");
        strcat(username, pass);
        send(sock, username, strlen(username), 0);
        printf("register success\n");
        goto screen1;
    }
    else
    {
        printf("invalid input\n");
        goto screen1;
    }
    ...
}
```
Dikarenakan menggunakan bahasa C, untuk memberikan value string pada suatu variabel, harus menggunakan fungsi ```strcpy``` yang berarti string copy, sedangkan untuk menggabungkan string, digunakan fungsi ```strcat``` yang berarti string concat. Dikarenakan pada contoh soal no 2, memungkinkan username dan password memiliki spasi, tidak dapat diterima menggunakan ```scanf("%s", &str);``` biasa, sehingga harus menggunakan ```scanf("%[\n]", &str);``` yang berarti scan terus inputan tersebut sampai menemui karakter ```\n``` yang berarti enter. Hal yang sama dilakukan untuk men-scan input password.

Di sini, setiap mendapatkan input username dan password baik dari login maupun register, akan kami passing (send) melalui socket ke server side, dengan command ```send(sock, str, strlen(str), 0)```. Untuk membedakan string yang dipassing merupakan dari menu login atau register, maka kami tambahkan karakter **l** yang menandakan itu dari menu login, dan karakter **r** yang menandakan dari menu register saat ingin dipassing.

#

Berikut source code Screen 2, untuk menangani find player agar dapat memulai game:
```c
void *finding()
{
    while(1)
    {
        printf("Waiting for player ...\n");
        sleep(1);
    }
}

void *playing(void *arg)
{
    while(1)
    {
        char ch = getchar();
        if(ch == ' ') 
        {
            printf("hit !!\n");
            send(*(int*) arg, &ch, sizeof(ch), 0);
        }
    }
}

int main()
{
	screen2:
	printf("1. Find Match\n2. Logout\nChoices : ");
	scanf("%s", cmd2);
	if(strcmp(cmd2, "logout") == 0)
	{
		send(sock, cmd2, strlen(cmd2), 0);
		goto screen1;
	}
	else if(strcmp(cmd2, "find") == 0)
	{
		int start;
		send(sock, cmd2, strlen(cmd2), 0);
		pthread_t th;
		pthread_create(&th, NULL, finding, NULL);
		read(sock, &start, sizeof(start));
		pthread_cancel(th);
		printf("Game dimulai silahkan \e[3mtap tap\e[0m secepat mungkin !!\n");
		struct termios prev, cur;
		tcgetattr(0, &prev);
		cur = prev;
		cur.c_lflag &= -ICANON;
		cur.c_lflag &= -ECHO;
		tcsetattr(0, TCSANOW, &cur);
		pthread_t th2;
		pthread_create(&th2, NULL, playing, (void *) &sock);
		int health = 100;
		while(1)
		{
		    read(sock, &health, sizeof(health));
		    if(health == -1) break;
		    printf("Health: %d\n", health);
		}
		pthread_cancel(th2);
		bool cek;
		read(sock, &cek, sizeof(cek));
		if(cek) printf("Game berakhir kamu menang\n");
		else printf("Game berakhir kamu kalah\n");
		tcsetattr(0, TCSANOW, &prev);
		goto screen2;
	}
	else
	{
		printf("invalid input\n");
		goto screen2;
	}
	...
}
```
Untuk screen 2, akan ada 2 thread yang dijalankan thread pertama untuk mencetak "waiting for player ..." (```pthread_create(&th, NULL, finding, NULL);```) ketika belum ditemukan client lain yang sedang mencari match juga. Thread ini akan terus berjalan setiap detik, sampai dicancel saat sudah menemukan player lain. Untuk mengecek apakah sudah ada player lain yang dapat bermain, digunakan fungsi ```read(sock, &start, sizeof(start));```, di mana variabel start akan dipassing dari server side yang menandakan bahwa game sudah siap dimulai karena sudah ada 2 client yang terkoneksi ke server di waktu yang sama. Kemudian di sini kami juga menggunakan fungsi ```termios``` agar saat bermain, saat user menginputkan karakter spasi, langsung terbaca menembak musuh tanpa harus menekan enter. Saat  bermain maka akan dijalankan thread yang berfungsi menunggu player saat menginput spasi untuk menembak musuh. Saat health salah satu player sudah habis (<= 0), maka server akan mengirimkan socket value health -1, yang menandakan game dihentikan, kemudian server mengirimkan kode ke masing-masing player apakah ia menang atau kalah.
#
Pada Server Side, digunakan untuk verifikasi client yang sedang login/register, untuk penyimpanan data ke ```akun.txt```, serta pengecekan data saat login apakah akun sudah terdaftar. Kemudian pada server side, juga berfungsi untuk mencari player-player yang dapat saling terkoneksi, ketika banyak yang terkoneksi ada 2 player, maka game akan segera dimulai. Ketika sudah selesai, maka koneksi mereka akan diputus menjadi bukan yang sedang bermain. Ruang bermain hanya dibatasi 2 player saja, yang masing-masing mempunyai thread sendiri.

Untuk menyimpan data suatu player, yaitu thread sendiri, thread musuh, health sendiri, dan health musuh, digunakan struct player yang menyimpan itu semua, agar saat menjalankan thread player, dapat langsung melakukan passing semua data player.

Source code:
```c
typedef struct akun {
    char un[1024];
    char ps[1024];
} akun;

typedef struct client_serv {
    int cid;
    int login;
} client_serv;

typedef struct player {
    int cidp, cide;
    int *healthp, *healthe;
} player;

void *ready(void *arg)
{
    client_serv cl = *(client_serv *) arg;
    int cid = cl.cid;
    int log = cl.login;
    if(log)
    {
        screen1:;
        char data[1024], un[1024], ps[1024], kode;
        memset(data, 0, sizeof(data));
        read(cid, data, sizeof(data)); //kode username	password
        kode = data[0];
        int id = 0;
        for(int i = 2; i < strlen(data); i++)
        {
            if(data[i] == '\t') break;
            un[id] = data[i];
            id++;
        }
        //un[id] = '\0';
        int id2 = 0;
        id += 3;
        for(int j = id; j < strlen(data); j++)
        {
            ps[id2] = data[j];
            id2++;
        }
        //ps[id2] = '\0';
        //printf("debug: %s - %s\n", un, ps);
        if(kode == 'l')
        {
            bool done = 0;
            while(akun_cek);
            for(int i = 0; i < akun_n; i++)
            {
                if((strcmp(un, list_akun[i].un) == 0) && (strcmp(ps, list_akun[i].ps) == 0))
                {
                    done = 1;
                    break;
                }
            }
            send(cid, &done, sizeof(done), 0);
            if(done) printf("Auth success\n");
            else
            {
                printf("Auth Failed\n");
                goto screen1;
            }
        }
        else if(kode == 'r')
        {
            akun_cek = 1;
            akun akun_reg;
            strcpy(akun_reg.un, un);
            strcpy(akun_reg.ps, ps);
            list_akun[akun_n] = akun_reg;
            akun_n++;
            while(file_cek);
            file_cek = 1;
            FILE *fptr = fopen("akun.txt", "a");
            fprintf(fptr, "%s\t%s\n", un, ps);
            fclose(fptr);
            file_cek = 0;
            akun_cek = 0;
            printf("Username\tPassword\n");
            for(int i = 0; i < akun_n; i++)
            {
                akun cur = list_akun[i];
                printf("%s\t%s\n", cur.un, cur.ps);
            }
        }
    }
    log = 1;
    char cmdt;
    read(cid, &cmdt, strlen(&cmdt));
    if(strcmp(&cmdt, "logout") == 0) goto screen1;
    else if(strcmp(&cmdt, "find") == 0)
    {
        while(conn_cek);
        conn_cek = 1;
        conn++;
        conn_cek = 0;
        while(conn < 2);
    }
    pthread_exit(0);
}

void *play(void *arg)
{
    player p = *(player *) arg;
    int *he = p.healthe;
    int start = 1;
    send(p.cidp, &start, sizeof(start), 0);
    while(1)
    {
        char ch;
        read(p.cidp, &ch, sizeof(ch));
        *he -= 10;
        send(p.cide, he, sizeof(*he), 0);
    }
}

int main()
{
    ...
    while(1)
    {
        bool player1_cek = 0;
        bool player2_cek = 0;
        int health1 = 100;
        int health2 = 100;
        player player1, player2;
        player1.cidp = client[0];
        player1.cide = client[1];
        player1.healthp = &health1;
        player1.healthe = &health2;
        player2.cidp = client[1];
        player2.cide = client[0];
        player2.healthp = &health2;
        player2.healthe = &health1;
        pthread_create(&th[0], NULL, play, (void *) &player1);
        pthread_create(&th[1], NULL, play, (void *) &player2);
        while(1) if(health1 <= 0 || health2 <= 0) break;
        pthread_cancel(th[0]);
        pthread_cancel(th[1]);
        int fin = -1, win = 1, lose = 0;
        send(client[0], &fin, sizeof(fin), 0);
        send(client[1], &fin, sizeof(fin), 0);
        conn -= 2;
        if(health1 <= 0)
        {
            send(client[0], &lose, sizeof(lose), 0);
            send(client[1], &win, sizeof(win), 0);
        }
        else if(health2 <= 0)
        {
            send(client[0], &win, sizeof(win), 0);
            send(client[1], &lose, sizeof(lose), 0);
        }
        for(int i = 0; i < 2; i++)
        {
            client_serv p;
            p.cid = client[i];
            p.login = 1;
            pthread_create(&th[i], NULL, ready, (void *) &p);
        }
        pthread_join(th[0], NULL);
        pthread_join(th[1], NULL);
    }
    ...
}
```
#

## Soal3
kategorikan file-file ke dalam folder sesuai ekstensinya. Terdapat 3 mode input yakni -f, -d, dan \*. Dimana pada mode -f user dapat menambahkan argumen yang berupa full path file sebanyak yang user inginkan, pada mode -d user hanya bisa input 1 directory saja, lalu pada mode \* user akan memindahkan seluruh file berdasarkan ekstensinya. Menggunakan library ```dirent.h``` untuk melakukan looping pada tiap direktori dan mengecek setiap ekstensi file. Pada soal ini kami membuat fungsi untuk memindahkan file ke folder cwd (current working directory) per file. 
```c
void pindahFile(char *argv, char *cwd){
//   printf("stringvoid = %s\n", argv);
//   printf("stringvoid = %s\n", cwd);
  
  char string[1000];
  strcpy(string, argv);
  int isFile = is_regular_file(string);
  char dot = '.'; 
  char slash = '/';
  char* tipe = strrchr(string, dot); 
  char* nama = strrchr(string, slash);
  
  char tipeLow[100];
  if(tipe)
  {
    if((tipe[strlen(tipe)-1] >= 'a' && tipe[strlen(tipe)-1] <= 'z') || (tipe[strlen(tipe)-1] >= 'A' && tipe[strlen(tipe)-1] <= 'Z'))
    {
      strcpy(tipeLow, tipe);
      for(int i = 0; tipeLow[i]; i++){
        tipeLow[i] = tolower(tipeLow[i]);
      }
    }
  }
  else
  {
    if(!isFile){
      printf("ini adalah folder, salah argumen\n");
    //   mkdir(nama, 0777);
      return;
    }
    else
    {
      strcpy(tipeLow, " Unknown"); //tanpa ekstensi
    }
  }
    mkdir((tipeLow + 1), 0777); //bikin folder ekstensi

    size_t len = 0 ;
    // strcpy
    char a[1000] ; //res
    strcpy(a, argv);
    char b[1000] ; //des
    strcpy(b, cwd);
    strcat(b, "/");
    strcat(b, tipeLow+1);
    strcat(b, nama);
    printf("a = %s\n", a);

    printf("b = %s\n", b);

    rename(a, b);
    remove(a);
}
```
untuk file banyak, yaitu command -d dan \*, ada lagi fungsi untuk memasukan setiap file yang ada di dalam folder tersebut ke sebuah string lalu di iterasi untuk semua file yang ada didalam directory tersebut.
```c
void sortHere(char *asal){
  arg_struct args;
  // args.cwd = "/home/rapuyy/modul3";
  strcpy(args.cwd,"/home/rapuyy/modul3");
  DIR *dirp;
    struct dirent *entry;
    dirp = opendir(asal);
    int index = 0;
    while((entry = readdir(dirp)) != NULL)
    {
      if(entry->d_type == DT_REG)
      {
        char namafile[105];
        sprintf(namafile, "%s/%s", asal, entry->d_name);
        strcpy(args.asal, namafile);
        if(strcmp(namafile, "/home/rapuyy/modul3/no3.c")!=0)
        {
            pthread_create(&tid[index], NULL, pindahf, (void *)&args);
            printf("%s\n", namafile);
            sleep(1);
            index++;    
        }
        
      }
    }
}
```

#

## Soal4
Soal ini meminta kami untuk menghitung perkalian matriks kemudian mengolah angka-angka tersebut menjadi  faktorial penjumlahan. dan terakhir memberikan list.

#
4a. Di bagian ini kita diminta untuk mengalikan matriks dengan ukuran 4x2 dan 2x5, yang mana hasilnya adalah matriks 4x5, dan isi dari matriks awalnya kita isi sendiri. digunakan thread untuk menghitug tiap bagiannya
```c
.....

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
.....
```
#
4b. Kemudian hasilnya akan dikirim ke proses *soal4b.c* menggunakan shared memory. setelah *soal4b.c* mendapat kiriman dari *soal4a.c*, matriks hasil akan dilakukan penjumlahan dari n sampai 1. lalu ditampilkan hasilnya dengan tampilan seperti matriks.
```c
void *jumlah(void* arg) 
{ 
  int data = *(int*)arg;
  unsigned long long x = 1; 
  
  x=data*(data+1)/2;

  unsigned long long *p = (unsigned long long*)malloc(sizeof(unsigned long long)); 
  *p = x; 

  pthread_exit(p); 
}
  for (int i = 0; i < 20; i++)  
  {
    int *new_val = &value[i];
    pthread_create(&threads[i], NULL, jumlah, (void *)new_val);
  }
```
#
4c. Menemukan jumlah file dan folder di direktori saat ini dengan command "ls | wc -l" menggunakan IPC Pipe.
    pada child me-list isi directory, dan pada parent menghitung jumlah line nya
```c
.....
     if (p1 == 0) {
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        char *argv[] = {"ls", NULL};
        execv("/bin/ls", argv);
    }

    else{
        dup2(fd[0], 0);
        close(fd[0]);
        close(fd[1]);
        char *argv[] = {"wc", "-l", NULL};
        execv("/usr/bin/wc", argv);
    }
.....
```
