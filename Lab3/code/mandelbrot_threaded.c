//LIBRERIAS
//******************************************************************
#include <stdio.h>
#include <string.h>
#include <unistd.h>    // Unix-like system calls read and write
#include <fcntl.h>     // Unix-like system calls to open and close

#include "myutils.c"
#include "myutils.h"
//******************************************************************

//DEFINIMOS VARIABLES GLOBALES
#define R 4  // Constant indicating the image divisions RxR
#define THREAD_NUM R*R // Numero de threads en total R*R


enum { width=1024, height=1024 }; // Dimensiones de la imagen

unsigned char pixels[width*height*3]; // Cantidad de pixeles en total 

// Variables de control de THREADS
monitor_lock ml[THREAD_NUM];
pthread_mutex_t lock;
pthread_mutex_t locks[THREAD_NUM];
//******************************************************************

//ESTRUCTURA thread_data 
//Da la información definida de cada THREAD
typedef struct thread_data{
  unsigned char* pixel;
  int si;
  int sj;
  int ti;
  int tj;
}thread_data;

thread_data thread_data_array[THREAD_NUM]; // Array portadora de cada THREAD con su data
//******************************************************************

//Función generadora del encabezado de la imagen
int tga_write_header(int fd, int width, int height) {
  static unsigned char tga[18];
  int nbytes;
  tga[2] = 2;
  tga[12] = 255 & width;
  tga[13] = 255 & (width >> 8);
  tga[14] = 255 & height;
  tga[15] = 255 & (height >> 8);
  tga[16] = 24;
  tga[17] = 32;
  nbytes = write(fd, tga, sizeof(tga));
  return nbytes == sizeof(tga);
}

//Función encargada de la escritura del archivo
void write_tga(char* fname, unsigned char *pixels, int width, int height)
{
  int fd = open(fname,  O_CREAT | O_RDWR, 0640);
  tga_write_header(fd, width, height);
  printf("Created file %s: Writing pixels size %d bytes\n", fname, 3*width*height);
  write(fd, pixels, 3*width*height);
  close(fd);
}

//Función de lectura del encabezado
void tga_read_header(int fd, int* width, int* height) {
  static unsigned char tga[18];
  read(fd, tga, 12);
  read(fd, width, 2);
  read(fd, height, 2);
  read(fd, &tga[16], 2);
}

//Función de calculo de iteraciones  
int compute_iter(int i, int j, int width, int height) {
  int itermax = 255/2;  
  int iter;
  double x, xx, y, cx, cy;
  cx = (((float)i)/((float)width)-0.5)/1.3*3.0-0.7;
  cy = (((float)j)/((float)height)-0.5)/1.3*3.0;
  x = 0.0; y = 0.0;
  for (iter=1; iter < itermax && x*x+y*y < itermax; iter++)  {
    xx = x*x-y*y+cx;
    y = 2.0*x*y+cy;
    x = xx;
  }
  return iter;
}

//Generamos la imagen mediante un solo proceso
void generate_mandelbrot(unsigned char *p, int width, int height) {
  int i, j;
  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      *p++ = 255 * ((float)j / height);
      *p++ = 255 * ((float)i / width);
      *p++ = 2*compute_iter(i, j, width, height);
    }
  }
}

//Generamos la imagen mediante multithreading
void *generate_mandelbrot_region(void* params) {
  thread_data* t_d_a = (thread_data *) params;
  int ti = t_d_a->si;
  int tj = t_d_a->sj;
  unsigned char *p = t_d_a->pixel;
  int k,l;
  int n = width / R;
  for (k=0; k<n; k++) {
    int t_index = ti*n*width + tj*n + k*width;
    int i = t_index / width;
    int j = t_index % width;
    unsigned char *p_region = &p[3*t_index];
    for (l=0; l<n; l++) {
      *p_region++ = 255 * ((float)(j+l) / height);
      *p_region++ = 255 * ((float)i / width);
      *p_region++ = 2 * compute_iter(i, (j+l), width, height);
    }
  }
}

