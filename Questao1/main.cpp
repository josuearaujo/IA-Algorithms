#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

#define POP 20      //Define o tamanho da população. Pode ser editado.
#define ITERACOES 100 //Define a quantidade de iterações. Pode ser editado.

struct Individuo{
    int fitness;    // armazena o valor do fitness de cada invidivíduo
    int caminho[5]; // armazena o caminho do indivíduo
    float peso;     // armazena um valor auxiliar que será utilizado na seleção (usando roleta)
};


void gerarPopulacaoAleatoriamente(Individuo populacao [POP]);  //função para inicializar a população com indivíduos aleatórios
void imprimePopulacao(Individuo populacao[POP], int tam);      //função criada para imprimir a população de indivíduos na tela, se necessário
int fitnessFunction(int caminho[5], int dados[6][6]);          //função fitness para avaliar cada indivíduo
void ordenar(Individuo populacao[POP]);                        //função para ordenar crescentemente os indivíduos, levando em consideração o fitness de cada um
void selecao(Individuo populacao[POP], Individuo pais[], int k);//função para selecionar os pais, utilizando o método da roleta
void cruzamento(Individuo pais[], Individuo filhos[], int k);   //função para cruzar os pais, gerando os filhos para a próxima geração
void mutacao(Individuo filhos[], int k);                        //função para efetuar mutação em um determinado indivíduo
void atualizaGeracao(Individuo populacao[POP], Individuo filhos[], int k); //função para atualizar a população com os novos indivíduos

void entrada_dados(int dados[6][6]); //função para entrar com os dados disponibilizados na prova


int main()
{
    int dados[6][6];        //Matriz para armazenar a entrada de dados. Por mais que só fosse necessário uma matriz 5x5,
                            //optamos por criar uma 6x6 para que os elementos da matriz fossem indexados conforme as cidades
                            //foram dispostas na prova. Por exemplo: A distancia entre a cidade 1 e 3 pode ser acessada atraves do elemento
                            // dados[1][3] da nossa matriz.

    entrada_dados(dados);   //entrada de dados

    int k = (POP-2);  //NUMERO DE FILHOS PARA A PROXIMA GERAÇÃO, TENDO EM VISTA QUE OS 2 MELHORES
                      //DA GERAÇÃO ATUAL SERÃO MANTIDOS POR ELITISMO

    Individuo populacao[POP];                       //vetor que armazena a população
    gerarPopulacaoAleatoriamente(populacao);        //gera população aleatoriamente

    int it=1;    //contador de iterações
    while(it<=ITERACOES){
        //AVALIAR A POPULAÇÃO
        for(int i=0; i<POP; i++){
            populacao[i].fitness = fitnessFunction(populacao[i].caminho, dados); //avalia toda população, individuo por individuo e armazena no atributo da estrutura criada para isso
        }

        //ORDENAR EM FUNÇÃO DO FITNESS DE CADA INDIVIDUO
        ordenar(populacao); //Ordena a população para que os melhores elementos de cada geração fiquem no início do vetor
                            //Com isso, para fazermos o elitismo só precisamos manter os primeiros elementos do vetor.

        //SELEÇÃO
        Individuo pais[k];     //Vetor que irá armazenar os pais selecionados em cada geração
        Individuo filhos[k];   //Vetor que irá armazenar os filhos gerados em cada geração
        selecao(populacao, pais, k);  //Chamada a função para selecionar os pais através do método da roleta

        cruzamento(pais, filhos, k);  //Faz o cruzamento utilizando a tecnica "um ponto"

        mutacao(filhos, k);         //Faz a mutação invertendo a posição de dois nós selecionados aleatóriamente
        atualizaGeracao(populacao, filhos, k); //atualiza a geração com os novos indivíduos gerados


        it++; //incrementa o contador de iterações
    }

    //Imprime a solução encontrada
    cout << "O MELHOR DE TODOS OS CAMINHOS EH: " << endl;
    cout << populacao[0].caminho[0] << " " << populacao[0].caminho[1] << " " <<populacao[0].caminho[2] << " ";
    cout << populacao[0].caminho[3] << " " << populacao[0].caminho[4] << endl;
    cout << populacao[0].fitness << endl;

    return 0;
}

