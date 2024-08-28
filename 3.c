// 3) Você foi contratado pelo C3 (Clube do Capitalismo Compulsivo) para desenvolver um sistema de 
// gerenciamento de um banco. Sua principal tarefa é desenvolver um sistema que evite que múltiplas 
// operações sejam realizadas em uma mesma conta bancária de forma simultânea. Assuma que, para cada
// conta corrente, você possui tanto o seu identificador como o saldo disponível.  Crie diversas 
// threads para simular sequências de operações em paralelo e, aleatoriamente, defina qual conta 
// receberá a operação, o tipo de operação (crédito ou débito), e o valor da operação. Realize 
// simulações com diferentes números de threads. Após, assuma que existe uma nova operação que 
// realiza a consulta do saldo. A principal diferença para esta operação é que até 5 threads podem
// consultar o saldo de uma conta simultaneamente, desde que nenhuma outra thread esteja realizando 
// uma operação de crédito ou débito. Operações de débito e crédito continuam precisando de acesso 
// exclusivo aos registros da conta para executarem adequadamente.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_CONSULTAS 5

int n_contas; // número de contas que serão criadas
int n_threads = 100;

typedef struct {
    int id;
    int saldo;
    int limite;
    int fatura;
    pthread_mutex_t mutex_conta;
    pthread_cond_t cond_consulta;
    int consultas;
} Conta_corrente;

Conta_corrente *contas = NULL;
pthread_mutex_t mutex_global = PTHREAD_MUTEX_INITIALIZER;

void* operacoes(void* arg) {
    int id = *(int*) arg;
    int conta = rand() % n_contas;
    int op = (rand() % 3) + 1;
    int valor = (rand() % 1000) + 1;

    
    pthread_mutex_lock(&contas[conta].mutex_conta);


    if (op == 1 || op == 2) {
        // Operação débito ou crédito

        if (op == 1) {
            // Operação débito
            if ((contas[conta].saldo - valor) > 0) {
                printf("\nThread: %d Compra no débito de R$ %d na conta %d \n", id, valor, conta);
                contas[conta].saldo -= valor;
                printf("Saldo antigo: R$ %d \n", contas[conta].saldo + valor);
                printf("Saldo atual: R$ %d \n", contas[conta].saldo);
            } else {
                printf("\nThread: %d Compra no débito de R$ %d negada para a conta %d (saldo insuficiente) \n", id, valor, conta);
            }
        } else if (op == 2) {
            // Operação crédito
            if ((contas[conta].fatura + valor) < contas[conta].limite) {
                printf("\nThread: %d Compra no crédito de R$ %d na conta %d \n", id, valor, conta);
                contas[conta].fatura += valor;
                printf("Fatura atual: R$ %d\n",contas[conta].fatura);
                printf("Limite: R$ %d\n",contas[conta].limite);
            } else {
                printf("\nThread: %d Compra no crédito de R$ %d negada para a conta %d (limite insuficiente) \n", id, valor, conta);
            }
        }

    
    
    } else if (op == 3) {
        // Operação consulta
        pthread_mutex_lock(&mutex_global);
        while (contas[conta].consultas >= MAX_CONSULTAS) {
            pthread_cond_wait(&contas[conta].cond_consulta, &mutex_global);
        }
        contas[conta].consultas++;
        pthread_mutex_unlock(&mutex_global);

        printf("\nThread: %d Consultando saldo da conta %d\nSaldo: R$ %d\nFatura: R$ %d\n", id, conta, contas[conta].saldo, contas[conta].fatura);


        pthread_mutex_lock(&mutex_global);
        contas[conta].consultas--;
        pthread_cond_signal(&contas[conta].cond_consulta);
        pthread_mutex_unlock(&mutex_global);
    }

    pthread_mutex_unlock(&contas[conta].mutex_conta);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[n_threads];
    

    srand(time(NULL));
    printf("Seja bem-vindo ao Clube do Capitalismo Compulsivo(C3) \n");
    printf("Digite o número de contas: ");
    scanf("%d", &n_contas);

    contas = malloc(n_contas * sizeof(Conta_corrente));

    for (int i = 0; i < n_contas; i++) {
        contas[i].id = i;
        contas[i].saldo = (rand() % 10000) + 1;
        contas[i].limite = (rand() % 10) * 500 + 500;
        contas[i].fatura = 0;
        pthread_mutex_init(&contas[i].mutex_conta, NULL);
        pthread_cond_init(&contas[i].cond_consulta, NULL);
        contas[i].consultas = 0;
    }

    printf("\n----- Contas cadastradas -----\n\n");
    for (int i = 0; i < n_contas; i++) {
        printf("Conta: %d ", contas[i].id);
        printf("Saldo: R$ %d ", contas[i].saldo);
        printf("Limite: R$ %d \n", contas[i].limite);
    }
    printf("\n");

    for (int i = 0; i < n_threads; i++) {
        pthread_create(&threads[i], NULL, operacoes, (void*)&i);
    }

    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n");

    for (int i = 0; i < n_contas; i++) {
        printf("Conta: %d | ", contas[i].id);
        printf("Saldo: R$ %d | ", contas[i].saldo);
        printf("Fatura: R$ %d\n", contas[i].fatura);
        pthread_mutex_destroy(&contas[i].mutex_conta);
        pthread_cond_destroy(&contas[i].cond_consulta);
    }

    pthread_mutex_destroy(&mutex_global);
    free(contas);
    return 0;
}