//Función destinada a intercambiar pixeles en la imagen pertinente
void interchange(int si, int sj, int ti, int tj, unsigned char *p, int width, int height) {
  int k;
  int n = width / R;
  unsigned char* square = malloc(n*n*3);
  memset(square, 0, n*n*3);

  for (k=0; k<n; k++) {
    int t_index = ti*n*3*width + tj*3*n + k*3*width;
    memcpy(&square[k*3*n], &p[t_index], n*3);
  }
  for (k=0;k<n;k++) {
    int s_index = si*n*3*width + sj*3*n + k*3*width;
    int t_index = ti*n*3*width + tj*3*n + k*3*width;
    memcpy(&p[t_index], &p[s_index], n*3);
  }
  for (k=0;k<n;k++) {
    int s_index = si*n*3*width + sj*3*n + k*3*width;
    memcpy(&p[s_index], &square[k*3*n], n*3);
  }

  free(square);
}

//Función destinada a intercambiar pixeles aplicando multithreading
void *thread_interchange(void* params){
  int si, sj, ti, tj; 
  si = sj = ti = tj = 1;

  for(int i=0; i<(1000/THREAD_NUM); i++) {
    while(si>=ti || sj>=tj){
      si = rand()%R;
      sj = rand()%R;
      ti = rand()%R;
      tj = rand()%R;
    }

    
    
    pthread_mutex_lock(&locks[si*R+sj]);
    pthread_mutex_lock(&locks[ti*R+tj]);
    interchange(si, sj, ti, tj, pixels, width, height);
    pthread_mutex_unlock(&locks[ti*R+tj]);
    pthread_mutex_unlock(&locks[si*R+sj]);
    
    
  }
}
void *thread_monitor_interchange(void* params){
  int si , sj , ti, tj ; 
  si = sj = ti = tj = 1;

  for(int i=0; i<(1000/THREAD_NUM); i++) {
    while(si>=ti || sj>=tj){
      si = rand()%R;
      sj = rand()%R;
      ti = rand()%R;
      tj = rand()%R;
    }

     
    
    mon_lock(&ml[si*R+si]);
    mon_lock(&ml[ti*R+tj]);
    interchange(si, sj, ti, tj, pixels, width, height);
    mon_unlock(&ml[si*R+si]);
    mon_unlock(&ml[ti*R+tj]);
    
    
  }
}
//******************************************************************

//Inicializamos el programa
//******************************************************************
int main(void) {
  int i, j, si, sj, ti, tj;

  pthread_t thread_id[THREAD_NUM];
  pthread_t thread_id2[THREAD_NUM];
  pthread_mutex_init(&lock,NULL);
  for(i = 0; i<THREAD_NUM;i++){
    pthread_mutex_init(&locks[i],NULL);
    mon_lock_init(&ml[i]);
  }
  
  generate_mandelbrot(pixels, width, height);
  startTimer(0);
  
  for(i = 0; i < R; i++){
    for(j = 0; j < R; j++){
      
      thread_data_array[i*R+j].si = i;
      thread_data_array[i*R+j].sj = j;
      thread_data_array[i*R+j].pixel = pixels;
      pthread_create(&thread_id[i*R+j], NULL,(void *) generate_mandelbrot_region, (void *) &thread_data_array[i*R+j]);
    }
  }
  
  for(i = 0; i < THREAD_NUM; i++){
    pthread_join(thread_id[i], NULL);
  }


  printf("Time spent generate_mandelbrot: %ldms\n", endTimer(0));
  write_tga("image.tga", pixels, width, height);

  
  startTimer(0);
  
  for(i = 0; i < THREAD_NUM; i++){
    pthread_create(&thread_id2[i], NULL, thread_monitor_interchange, NULL);
  }
  for(i = 0; i < THREAD_NUM; i++){
     pthread_join(thread_id2[i], NULL);
  }

  printf("Time spent during interchange: %ldms\n", endTimer(0));
  write_tga("image_scrambled.tga", pixels, width, height);

  return 0;
}
//******************************************************************