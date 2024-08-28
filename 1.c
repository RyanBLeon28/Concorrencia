// 1) Em uma mesa circular N filósofos estão sentados. Em frente a cada filósofo há um 
// prato e ao lado de cada prato há somente um hashi para que os filósofos possam comer macarrão.
// Para comer, um filósofo precisa pegar dois hashis, que estão localizados a sua esquerda e a 
// sua direita, respectivamente. Note que um filósofo compartilha os hashis com os filósofos 
// sentados ao seu lado. Sua tarefa é desenvolver um algoritmo que coordene o processo de comer 
// e pensar de um filósofo. Caso um filósofo não consiga pegar os dois hashis, ele deverá deixar 
// os hashis novamente na mesa para evitar a ocorrência de impasses e inanição e pensar. Após 
// terminar de pensar, ele deverá tentar comer novamente.

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

int N; // Número de filósofos

pthread_mutex_t *hashis;

// Estrutura para as informacoes do filosofo
typedef struct {
    int philosopher;
} Philosopher;

void pensar(int n){
    printf("Filosofo %d está pensando \n", n);
    sleep(1);
}

void* comer(void* arg) {
    Philosopher* philosopher = (Philosopher*) arg;

    int left = philosopher->philosopher;
    int right = (philosopher->philosopher + 1) % N;

    while (1) {
        // trylock retorna zero caso o mutex conseguiu bloquear
        if (pthread_mutex_trylock(&hashis[left]) == 0 ){  // Pega o hashi da esquerda
            if (pthread_mutex_trylock(&hashis[right]) == 0){ // Pega o hashi da direita
                
                printf("Filosofo %d está comendo\n", philosopher->philosopher + 1);
                sleep(rand() % 3 + 1);  // Simula o tempo de comer

                // Devolve os hashis
                pthread_mutex_unlock(&hashis[right]);  // Solta o hashi da direita
                pthread_mutex_unlock(&hashis[left]);   // Solta o hashi da esquerda
            }
            else { // Pensa porque não conseguiu pegar o segundo hashi e solta o da direita

                pthread_mutex_unlock(&hashis[left]);
                pensar(philosopher->philosopher + 1); 
            }
        } 
        else {  // caso nao pegue nenhum hashi
            pensar(philosopher->philosopher + 1);
        }
    }
    pthread_exit(NULL);
}

int main(){
    srand(time(NULL));

    printf("Digite o número de filósofos: ");
    scanf("%d", &N);

    hashis = malloc(N * sizeof(pthread_mutex_t));

    // Inicializa os mutexes dos hashis
    for (int i = 0; i < N; i++) {
        pthread_mutex_init(&hashis[i], NULL); // inicia como desbloqueado
    }

    pthread_t threads[N];
    Philosopher thread_data[N];

    for (int i = 0; i < N; i++) {
        thread_data[i].philosopher = i;
        pthread_create(&threads[i], NULL, comer, (void*)&thread_data[i]);
        usleep(rand() % 3 + 1);
    }

    //Deve ser chamada para sincronizar as threads no final
    for(int i = 0; i < N; i++){
        pthread_join(threads[i],NULL);
    }

    // Destrói os mutexes dos hashis
    for (int i = 0; i < N; i++) {
        pthread_mutex_destroy(&hashis[i]);
    }

    return 0;
}
