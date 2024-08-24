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