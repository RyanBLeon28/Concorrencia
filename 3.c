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

int n_contas; // número de contas que serão criadas
// pthread_mutex_t lock;
int n_threads = 50;

typedef struct
{
    int id;
    int saldo;
    int limite;
    int fatura;
    pthread_mutex_t acesso[5];
}Conta_corrente;


Conta_corrente *contas=NULL;

void* operacoes(void *arg){
    int conta = *(int*) arg;

    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&contas[conta].acesso[i]);
    }

    int op = (rand() % 2) +1;
    int saldo = contas[conta].saldo;
    int valor = (rand() % 1000) +1;
    
    
    switch (op)
    {
    case 1:
        // Operação debito
        if ((contas[conta].saldo - valor) > 0){
            printf("Conta %d está sofrendo uma operação de débito no valor de R$ %d \n", conta, valor);
            contas[conta].saldo -= valor;

            // printf("Saldo antigo: R$ %d \n", saldo);
            // printf("Saldo atual: R$ %d \n", contas[conta].saldo);
        }

        else{
            printf("Compra no débito de R$ %d negada para a conta %d (saldo insuficente) \n", valor, conta);
        }
        break;
    case 2:
        // Operação crédito
        if ((contas[conta].fatura + valor) < contas[conta].limite){
            printf("Conta %d está sofrendo uma operação de crédito no valor de R$ %d \n", conta, valor);
            contas[conta].fatura += valor;
        }
        else{
            printf("Compra no crédito de R$ %d negada para a conta %d (limite insuficente) \n", valor, conta);
        }
        break;
    
    }

    for (int i = 0; i < 5; i++) {
        pthread_mutex_unlock(&contas[conta].acesso[i]);
    }

    free(arg);
    pthread_exit(NULL);
}

void* consulta(void *arg){
    int aux = 0;
    int conta = *(int*) arg;
    while(aux < 5){
        if(pthread_mutex_trylock(&contas[conta].acesso[aux]) == 0){
            printf("O saldo da conta %d é: R$ %d \n", conta, contas[conta].saldo);
            pthread_mutex_unlock(&contas[conta].acesso[aux]);
            break;
        }
        else{
            aux++;
        }
    }
    free(arg);
    pthread_exit(NULL);
}

int main()
{
    pthread_t threads[n_threads],consultas[n_threads];


    srand(time(NULL));
    printf("Seja bem-vindo ao Clube do Capitalismo Compulsivo(C3) \n");
    printf("Digite o númnero de contas: ");
    scanf("%d", &n_contas);

    contas = malloc(n_contas * sizeof(Conta_corrente));

    // pthread_mutex_init(&lock, NULL);

    for(int i = 0; i<n_contas; i++){
        contas[i].id = i;
        contas[i].saldo = (rand() % 10000) +1;
        contas[i].limite = (rand() % 10) * 500 + 500;
        contas[i].fatura = 0;
        for (int j = 0; j < 5; j++)
        {
            pthread_mutex_init(&contas[i].acesso[j], NULL);
        }
        
    }

    printf("\n----- Contas cadastradas -----\n\n");

    for(int i = 0; i<n_contas; i++){
        printf("Conta: %d ", contas[i].id);
        printf("Saldo: R$ %d ", contas[i].saldo);
        printf("Limite: R$ %d \n", contas[i].limite);
    }
    printf("\n");

    for(int i=0; i<n_threads; i++){
        
        int *conta = malloc(sizeof(int));
        *conta = rand() % n_contas;
        pthread_create(&threads[i], NULL, operacoes, (void*)conta);
        
        int *conta_consulta = malloc(sizeof(int));
        *conta_consulta = rand() % n_contas;
        pthread_create(&consultas[i], NULL, consulta, (void*)conta_consulta);
    }

    
    for(int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    
    for (int i = 0; i < n_threads; i++)
    {
        pthread_join(consultas[i], NULL);
    }
    

    for(int i = 0; i<n_contas; i++){
        printf("Conta: %d ", contas[i].id);
        printf("Saldo: R$ %d | ", contas[i].saldo);
        printf("Fatura: R$ %d\n", contas[i].fatura);
    }

    // pthread_mutex_destroy(&lock);
    for (int i = 0; i < n_contas; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            pthread_mutex_destroy(&contas[i].acesso[j]);
        }
    }
    
    free(contas);
    return 0;
}