void gerarPopulacaoAleatoriamente(Individuo populacao[10]){ //FUNÇÃO PARA GERAR ALEATORIAMENTE A POPULAÇÃO COM INDIVÍDUOS VÁLIDOS, OU SEJA, QUE NÃO TENHAM NÓS REPETIDOS
    srand(time(NULL));
    int numRand;
    for(int i=0; i<POP; i++){
        int verificacao[6] = {};
        for(int j=0; j<5; j++){
            numRand = (rand()%5)+1;
            if(verificacao[numRand]==0){
                verificacao[numRand]++;
                populacao[i].caminho[j] = numRand;
            }
            else{
                j--;
            }
        }
    }
}

void imprimePopulacao(Individuo populacao[], int tam){ //FUNÇÃO AUXILIAR CASO DESEJE IMPRIMIR A POPULAÇÃO NA TELA
    for(int i=0; i<tam; i++){
        cout << "( ";
        for(int j=0; j<5; j++){
            cout << populacao[i].caminho[j];
            if(j==4){
                cout << " )" << endl;
            }
            else{
                cout << " , ";
            }
        }
    }
}

int fitnessFunction(int caminho[5], int dados[6][6]){ //FUNÇÃO DE AVALIAÇÃO QUE SOMA O VALOR DAS ARESTAS DO GRAFO
    int soma=0;
    for(int i=0;i<4;i++){
        soma += dados[caminho[i]][caminho[i+1]];
    }
    return soma;
}


void ordenar(Individuo populacao[POP]){ //FUNÇÃO PARA ORDENAR O VETOR, MANTENDO OS INDIVÍDUOS COM MELHOR FITNESS NO INÍCIO DO VETOR POPULAÇÃO
    Individuo aux;
    for(int i=0; i<POP; i++){
        for(int j=i; j<POP; j++){
            if(populacao[j].fitness < populacao[i].fitness){
                aux = populacao[i];
                populacao[i] = populacao[j];
                populacao[j] = aux;
            }
        }
    }
}

void selecao(Individuo populacao[POP], Individuo pais[], int k){ //METODO ROLETA
    srand(time(NULL));
    float soma = 0;
    for(int i=0; i<POP; i++){
        soma += float(1)/populacao[i].fitness;
    }
    for(int i=0; i<POP; i++){
        populacao[i].peso = 1000*((float(1)/populacao[i].fitness)/soma);
    }
    int auxRand;
    for(int i=0; i<k; i++){
        auxRand = rand()%1000;
        float auxSoma=0;
        int j;
        for(j=0; j<POP; j++){
            auxSoma+=populacao[j].peso;
            if(auxSoma>auxRand){
                break;
            }
        }
        pais[i] = populacao[j];
    }
}

