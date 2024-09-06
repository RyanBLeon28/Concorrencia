// 2) Uma barbearia possui um barbeiro, uma cadeira para corte de cabelo e N cadeiras de espera. 
// Clientes chegam aleatoriamente de tempos em tempos para cortar o cabelo. Se há uma cadeira de espera 
// disponível, o cliente senta e aguarda até que o barbeiro esteja disponível. Se não há uma cadeira de 
// espera disponível, o cliente vai embora. Quando o barbeiro termina de cortar o cabelo de um cliente, 
// ele verifica se há clientes aguardando. Caso haja, ele começa a cortar o cabelo do próximo cliente. 
// Caso não haja clientes esperando, ele dorme na cadeira de corte. Quando o barbeiro acordar, se houver 
// um cliente esperando, ele inicia o corte do recém chegado cliente.
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5  // Número de cadeiras de espera
int freeChairs = N; 

int clients = 0;
int *clientsQueue;

pthread_mutex_t clientsMutex;
pthread_mutex_t queueMutex;
int ini = 0, fim = 0;

sem_t barberReady;   // Semáforo para sinalizar que o barbeiro está pronto
sem_t chairsAccess;  // Semáforo para controlar o acesso às cadeiras de espera
sem_t clientsReady;  // Semáforo para sinalizar que há clientes prontos

void addClientById(int clientID) {
    clientsQueue[fim] = clientID;
    fim = (fim + 1) % N; // caso tenha que colocar no inicio da fila
}

int removeClient() {
    int clientID = clientsQueue[ini];
    ini = (ini + 1) % N;
    return clientID;
}

void* barber(void* arg) {
    while (1) {
        sem_wait(&clientsReady);      // Espera por um cliente
        sem_wait(&chairsAccess);      // Acessa as cadeiras de espera

        freeChairs += 1;  // Um cliente saiu da cadeira de espera
        int clientID;

        pthread_mutex_lock(&queueMutex);
        clientID = removeClient();
        pthread_mutex_unlock(&queueMutex);

        // Corta o cabelo
        printf("Barbeiro está cortando o cabelo do cliente %d ...\n", clientID);

        sem_post(&barberReady);       // Barbeiro está pronto para cortar o cabelo
        sem_post(&chairsAccess);      // Libera o acesso às cadeiras de espera

        sleep(1); 
    }
    return NULL;
}

void* client(void* arg) {
    int clientID; 

    pthread_mutex_lock(&clientsMutex);
    clients++;
    clientID = clients;
    pthread_mutex_unlock(&clientsMutex);
    
    sem_wait(&chairsAccess);  // Acessa as cadeiras de espera

    if (freeChairs > 0) {
        freeChairs -= 1;  // Cliente senta em uma cadeira de espera

        pthread_mutex_lock(&queueMutex);
        addClientById(clientID);
        pthread_mutex_unlock(&queueMutex);

        printf("Cliente %d sentou em uma cadeira de espera. Cadeiras livres: %d\n", clientID, freeChairs);
        
        sem_post(&clientsReady);  // Sinaliza que há um cliente pronto
        sem_post(&chairsAccess);  // Libera o acesso às cadeiras de espera

        sem_wait(&barberReady);  // Espera o barbeiro ficar pronto
    } else {
        printf("Cliente %d foi embora, sem cadeiras disponíveis.\n", clientID);
        sem_post(&chairsAccess);  // Libera o acesso às cadeiras de espera
    }

    return NULL;
}

int main() {
    printf("fale n : ");
    scanf("%d", &N);

    freeChairs = N;
    clientsQueue = (int*)malloc(N*sizeof(int));

    srand(time(NULL));

    pthread_t barber_thread;
    pthread_t client_thread;

    // Inicializa os semáforos
    sem_init(&barberReady, 0, 0);
    sem_init(&chairsAccess, 0, 1);
    sem_init(&clientsReady, 0, 0);

    pthread_mutex_init(&clientsMutex, NULL);
    pthread_mutex_init(&queueMutex, NULL);

    pthread_create(&barber_thread, NULL, barber, NULL);

    while (1){
        pthread_create(&client_thread, NULL, client, NULL);
        pthread_detach(client_thread);
        
        sleep(rand() % 2);  // Tempo aleatorio da chegada de um cliente
    }

    pthread_mutex_destroy(&clientsMutex);
    pthread_mutex_destroy(&queueMutex);
    free(clientsQueue);
    return 0;
}