
#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define N 10

sem_t *llenas;
sem_t *vacias;
sem_t *mutex;


void producer(void){
	//Se establece una semilla para el rand
    srand(time(NULL));
    int item=1,compart,*buf,i,posicion,pos,a;
    
	//Se abren semáforos creados en el iniciador.
    llenas=sem_open("LLENAS",0);
    vacias=sem_open("VACIAS",0);
    mutex=sem_open("MUTEX",0);
    //si no se abren los semáforos se muestra el error.
    if(llenas==SEM_FAILED|vacias==SEM_FAILED|mutex==SEM_FAILED){
        perror("Error abriendo semáforos");
        exit(1);
    }
	//abre buffer
    compart=open("Buffer",O_CREAT|O_RDWR,0700);
    if (compart==-1){
        perror("Error en la funcion open");exit(1);
    }
    ftruncate(compart,(N)*sizeof(int));
	//Se abre mapa de memoria del buffer en la variable buf.
    buf=(int*)mmap(NULL,(N)*sizeof(int),PROT_WRITE,MAP_SHARED,compart,0);
    //Si error en abrir mapa de memoria se muestra por pantalla el error
    if(buf < 0){perror("Error en la funcion mmap\n");exit(1);}
    for (i=0;i<10;i++){//bucle for produce 10 items
		//bloquea si semáforo vacias=0
        sem_wait(vacias);//Down a vacias (Se reduce en 1 el valor del semaforo "Vacias")
        sem_wait(mutex);//Down a mutex (Se reduce en 1 el valor del semaforo "Mutex")
        sem_getvalue(llenas,&pos);//Obtengo valor del semáforo llenas y se guarda en pos
     	*(buf+pos)=item;
		//Introduce en el buffer el item  producido en la posición que marca el semáforo
		//imprime item a producir y la posición en la que se produce.
        printf("\nItem[%d] producido en la posicion[%d]\n",buf[pos],pos);
		//up a mutex permite 
        sem_post(mutex);//incrementa valor semaforo mutex en 1
        sem_post(llenas);//up a llenas, incrementa el valor del semaforo llenas en 1
        sleep(rand()%4);//para el proceso durante un tiempo comprendido entre 1 y 4 s
        item++;// incrementa el 1 la variable item, que será el que se introduzca en el buffer
		//en la siguiente vuelta.
    }
	//Destruimos semáforos si están abiertos por otro proceso
    sem_unlink("LLENAS");
    sem_unlink("VACIAS");
    sem_unlink("MUTEX");
    munmap(buf,N*sizeof(int));
	//Cierra el mapa de memoria
    close(compart);
	//Cierra archivo buffer.
}

int main(){
    producer();
}

