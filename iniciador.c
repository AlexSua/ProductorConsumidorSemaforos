
#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define N 10//Tamaño del buffer.

//Declaramos semáforos
sem_t *llenas;
sem_t *vacias;
sem_t *mutex;

void iniciador(void){
//Destruye semáforos de alguna ejecución anterior no finalizada
    sem_unlink("LLENAS"); 
    sem_unlink("VACIAS");
    sem_unlink("MUTEX");
    int item=0,compart,*buf,i,posicion;
	//crea semáforos con permisos de lectura, escritura y ejecución de usuario (0700)
    llenas=sem_open("LLENAS",O_CREAT,0700,0);//Inicializamos semáforo en 0 
    vacias=sem_open("VACIAS",O_CREAT,0700,N);//Inicializamos semáforo en N
    mutex=sem_open("MUTEX",O_CREAT,0700,1);//Inicializamos semáforo en 1

	//si algun semáforo falla en su creacion se muestra el error por pantalla
    if(llenas==SEM_FAILED|vacias==SEM_FAILED|mutex==SEM_FAILED){
        perror("Error abriendo semáforos");
        exit(1);
    }
    compart=open("Buffer",O_CREAT|O_RDWR,0700);//Se crea/abre el archivo buffer con permisos (0700)
    //Si falla el fichero en su apertura muestra el error por pantalla
	if (compart==-1){
        perror("Error en la funcion open");exit(1);
    }
    ftruncate(compart,(N)*sizeof(int));//Ajusta el tamaño del buffer a N
	//Mapea el archivo buffer en la variable buf de manera que puede ser mapeado por 
	//otros procesos MAP_SHARED
    buf=(int*)mmap(NULL,(N)*sizeof(int),PROT_WRITE,MAP_SHARED,compart,0);
	//si la función mmap da error este se muestra por pantalla
    if(buf < 0){perror("Error en la funcion mmap\n");exit(1);}
	//Ponemos a 0 todas las posiciones del buffer.
    for (i=0;i<=N;i++)
	*(buf+i)=0;
    munmap(buf,N*sizeof(int));
	//Cierra el mapa de memoria
    close(compart);
	//cierra buffer
}


int main(){
    iniciador();
}