void cruzamento(Individuo pais[], Individuo filhos[], int k){
    srand(time(NULL));
    //GERAR OS FILHOS, AINDA COM REPETIÇÃO DE CIDADES
    for(int i=0; i<k; i=i+2){
        //1º filho do cruzamento
        filhos[i].caminho[0] = pais[i].caminho[0];
        filhos[i].caminho[1] = pais[i].caminho[1];
        filhos[i].caminho[2] = pais[i].caminho[2];
        filhos[i].caminho[3] = pais[i+1].caminho[3];
        filhos[i].caminho[4] = pais[i+1].caminho[4];
        //2º filho do cruzamento
        filhos[i+1].caminho[0] = pais[i+1].caminho[0];
        filhos[i+1].caminho[1] = pais[i+1].caminho[1];
        filhos[i+1].caminho[2] = pais[i+1].caminho[2];
        filhos[i+1].caminho[3] = pais[i].caminho[3];
        filhos[i+1].caminho[4] = pais[i].caminho[4];
    }


    //VERIFICAR SE EXISTEM CIDADES REPETIDAS E CORRIGIR
    int matVerificacao[k][5];   //MATRIZ AUXILIAR, INICALIZADA COM ZEROS, PARA ARMAZENAR A QUANTIDADE DE CIDADES DIFERENTES EM CADA INDIVIDUO
                                //POR EXEMPLO: O CAMINHO (1, 3, 2, 5, 4), APÓS SER VERIFICADO, TERÁ COMO RESULTADO NA
                                //MATRIZ DE VERIFICAÇÃO UM VETOR DA SEGUINTE FORMA: (1, 1, 1, 1, 1)
                                //QUE INDICA QUE CADA CIDADE APARECE UMA VEZ NO INDIVÍDUO.
                                //OUTRO EXEMPLO: O CAMINHO(4, 4, 5, 5, 5), APÓS SER VERIFICADO, TERÁ COMO RESULTADO NA
                                //MATRIZ DE VERIFICAÇÃO UM VETOR DA SEGUINTE FORMA: (0, 0, 0, 2, 3)
                                //QUE INDICA QUE EXISTEM DOIS NÓS 4 REPETIDOS E TRÊS NÓS 5 REPETIDOS
                                //A PARTIR DAÍ É FEITA A CORREÇAÕ PARA QUE ESSE INDIVÍDUO INVÁLIDO SE TORNE UM INDIVÍDUO DO TIPO (1, 1, 1, 1, 1), OU SEJA, NÃO TENHA NÓS REPETIDOS.
    for(int i=0; i<k; i++){
        for(int j=0; j<5; j++){
            matVerificacao[i][j] = 0;
        }
    }
    for(int i=0; i<k; i++){
        for(int j=0; j<5; j++){
            matVerificacao[i][filhos[i].caminho[j]-1]++;
        }
    }

    for(int i=0; i<k; i++){
        for (int j=0; j<5; j++){
            if(matVerificacao[i][j]==0){
                int auxRand;
                do{
                    auxRand = rand()%5;
                }while(matVerificacao[i][auxRand]<=1);
                matVerificacao[i][auxRand]--;
                matVerificacao[i][j]++;
                for(int w=0; w<5; w++){
                    if(filhos[i].caminho[w]==(auxRand+1)){
                        filhos[i].caminho[w]=j+1;
                        break;
                    }
                }
            }
        }
    }

}


void mutacao(Individuo filhos[], int k){ //GERA DOIS NÚMEROS ALEATÓRIOS QUE SERÃO OS INDICES DOS NÓS QUE SERÃO INVERTIDOS
    srand(time(NULL));
    int auxRand = rand()%k; //Sortear o filho que sofrerá mutação

    int c1 = rand()%5; // c1 e c2 indicam as cidades que serão invertidas
    int c2;
    do{
        c2 = rand()%5;
    }while(c1==c2); //CONDIÇÃO PARA GARANTIR QUE ELE IRÁ DE FATO INVERTER DOIS NÓS, OU SEJA, NÃO PERMITE QUE SEJAM GERADOS DOIS NÚMEROS ALEATÓRIOS IGUAIS
    int aux = filhos[auxRand].caminho[c1];
    filhos[auxRand].caminho[c1] = filhos[auxRand].caminho[c2];
    filhos[auxRand].caminho[c2] = aux;
}

void atualizaGeracao(Individuo populacao[POP], Individuo filhos[], int k){ //ATUALIZA A POPULAÇÃO PARA A PRÓXIMA ITERAÇÃO
    for(int i=0; i<k; i++){
        populacao[i+2] = filhos[i];
    }
}




void entrada_dados(int dados[6][6]){ //Dados de entrada fornecidos na prova
    dados[1][1] = 0;
    dados[1][2] = 2;
    dados[1][3] = 9;
    dados[1][4] = 3;
    dados[1][5] = 6;
    dados[2][1] = 2;
    dados[2][2] = 0;
    dados[2][3] = 4;
    dados[2][4] = 3;
    dados[2][5] = 8;
    dados[3][1] = 9;
    dados[3][2] = 4;
    dados[3][3] = 0;
    dados[3][4] = 7;
    dados[3][5] = 3;
    dados[4][1] = 3;
    dados[4][2] = 3;
    dados[4][3] = 7;
    dados[4][4] = 0;
    dados[4][5] = 3;
    dados[5][1] = 6;
    dados[5][2] = 8;
    dados[5][3] = 3;
    dados[5][4] = 3;
    dados[5][5] = 0;

    /*  DESCOMENTE ESSA PARTE DO CÓDIGO SE QUISER VER A REPRESENTAÇÃO EM FORMA DE MATRIZ DA ENTRADA
    for (int i = 1; i<=5; i++){
        for(int j = 1; j<=5; j++){
            cout  << dados[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
    */
}
