/*
Universidade de Brasilia
Instituto de Ciencias Exatas
Departamento de Ciencia da Computacao
Algoritmos e Programaçao de Computadores – 2/2018
Aluno(a): Joao Francisco Gomes Targino
Matricula: 180102991
Turma: A
Versao do compilador: 6.3.0
Descricao: Este programa consiste em uma versao de puyo puyo em DIY feita por mim para o trabalho de APC da professora Carla Castanho
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifdef _WIN32/*Teste para ver o sistema operacional e pegar o CLEAR de acordo com o mesmo para limpar a tela*/
#else
    #include <termios.h>
#endif

#ifdef _WIN32/*Teste para ver o sistema operacional e pegar o CLEAR de acordo com o mesmo para limpar a tela*/
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

#ifndef _WIN32/*Checar o sistema operacional, e se estiver no windows implementar kbhit e getch, senao somente adicionar a biblioteca conio.h*/
    int kbhit(){
        struct termios oldt, newt;
        int ch, oldf;
        tcgetattr(STDIN_FILENO,&oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);
        if(ch != EOF){
            ungetc(ch,stdin);
        return 1;
        }
        return 0;
    }
    int getch(void) {
        int ch;
        struct termios oldt;
        struct termios newt;
        tcgetattr(STDIN_FILENO,&oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#else
    #include <conio.h>
#endif

/*Define as expressoes que muda a cor dos caracteres no teminal*/
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"
#define RED "\x1b[31m"
#define LGREEN "\x1b[92m" 	
#define LBLUE "\x1b[94m" 	
#define LMAGENTA "\x1b[95m" 
#define LCYAN "\x1b[96m" 
#define LYELLOW "\x1b[93m" 	
/*Variaveis Globais*/
typedef struct {
    char nick[11];
    int score;
}Player; /*struct do tipo player para o ranking*/

FILE *Replay;
FILE *Config;
char tabuleiro[1000][1000];
int altura=13, largura=8, perdeu=0;
int velocidade=720000, pontos=0, qntpecas=5 ;
char p1, p2;
int k=1, k1;/*Contadores de movimentaçao das pecas*/
int vetline[99], vetcolumn[99], iguais=0, destruir=4;/*Vetores auxiliares e contador para a quebra das pecas*/
int s, desapareceu = 0, rank=0;/*Variaveis de controle s=quando desce a peca de uma vez, desapareceu = quando as pecas quebraram, rank=ativa ou desativa modo rankeado*/


void corpecas(int i, int j){ /*Checa o char da matriz, e caso seja uma letra printa com uma cor*/
    if(tabuleiro[i][j] == 'A'){
        printf(RED "A" RESET);
    }
    else if(tabuleiro[i][j] == 'B'){
        printf(BLUE "B" RESET);
    }
    else if(tabuleiro[i][j] == 'C'){
        printf(GREEN "C" RESET);
    }
    else if(tabuleiro[i][j] == 'D'){
        printf(MAGENTA "D" RESET);
    }
    else if(tabuleiro[i][j] == 'E'){
        printf(CYAN "E" RESET);
    }
    else if(tabuleiro[i][j] == 'F'){
        printf(LGREEN "F" RESET);
    }
    else if(tabuleiro[i][j] == 'G'){
        printf(LBLUE "G" RESET);
    }
    else if(tabuleiro[i][j] == 'H'){
        printf(LCYAN "H" RESET);
    }
    else if(tabuleiro[i][j] == 'I'){
        printf(LMAGENTA "I" RESET);
    }
    else if(tabuleiro[i][j] == 'J'){
        printf(YELLOW "J" RESET);
    }
    else if(tabuleiro[i][j] == '+'){
        printf(LYELLOW "+" RESET);
    }
    else{
        printf("%c",tabuleiro[i][j]);
    } 
}

void printtab(){    /*Printa o tabuleiro*/
    int c=0, i=0;
    if (rank ==1 ){
        printf("RANKED"GREEN" ATIVADO\n"RESET);/*Mostra uma mensagem de para modo rankeado*/
    }
    else{
        printf("RANKED"RED" DESATIVADO\n"RESET);
    }
    printf("PONTOS:"YELLOW" %d\n\n"RESET, pontos);
    for (c=0; c<altura; c++){
        for(i=0; i<largura; i++){
            corpecas(c, i);
        }
        printf("\n");
    }
}

int Existe(int linha, int coluna){/*Verifica se a peca que eu estou comparando ja foi contada*/
    int c;
    for(c=0; c<iguais; c++){
        if (vetline[c] == linha && vetcolumn[c] == coluna){
            return 1;
        }
    }
    return 0;
}

void Contar_Iguais(int linha, int coluna){/*Verifica se 4 ou mais pecas iguais estao encostando*/
    char peca=tabuleiro[linha][coluna];
    if (iguais == 0){/*Se for a primeira peca a ser testada, somente aloca nos vetores auxiliares para saber que ja foi contada*/
        vetline[iguais] = linha;
        vetcolumn[iguais] = coluna;
        iguais++;
    }
    if((tabuleiro[linha+1][coluna] == peca) && (tabuleiro[linha+1][coluna] != ' ') && (Existe(linha+1, coluna) == 0)){
        vetline[iguais] = linha+1;                                  
        vetcolumn[iguais] = coluna;                             /*Verifica abaixo da peca*/
        iguais++;
        Contar_Iguais(linha+1, coluna);
    }
    if((tabuleiro[linha-1][coluna] == peca) && (tabuleiro[linha-1][coluna] != ' ') && (Existe(linha-1, coluna) == 0)){
        vetline[iguais] = linha-1;
        vetcolumn[iguais] = coluna;                             /*Acima*/
        iguais++;
        Contar_Iguais(linha-1, coluna);
    }
    if((tabuleiro[linha][coluna+1] == peca) && (tabuleiro[linha][coluna+1] != ' ') && (Existe(linha, coluna+1) == 0)){
        vetline[iguais] = linha;
        vetcolumn[iguais] = coluna+1;                           /*Lado direito*/
        iguais++;
        Contar_Iguais(linha, coluna+1);
    }
    if((tabuleiro[linha][coluna-1] == peca) && (tabuleiro[linha][coluna-1] != ' ') && (Existe(linha, coluna-1) == 0)){
        vetline[iguais] = linha;
        vetcolumn[iguais] = coluna-1;                           /*Lado esquerdo*/
        iguais++;
        Contar_Iguais(linha, coluna-1);
    }
    
    return;
}

void Quebra_Desce(){/*Desce as pecas que acima das que quebraram*/
    int x, y, c1, c;
    for (c1=0; c1<iguais; c1++){                               
        x = vetline[c1];                               /*x e y pegam dos vetores as linhas e colunas onde as pecas estavam*/ 
        y = vetcolumn[c1];                       /*e checa se ha peca acima disso e se abaixo ainda existe espaco caso precise descer de novo*/
        while(tabuleiro[x-1][y] != ' ' && tabuleiro[x][y] == ' '){
            tabuleiro[x][y] = tabuleiro[x-1][y];
            tabuleiro[x-1][y] = ' ';
            system(CLEAR);
            printtab();
            usleep(velocidade);
            c=x;
            while(tabuleiro[c+1][y] == ' ' && tabuleiro[c][y] != ' '){
                c++;
                tabuleiro[c][y] = tabuleiro[c-1][y];
                tabuleiro[c-1][y] = ' ';
                system(CLEAR);
                printtab();
                usleep(velocidade);
            }
            if(tabuleiro[x-2][y] != ' '){
                x--;
            }
        }
    }
}

void Destroy(){/*Tranforma as pecas em '+' e depois de mostrar na tela apaga e manda para as funcoes de descer as pecas que estavam em cima*/
    int x, y, c1;
    desapareceu=1;/*Variavel de controle avisa que foram quebradas pecas*/
    for(c1=0; c1<iguais; c1++){
        x = vetline[c1];
        y = vetcolumn[c1];
        tabuleiro[x][y] = '+';
    }
    system(CLEAR);
    printtab();
    usleep(velocidade);
    for(c1=0; c1<iguais; c1++){
        x = vetline[c1];
        y = vetcolumn[c1];
        tabuleiro[x][y] = ' ';
    }
    pontos += iguais;/*Cada peca igual da 1 ponto*/
    system(CLEAR);
    printtab();                            
    usleep(velocidade);                          
    Quebra_Desce();                         
}                                           

void Checar_Caida(int bonus){/*Verifica se na posicao onde as pecas sumiram descceram novas pecas e se sim manda para a funcao de contar e eventualmente a de destruir*/
    int c, x, y, quebradas = iguais;
    for(c=0;c<quebradas;c++){
        iguais=0;
        x = vetline[c];
        y = vetcolumn[c];
        Contar_Iguais(x, y);
        if(iguais >= destruir){
            Destroy();
            pontos += iguais*bonus;/*Adiciona um bonus de pontos*/
        }
    }
}

void Manda_Pecas(int x, int y){/*Recebe as posicoes de p1 e p2 quando elas param de cair*/
    int bonus=1;
    iguais=0;
    if(tabuleiro[x][y] != ' ' && tabuleiro[x][y] != '#'){ /*Manda para a funcao de contar quantas pecas iguais a ela exisem dos lados*/
        Contar_Iguais(x, y);                   /*Se existirem mais de x(definido nas configuracoes pelo usuario) pecas manda para o funcao de destruir as pecas*/
        if(iguais >= destruir){                                       
            Destroy();
            while(iguais >= 4){
                bonus++;  /*Se pecas forem destruidas checa as posicoes onde as pecas estavam*/
                Checar_Caida(bonus);
            }
        }
    }
}

/*A partir de agora, toda variavel w que aparecer na funcao diz se as pecas estao horizontais(w=0) ou verticais(w=1)*/

void descer_rapido(int w){ /*Funcao para caso 's' seja apertado, desce as pecas de uma vez sem reversao*/
    if(w){ /*Descida especifica para se a peca estiver na orientacao vertical*/
        while((w == 1) && (tabuleiro[k][k1+1] == ' ')){
                tabuleiro[k-2][k1+1] = ' ';
                tabuleiro[k-1][k1+1] = p1;
                tabuleiro[k][k1+1] = p2;
                k++;
                system(CLEAR);
                printtab();
                usleep(velocidade/10);
        }
    }
    else{ /*Descida para peca em posicao horizontal*/
        while ((tabuleiro[k][k1] == ' ')&&(tabuleiro[k][k1+1] == ' ')){/*Caso as duas pecas forem cair juntas*/
            tabuleiro[k-1][k1+1]=' ';
            tabuleiro[k-1][k1]=' ';
            tabuleiro[k][k1+1]=p2;
            tabuleiro[k][k1]=p1;
            k++;
            system(CLEAR);
            printtab();
            usleep(velocidade/10);
        }
        while ((tabuleiro[k][k1] == ' ')&&(tabuleiro[k][k1+1] != ' ')){/*Caso houver uma letra abaixo da p2*/
                tabuleiro[k-1][k1]=' ';
                tabuleiro[k][k1]=p1;
                k++;
                system(CLEAR);
                printtab();
                usleep(velocidade/10);
        }
        while ((tabuleiro[k][k1] != ' ')&&(tabuleiro[k][k1+1] == ' ')){/*Caso houver uma letra abaixo de p1*/
            tabuleiro[k-1][k1+1]=' ';
            tabuleiro[k][k1+1]=p2;
            k++;
            system(CLEAR);
            printtab();
            usleep(velocidade/10);
        }
    Manda_Pecas(k-1, k1);
    Manda_Pecas(k-1, k1+1);
    }
    s=1;/*Variavel de controle para a peca nao entrar na funcao de movimentacao novamente*/
}

int teclaw(int w){/*Chega se alguma tecla de movimentacao foi apertado caso a orientacao da peca seja vertical*/
    char tecla, aux;
    int c;
    if(w==1){
    for(c=0; c<5; c++){
        if (kbhit()){
                tecla=getch();
                if((tecla == 'd' || tecla == 'D') && (tabuleiro[k-1][k1+2] == ' ') && (tabuleiro[k-2][k1+2] == ' ')){/*Mexer as pecas para direita*/
                    k1++;
                    tabuleiro[k-1][k1] = ' ';
                    tabuleiro[k-2][k1] = ' ';
                    tabuleiro[k-2][k1+1]= p1;
                    tabuleiro[k-1][k1+1]= p2;
                    system(CLEAR);
                    printtab();
                }
                else if((tecla == 'a' || tecla == 'A') && (tabuleiro[k-1][k1] == ' ') && (tabuleiro[k-2][k1] == ' ')){/*Mexer as pecas para esquerda*/
                    k1--;
                    tabuleiro[k-1][k1+2] = ' ';
                    tabuleiro[k-2][k1+2] = ' ';
                    tabuleiro[k-2][k1+1]= p1;
                    tabuleiro[k-1][k1+1]= p2;
                    system(CLEAR);
                    printtab();
                    
                }
                else if(tecla == 's' || tecla == 'S'){/*Acelerar a descida das pecas*/
                    descer_rapido(w);
                    s=1;
                    break;
                }
                else if ((tecla == 'w' || tecla == 'W') && (tabuleiro[k-1][k1] == ' ')&& (tabuleiro[k][k1+1] == ' ')){/*Muda a orientacao para horizontal*/
                        w--;
                        tabuleiro[k-2][k1+1] = ' ';
                        break;
                }
                else if(tecla == 32){/*Troca as pecas de lugar*/
                    aux = p1;
                    p1 = p2;
                    p2 = aux;
                    system(CLEAR);
                    printtab();
                }
            }
            usleep(velocidade/5);
        }
    }
    return w;
}
      
void movew(int w){ /*Funcao de descida se a orientacao da peca for vertical*/
    while((w == 1) && (tabuleiro[k][k1+1] == ' ')){/*Checa abaixo da peca que esta mais em baixo e se for vazio desce as duas pecas*/
        tabuleiro[k-2][k1+1] = ' ';
        tabuleiro[k-1][k1+1] = p1;
        tabuleiro[k][k1+1] = p2;
        k++;
        system(CLEAR);
        printtab();
        w = teclaw(w);/*Ir para a funcao de checar tecla*/
    }
    Manda_Pecas(k-2, k1+1);
    Manda_Pecas(k-1, k1+1);
}

int teclas(){/*Chega se alguma tecla de movimentacao foi apertado caso a orientacao da peca seja horizontal*/
    int c, aux, w=0;
    char tecla;
    s=0;
    for(c=0; c<5; c++){
        if (kbhit()){
            tecla = getch();
            if (w == 0){
                if((tecla == 'd' || tecla == 'D') && (tabuleiro[k][k1+2] == ' ') && (tabuleiro[k][k1] == ' ') && (tabuleiro[k][k1+1] == ' ')){
                    k1++;                                               /*Mexer as pecas para direita*/
                    tabuleiro[k-1][k1-1] = ' ';
                    tabuleiro[k-1][k1]= p1;
                    tabuleiro[k-1][k1+1]= p2;
                    system(CLEAR);
                    printtab();
                }       
                else if((tecla == 'a' || tecla == 'A') && (tabuleiro[k][k1-1] == ' ') && (tabuleiro[k][k1] == ' ') && (tabuleiro[k][k1+1] == ' ')){
                    k1--;                                                   /*Mexer as pecas para esquerda*/
                    tabuleiro[k-1][k1+2] = ' ';
                    tabuleiro[k-1][k1+1] = p2;
                    tabuleiro[k-1][k1]= p1;
                    system(CLEAR);
                    printtab();
                }
                else if(tecla == 32){/*Trocar as pecas de lugar*/
                    aux = p1;
                    p1 = p2;
                    p2 = aux;
                    system(CLEAR);
                    printtab();
                }
                else if((tecla == 'w') || (tecla == 'W')){/*Trocar orientacao para vertical*/
                    if((w == 0) && (tabuleiro[k-2][k1+1] == ' ')){
                    w++;
                    tabuleiro[k-1][k1] = ' ';
                    tabuleiro[k-2][k1+1] = p1;  
                    system(CLEAR);
                    printtab();          
                    }
                }
                else if((tecla == 's') || (tecla == 'S')){/*Acelerar a descida das pecas e quebrar a funcao de pegar mais teclas*/
                    descer_rapido(w);
                    break;
                }
            }
            if (w == 1){
                movew(w);/*Ir para a funcao de mover pecas verticais*/
            }
        }
        usleep(velocidade/5);
    }
    return w;
}

        
void moverpecas(){/*Verifica as pecas vazias para descer ou nao a peca que foi gerada*/
    int w;
    k=1;/*Muda a variavel global de linha para comecar da linha 1 do tabuleiro*/
    s=0;/*Volta a variavel de controle de descer rapido para o padrao*/
    desapareceu=0;
    while ((tabuleiro[k][k1] == ' ')&&(tabuleiro[k][k1+1] == ' ') && s != 1){ /*Caso as pecas forem descer juntas (horizonal)*/
        tabuleiro[k-1][k1+1]=' ';
        tabuleiro[k-1][k1]=' ';
        tabuleiro[k][k1+1]=p2;
        tabuleiro[k][k1]=p1;
        k++;
        system(CLEAR);
        printtab();
        w = teclas();
        if (w == 1 && desapareceu == 1){/*Quebra a funcao de descer caso as orientacao seja vertical, e a peca tenha sido destruida para mandar outra peca*/
            break;
        }
    }
    if (w != 1 && s!= 1){
        while ((tabuleiro[k][k1] == ' ')&&(tabuleiro[k][k1+1] != ' ')){/*Caso a p1 fique solta e a p2 nao (horizontal)*/
            tabuleiro[k-1][k1]=' ';
            tabuleiro[k][k1]=p1;
            system(CLEAR);
            printtab();
            k++;
            usleep(velocidade);
        }
        while ((tabuleiro[k][k1] != ' ')&&(tabuleiro[k][k1+1] == ' ')){/*Caso a p2 fique solta e a p1 nao (horizontal)*/
            tabuleiro[k-1][k1+1]=' ';
            tabuleiro[k][k1+1]=p2;
            k++;
            system(CLEAR);
            printtab();
            usleep(velocidade);
        }
    Manda_Pecas(k-1, k1);
    Manda_Pecas(k-1, k1+1); /*Manda as posicoes onde as pecas pararam para verificacao de iguais ao seu redor*/  
    }
}

void tab(){/*Pega altura e largura que sao dadas nas configuraçoes ou no inicio do jogo, caso as configuraçoes nao mudem*/
    int c=0, i=0;
    for(c=0; c<altura; c++){
        for(i=0; i<largura; i++){ /*Aloca '#' nas primeiras e ultimas colunas, e na ultima linha do tabuleiro e ' ' no resto*/
            if((c == altura-1)){
                tabuleiro[c][i] = '#';
            }
            else if((i == 0)){
                tabuleiro[c][i] = '#';
            }
            else if((i == largura-1)){
                tabuleiro[c][i] = '#';
            }
            else{
                tabuleiro[c][i] = ' ';
            }
        }
    }
}

void criarchar(){/*Randomiza as duas pecas*/
    p1 = 'A' + rand() % (qntpecas);
    p2 = 'A' + rand() % (qntpecas);
}

void Jogo(int replay){
    int posinicial, perdeu=0;
    char arquivo[20];
    pontos =0;
    tab();/*carrega o tabuleiro*/
    if(replay==2){/*Se o usuario quiser jogar um replay ja existente deve-se digitar o nome do arquivo e serao carregadas as pecas*/
        printf("Digite o nome do arquivo de replay desejado com a extensao \".txt\" no final\n");
        scanf("%s",arquivo);
        if((Replay = fopen(arquivo,"r")) == NULL){/*Se o arquivo não existir, volta ao menu*/ 
            printf("Arquivo nao existente, voltando para o menu\n");
            usleep(5000000);
            return;
        }
        
    }
    while (!perdeu){
        posinicial = 1 + (rand() % (largura-3));/*Calcula a coluna inicial do primeiro caracter, e coloca o segundo a direita*/
        if (tabuleiro[0][posinicial] != ' '||tabuleiro [0][posinicial+1] != ' '){
            perdeu=1;               /*Se a coluna de p1 ou p2 conicidir com alguma peca que esta no topo do tabuleiro a pessoa perde*/
        }
        else{/*Se nada existir nas colunas iniciais as pecas sao geradas, colocadas nas colunas e sao mandadas para as funcoes de movimentacao*/
            if(rank==0){
                if(replay == 1){/*Se o usuario quiser criar um replay as pecas sao salvas em um arquivo de texto toda vez que sao criadas*/
                    criarchar();
                    fprintf(Replay,"%c %c\n", p1, p2);
                }
                else if(replay == 2){/*quando o usuario quer jogar um  replay existente, o jogo acaba quando as pecas no arquivo acabam*/ 
                    if(feof(Replay)){
                        perdeu=1;
                        fclose(Replay);
                    }
                    else{
                    fscanf(Replay,"%c %c\n", &p1, &p2);
                    }
                }
            }
            else{    
                criarchar();/*Se o modo rankeado estiver ativado nenhum arquivo de texto é aberto*/
            }
            if(!perdeu){
                tabuleiro[0][posinicial] = p1;
                tabuleiro[0][posinicial+1]= p2;
                system(CLEAR);
                printtab();
                usleep(720000);
                k1=posinicial;/*Variavel glogal de coluna recebe as colunas iniciais*/
                moverpecas();
            }
        }
    }
    system(CLEAR);
    printtab();
    printf("\n");
    printf("Voce perdeu, boa sorte na proxima");
    usleep(5000000);
    getch();
}


void setConfig(){/*Se existir um arquivo de texto com configuracoes, seus numeros sao carregados nas variveis globais do jogo*/
    Config = fopen("config.txt","r");
        if(Config != NULL){
            fscanf(Config,"%d %d %d %d %d", &altura, &largura, &qntpecas, &destruir, &velocidade);
        }
    fclose(Config);
}

void naorank(){/*Caso modo rankeado desativado, serao dadas as opcoes de criar ou abrir um txt para replay*/
    char file[100];
    int replay=0;
    printf("Digite uma opcao a seguir:\n\n");
        printf("1 - Criar um novo replay\n");
        printf("2 - Jogar um replay ja criado\n\n");
        scanf("%d", &replay);
        system(CLEAR);
        switch(replay){
            case 1:/*Abre ou criar um arquivo de texto para guardar as pecas*/
                setConfig();
                replay=1;
                printf("Digite o nome desejado para o arquivo que sera criado.\n Obs.: coloque \".txt\" no final:\n");
                scanf("%s",file);
                Replay = fopen(file,"w");
                Jogo(replay);
                fclose(Replay);
                return;
            break;

            case 2:/*Abre um arquivo de texto para leitura das pecas*/
                setConfig();
                replay=2;
                Jogo(replay);
                return;
            break;

            default:
                naorank();
            break;
    }
} 

void OrdenaRanking(Player *rankeada, int n){/*Recebe um vetor de tipo Player e ordena de acordo com a pontuacao*/
    int i, j;
    Player aux;
    for(i=0; i<n; i++){
        for(j=i+1; j<n; j++){
            if(rankeada[j].score > rankeada[i].score){
                aux=rankeada[j];
                rankeada[j]=rankeada[i];
                rankeada[i]=aux;
            }
        }
    }
}

void registerMatch(char* nickname){/*Quando um jogo no modo rankeado acaba, tenta alocar a nova pontuacao no arquivo binario*/
    int c=0;
    FILE *bin;
    Player rankeada[10];
    bin = fopen("ranking.bin","rb");
        if(bin == NULL){/*Se ainda nao existir um binario abre um para escrita, aloca pontuacao e nome na primeira posicao do vetor*/
            bin = fopen("ranking.bin","wb");
                strcpy(rankeada[0].nick, nickname);
                rankeada[0].score= pontos;
                for(c=0; c<10;c++){
                    if(c!=0){
                        strcpy(rankeada[c].nick, "");/*Aloca /0 como nick, e 0 como pontuacao em todos as outras 9 posicoes*/
                        rankeada[c].score = 0;
                    }
                    fwrite(&rankeada[c], sizeof(Player), 1, bin);
                }
            fclose(bin);
        }
        else{/*Se ja existe um binario, abre para leitura*/
            for(c=0;c<10;c++){
                fread(&rankeada[c], sizeof(Player), 1, bin);
            }
            if (rankeada[9].score < pontos){/*Se a ultima pontuacao contida no vetor for menor que a do jogo atual, ela é descartada e a nova entra no vetor*/
                strcpy(rankeada[9].nick, nickname);
                rankeada[9].score= pontos;
            }
        }
        OrdenaRanking(rankeada, 10);/*Quando sair de qualquer um dos casos acima, o vetor do tipo Player é ordenado e gravado no arquivo binario*/
        bin = fopen("ranking.bin","wb");
        for(c=0; c<10;c++){
            fwrite(&rankeada[c], sizeof(Player), 1, bin);
        }
    fclose(bin);
}

void simrank(){/*Se modo rankeado ativado, e salvo o nick do jogador*/
    char nickname[100];
    printf("Digite seu nickname com no maximo 10 caracteres\n");
        scanf("%s", nickname);
        while(strlen(nickname)>10){
            printf("Nickname muito longo, digite novamente\n");
            scanf("%s", nickname);
        }
        altura=15;/*Variaveis sao mudadas para o padrao de rankeada*/
        largura=9;
        qntpecas=7;
        destruir=5;
        velocidade=720000;
        Jogo(0);
        registerMatch(nickname);/*Depois de acabado, manda para ver se a pontuacao entra no top 10 do ranking*/
        return;
}

void config(){
    system(CLEAR);
    printf("CONFIGURACOES:\n\n");
    int op;
    if (rank ==1 ){
        printf("RANKED"GREEN" ATIVADO\n"RESET);/*Mostra uma mensagem de para modo rankeado*/
    }
    else{
        printf("RANKED"RED" DESATIVADO\n"RESET);
    }
    Config= fopen("config.txt","w");/*Coloca no arquivo de texto as configuracoes, depois de sair dos cases*/ 
            fprintf(Config,"%d %d %d %d %d", altura, largura, qntpecas, destruir, velocidade);
    fclose(Config);/*caso nao seja nada mudado, as variaveis globais estao setadas como padrao, e entram no arquivo de texto*/

    printf("Digite algum dos numeros a seguir para alterar as configuracoes\n\n");
    printf("1 - Tabuleiro\n");
    printf("2 - Pecas\n");
    printf("3 - Ativar modo rankeado\n");
    printf("4 - Voltar\n");
    scanf("%d",&op);
    switch(op){
        case 1:
            system(CLEAR);
            printf("Digite o tamanho do tabuleiro desejado, primeiro a altura e depois a largura\n");
            printf("Minimo de cada um dos parametros = 5\n");
            scanf("%d %d", &altura, &largura); /*Muda as variaveis de tamnaho do tabuleiro*/
            while (largura<5||altura<5){
                system(CLEAR);
                printf("Digite o tamanho do tabuleiro desejado, primeiro a altura e depois a largura\n");
                printf("Minimo de cada um dos parametros = 5\n");
                scanf("%d %d", &altura, &largura);
            }
            config();
        break;

        case 2:
            system(CLEAR);
            printf("Digite a variedade de pecas que voce deseja que o jogo tenha:\n");
            printf("Minimo= 2 pecas; Maximo = 10 pecas; Padrao = 5 pecas\n");
            scanf("%d", &qntpecas);/*Muda a variedade de pecas que aparecerao no jogo*/
            while (qntpecas<2 || qntpecas>10){
                system(CLEAR);
                printf("Quantidade invalida\n");
                printf("Digite a variedade de pecas que voce deseja que o jogo tenha:\n");
                printf("Minimo= 2 pecas; Maximo = 10 pecas; Padrao = 5 pecas");
                scanf("%d", &qntpecas);
            }

            system(CLEAR);
            printf("Digite a quantidade de pecas que deverao ficar conectadas para que a pontuacao seja dada\n");
            printf("Minimo= 3 pecas; Maximo = 10 pecas; Padrao = 4 pecas\n");
            scanf("%d",&destruir);/*Muda quantas pecas conectadas fazem pontos no jogo*/
            while (destruir<3 || destruir>10){
                system(CLEAR);
                printf("Quantidade invalida\n");
                printf("Digite a quantidade de pecas que deverao ficar conectadas para que a pontuacao seja dada\n");
                printf("Minimo= 3 pecas; Maximo = 10 pecas; Padrao = 4 pecas");
                scanf("%d", &destruir);
            }

            system(CLEAR);
            printf("Digite o tempo de atualizaçao do tabuleiro em microssegundos (velocidade de caida das pecas)\n");
            printf("Padrao = 720000 microssegundos\n");
            scanf("%d",&velocidade);/*Muda a velocidade que as pecas caem no tabuleiro*/
            config();
        break;

        case 3:
            system(CLEAR);
            printf("Modo rankeado\n\n");
            printf("Digite 1 para ativar e 0 para desativar\n");
            scanf("%d", &rank);/*Ativa ou dessativa o rank*/
            while (rank > 1 || rank < 0){
                system(CLEAR);
                printf("Modo rankeado\n\n");
                printf("Digite 1 para ativar e 0 para desativar\n");
                scanf("%d", &rank);
            }
            config();
        break;

        case 4:
            return;
        break;

        default:
            config();
        break;
    }
}

void menu(){
    int op;
    system(CLEAR);
    if (rank ==1 ){
        printf("RANKEAD"GREEN" ATIVADO\n"RESET);/*Mostra uma mensagem de para modo rankeado*/
    }
    else{
        printf("RANKED"RED" DESATIVADO\n"RESET);
    }
    printf("1 - Jogar\n");
    printf("2 - Intrucoes\n");
    printf("3 - Configuracoes\n");
    printf("4 - Ranking\n");
    printf("5 - Sair\n");
    printf("Digite o numero correspondente a opcao desejada:\n");
    scanf("%d",&op);
    switch(op) {
        case 1:/*Se a opcao 1 for escolhida e checado a flag de rankeada, caso esteja ativado vai para funcao especifica de jogo rankeado, caso nao, vai para funcao de replay*/
            system(CLEAR);
            if(rank==0){
                naorank();
            }
            else{
                simrank(); 
            }
            menu();
        break;

        case 2:/*Se digitada a opçao 2 serao dadas as instruçoes*/
            system(CLEAR);
            printf("INSTRUCOES:\n");
            printf("\n");
            printf("Os controles sao:\n");
            printf("W - muda a orientacao das pecas // A - move a peca para esquerda //\n D - move a peca para direita // S - desce a peca de uma vez na coluna que ela estiver, sem reversao\n");
            printf("Espaco = troca a posicao das duas pecas entre si\n\n");
            printf("As pecas destroem quando voce junta 4 ou mais, e cada peca te da um ponto\n");
            printf("Caso depois das pecas detruirem e as que tiverem acima dessa cairem e tambem tiverem uma formacao de 4 ou mais,\n voce ganha dois pontos por cada uma dessas pecas novas destruidas\n\n");
            printf("As pecas sao geradas em colunas aleatorias da primeira linha do tabuleiro,\n entao o jogo acabara quando a coluna gerada de uma das pecas ja estiver ocupada por outra\n");
            printf("\n");
            printf("Tecla <ENTER> para continuar!");
            getch();
            getch();
            menu();
        break;

        case 3:/*Se digitada a opçao 3 as configuraçoes serao abertas*/
            config();
            menu();
        break;

        case 4:/*Se digitada a opçao 4 sera mostrado o rank*/
            system(CLEAR);
            int c;
            FILE *bin;
            Player rankeada[10];
            bin = fopen("ranking.bin","rb");
                printf(RED"\t\t  RANKING\n"RESET);
                if (bin == NULL){/*Mostra a mensagem dizendo que nao ha jogos no ranking*/
                    printf("Ainda não existe nenhuma pontuacao maior que 0 que mereceu o ranking");
                    getch();
                    getch();
                }
                else{/*Carrega o binario de ranking para leitura e mostra nick e pontuacao*/
                    bin = fopen("ranking.bin","rb");
                        for(c=0;c<10;c++){
                            fread(&rankeada[c], sizeof(Player), 1, bin);
                        }
                    fclose(bin);
                    for(c=0;c<10;c++){
                        if(strlen(rankeada[c].nick) > 0){
                            if(c==0){
                                printf("\n\n");
                                printf(YELLOW"\t\t  %s %dPTS\n",rankeada[c].nick,rankeada[c].score);
                                printf(YELLOW"\t\t  _______\n");
                                printf("\t\t |       |\n");
                                printf("\t\t(|       |)\n");
                                printf("\t\t |   #1  |\n");
                                printf("\t\t  \\     /\n");
                                printf("\t\t    --- \n");
                                printf("\t\t   _|_|_\n\n"RESET);
                            }  
                            else{
                                printf("\t\t#%d-", c+1);
                                printf(YELLOW"%s  %dPTS\n"RESET,rankeada[c].nick, rankeada[c].score);
                            }
                        }
                    }
                }
                usleep(1000000);
                printf("\nTecla <ENTER> para continuar!");
                getch();
                getch();
            menu();
        break;

        case 5:/*E por fim se a opçao for 5 o jogo fechara*/
            exit(0);
        break;

        default:
            menu();
        break;
    }
}

void Nome_Jogo(){/*escreve o nome do jogo e espera o jogador digitar*/
    system(CLEAR);
    printf("\n");
    printf("\n");
    printf(RED "       ============" BLUE "  ========="CYAN"   ========="YELLOW" ============"MAGENTA" ===="GREEN" =========\n");
    printf(RED"            ||      "BLUE" ||     //   "CYAN"||          "YELLOW"   ||       "MAGENTA"||"GREEN"  ||       \n");
    printf(RED"            ||      "BLUE" ||    //    "CYAN"||          "YELLOW"   ||       "MAGENTA"||"GREEN"  ||        \n");
    printf(RED"            ||      "BLUE" ||  \\\\    "CYAN"  ||======  "YELLOW"     ||     "MAGENTA"  ||"GREEN"  =========\n");
    printf(RED"            ||      "BLUE" ||   \\\\   "CYAN"  ||        "YELLOW"     ||     "MAGENTA"  ||"GREEN"         ||\n");
    printf(RED"            ||      "BLUE" ||    \\\\  "CYAN"  ||        "YELLOW"     ||     "MAGENTA"  ||"GREEN"         ||\n");
    printf(RED"            ||      "BLUE" ||     \\\\ "CYAN"  ========= "YELLOW"     ||     "MAGENTA" ===="GREEN" =========\n"RESET);
    printf("\n");
    printf("\t\t Digite qualquer tecla para ir ao menu"); 
    while( kbhit() != 1){
    }
}

int main(){
    srand(time(0));
    Nome_Jogo(); /*Abre a funcao que printa o nome do Jogo na tela*/
    menu();
    return 0;
}
