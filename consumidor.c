
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


void consumer(void){
    int compart,*buf,i,posicion,pos;
    srand(time(NULL));
    llenas=sem_open("LLENAS",0);
    vacias=sem_open("VACIAS",0);
    mutex=sem_open("MUTEX",0);
    if(llenas==SEM_FAILED|vacias==SEM_FAILED|mutex==SEM_FAILED){
        perror("Error abriendo semáforos");
        exit(1);
    }
    compart=open("Buffer",O_RDWR,0700);
    if (compart==-1){
        perror("Error en la funcion open");exit(1);
    }
    ftruncate(compart,(N+1)*sizeof(int));
    buf=(int*)mmap(NULL,(N+1)*sizeof(int),PROT_WRITE,MAP_SHARED,compart,0);
    if(buf < 0){perror("Error en la funcion mmap\n");exit(1);}
    
    for (i=0;i<10;i++){
        sem_wait(llenas);//Decrementa el valor de llenas en 1
        sem_wait(mutex);//Decrementa el valor de mutex en 1
        sem_getvalue(vacias,&pos); //Obtengo el valor del semáforo vacias y se guarda en pos
	    pos=N-pos;//El siguiente elemento a consumir será N-pos-1.
        printf("\nItem[%d] consumido en la posicion[%d]\n",*(buf+(pos-1)),pos-1);
        *(buf+(pos-1))=0;//Consume elemento en la posición pos-1 donde pos=N-pos
        sem_post(mutex);//Incrementa el valor de mutex en 1
        sem_post(vacias);//Incrementa el valor de vacias en 1
        sleep(rand()%4);//Para el proceso de 1 a 4 segundos aleatoriamente
    }
	//Cierra semáforos si otro proceso no los tiene abiertos
    sem_unlink("LLENAS");
    sem_unlink("VACIAS");
    sem_unlink("MUTEX");
    munmap(buf,N*sizeof(int));
	//Cierra mapa de memoria cargado en buf
    close(compart);
	//Cierra buffer
}

int main(){
    consumer();
}
