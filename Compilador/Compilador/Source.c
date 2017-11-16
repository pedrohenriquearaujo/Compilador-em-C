#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#define tam 256
#define true 1

//RESERVADA
#define MAIN 0
#define IF 1
#define  ELSE 2
#define  WHILE 3
#define DO 4
#define  FOR 5
#define  INT 6
#define  FLOAT 7
#define CHAR 8
#define ID 9
//DIGITO
#define V_INT 10
#define V_FLOAT 11
#define V_CHAR 12
//ESPECIAL
#define PARENTESES_ESQUERDO 13
#define PARENTESES_DIREITO 14
#define CHAVE_ESQUERDA 15
#define CHAVE_DIREITA 16
#define VIRGULA 17
#define PONTO_VIRGULA 18
//ARITMETRICO
#define SOMA 19
#define SUB 20
#define MULT 21
#define DIV 22
#define ATRIBUICAO 23
//RELACIONAL
#define MENOR 24
#define MAIOR 25
#define MENOR_IGUAL 26
#define MAIOR_IGUAL 27
#define IGUALDADE 28
#define DIF 29
//ERROS
#define ERRO_FLOAT 30
#define ERRO_CHAR 31
#define ERRO_EXCLAMACAO 32
#define ERRO_COMENTARIO 33
//CARACTER
#define CARACTER_INVALIDO 34

typedef struct lista {

	char identificador[tam];
	int code;
	int escopo;
	struct lista *prox;

}Tlista;

Tlista * tabela_simbolos = NULL;

typedef struct token{

	int code;
	char identificador[tam];

}Ttoken;

void Abrir_Arquivo(char nome[] , FILE ** arq);
Ttoken * scan(FILE * arq, int * linha, int * coluna);
int Palavra_Reversada( char buffer[], int pos_buffer);
Ttoken * simbolos(Ttoken * t,char l_h);
void leitura(FILE * arq,char * l_h, int * linha, int * coluna, char buffer[], int * pos_buffer);
int verificar_id( char l_h , FILE * arq);
int verificar_numero( char l_h , FILE * arq );


int consultar(char identificador[]);
int verificar_tipo(Ttoken * t, int *linha, int *coluna);
void listar( Tlista ** tabela );
Tlista * criar_no_tabela( Tlista ** novo, char identicidador[], int tipo );
void inserir(char identicidador[], int tipo);
void remover(Tlista ** tabela);
void compatibilidade(Ttoken ** t, int tipo_direito, int tipo_esquerdo, int * linha, int * coluna);
int buscar_variavel(char idenficador[]);


void programa(FILE * arq, int * linha, int * coluna);
void parser(FILE * arq, int * linha, int * coluna);

void bloco(Ttoken ** t,FILE * arq, int * linha, int * coluna);
void declara_variavel(Ttoken ** t,FILE * arq, int * linha, int * coluna);
void atribuicao(Ttoken ** t,FILE * arq, int * linha, int * coluna);
void comando (Ttoken ** t,FILE * arq, int * linha, int * coluna);
void comando_basico (Ttoken ** t,FILE * arq, int * linha, int * coluna);
void comando_if(Ttoken ** t,FILE * arq, int * linha, int * coluna);
void iteracao(Ttoken ** t,FILE * arq, int * linha, int * coluna);

void expr_relacional(Ttoken ** t, FILE * arq, int * linha, int * coluna);
int expr_arit (Ttoken ** t, FILE * arq, int * linha, int * coluna);
int expr_arit2 (Ttoken ** t, FILE * arq, int * linha, int * coluna);
int termo (Ttoken ** t, FILE * arq, int * linha, int * coluna);
int fator (Ttoken ** t, FILE * arq, int * linha, int * coluna);

int escopo=0, divisao=0;


int main( int argc, char* argv[] ){

	FILE * arquivo;
	Ttoken * t=NULL;
	int linha=1, coluna=-1;	
	
	/*
	if (argc != 2) {
		printf("ERRO NOS PARAMETROS!");
		return 0;
	}*/

	Abrir_Arquivo(/*argv[1]*/"arquivo.txt",&arquivo);
					
	parser(arquivo,&linha,&coluna);
	
	return 0;
}

void parser(FILE * arq, int * linha, int * coluna) {
	
	Ttoken * t;

	programa(arq,linha,coluna);

	t=scan(arq,linha,coluna);
	
		if( t->code == EOF )
			printf("COMPILADO DO SUCESSO\n");			
		else if(!feof(arq))
			printf("ERRO na linha %i, coluna %i, ultimo token lido %s:ERRO DE PROGRAMA, COMANDOS FORA DO BLOCO\n", (*linha), (*coluna), t->identificador);
}

void programa(FILE * arq, int * linha, int * coluna){
	
	Ttoken * t=NULL;
	t=scan(arq,linha,coluna);
	
		if( t->code == INT ){
			t=scan(arq,linha,coluna);
			if( t->code == MAIN ){
				t=scan(arq,linha,coluna);
				if( t->code == PARENTESES_ESQUERDO  ){
					t=scan(arq,linha,coluna);
					if( t->code == PARENTESES_DIREITO ){
						t=scan(arq,linha,coluna); 												
							 bloco(&t,arq,linha,coluna);					
					}else{
						printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ERRO NO PROGRAMA FALTA DO ')'\n", (*linha), (*coluna), t->identificador);
						return;						
					}
				}else{
					printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ERRO NO PROGRAMA FALTA DO '('\n", (*linha), (*coluna), t->identificador);
					return;					
				}
			}else{
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ERRO NO PROGRAMA FALTA DO 'main'\n", (*linha), (*coluna), t->identificador);
				return;
			}						
		}else{
			printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ERRO NO PROGRAMA FALTA DO 'int'\n", (*linha), (*coluna), t->identificador);
			return;	
		}	
}

void declara_variavel(Ttoken ** t, FILE * arq, int * linha, int * coluna){
		
	int valor = verificar_tipo((*t),linha,coluna);
	
	(*t)=scan(arq,linha,coluna); 
						
				if( (*t)->code == ID ){					

						if(consultar((*t)->identificador)){

							inserir((*t)->identificador,valor);

							(*t)=scan(arq,linha,coluna);


							if( (*t)->code == VIRGULA ){
								
								while( (*t)->code == VIRGULA ){

									(*t)=scan(arq,linha,coluna);

										if( (*t)->code == ID){

											if(consultar((*t)->identificador)){

												inserir((*t)->identificador,valor);

												(*t)=scan(arq,linha,coluna);											
											
											}else{
												printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ERRO DE DECLARACAO: IDENTIFICADOR EM USO\n", *linha, *coluna, (*t)->identificador);
												exit(0);
											}	
										}else{
											printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ERRO DE DECLARACAO: FALTA DO IDENTIFICADOR\n", (*linha), (*coluna), (*t)->identificador);
											exit(0);
										}
								}
									if( (*t)->code == PONTO_VIRGULA ){
									(*t)=scan(arq,linha,coluna);
									return;								
									}
									else{									
										printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ERRO DE DECLARACAO: FALTA DO ';'\n", (*linha), (*coluna), (*t)->identificador);
										exit(0);
									}			
							}else if( (*t)->code == PONTO_VIRGULA ){
								(*t)=scan(arq,linha,coluna);
								return;
							}else{	
								printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ERRO DE DECLARACAO: FALTA DO ';'\n", (*linha), (*coluna), (*t)->identificador);
								exit(0);							
							}
						}else{
							printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ERRO DE DECLARACAO: IDENTIFICADOR EM USO\n", *linha, *coluna, (*t)->identificador);
							exit(0);
						}
				}else{
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ERRO DE DECLARACAO: FALTA DO IDENTIFICADOR\n", (*linha), (*coluna), (*t)->identificador);
				exit(0);
				}
}

void bloco(Ttoken ** t,FILE * arq, int * linha, int * coluna){

	if( (*t)->code == CHAVE_ESQUERDA ){

		escopo++;

		(*t)=scan(arq,linha,coluna); 

		while( (*t)->code == INT || (*t)->code == FLOAT || (*t)->code == CHAR ){
			declara_variavel(t,arq,linha,coluna);		
		}

		while ( (*t)->code == ID || (*t)->code == CHAVE_ESQUERDA || (*t)->code == WHILE || (*t)->code == DO || (*t)->code == IF ) {					
			comando(t,arq,linha,coluna);
		}

		if( (*t)->code != CHAVE_DIREITA){				

			if( (*t)->code == INT || (*t)->code == FLOAT || (*t)->code == CHAR){
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ERRO NO BLOCO, DECLARACOES DEVEM SER AGRUPADAS NO INICIO DO BLOCO\n", (*linha), (*coluna), (*t)->identificador);
				exit(0);
			
			}
			printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ERRO NO BLOCO, ESPERAVA UMA FECHAMENTO\n", (*linha), (*coluna), (*t)->identificador);
			exit(0);		
		}

		remover(&tabela_simbolos);
		escopo--;

	}else{
		printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ERRO NO BLOCO, ESPERAVA UMA ABERTURA DE BLOCO\n", (*linha), (*coluna), (*t)->identificador);
		exit(0);
	}
}

void atribuicao(Ttoken ** t, FILE * arq, int * linha, int * coluna){

	int tipo_direito = buscar_variavel((*t)->identificador), tipo_esquerdo;

	if(tipo_direito){
	
		(*t)=scan(arq,linha,coluna); 

			if( (*t)->code == ATRIBUICAO ){

				(*t)=scan(arq,linha,coluna); 

					tipo_esquerdo = expr_arit(t,arq,linha,coluna);		
			
				if( (*t)->code == PONTO_VIRGULA ){	

					compatibilidade(&(*t),tipo_direito,tipo_esquerdo,linha,coluna);			

				if ( tipo_direito == V_FLOAT && tipo_esquerdo == tipo_esquerdo == V_INT) 
					tipo_esquerdo = V_FLOAT;
				
				
					(*t)=scan(arq,linha,coluna);
					//return;	
				}else{
					printf("ERRO na linha %i, coluna %i, ultimo token lido %s:ERRO NA ATRIBUICAO, FALTA DO ';'\n", (*linha), (*coluna), (*t)->identificador);
					exit(0);
				}
			}else{
					printf("ERRO na linha %i, coluna %i, ultimo token lido %s:ERRO NA ATRIBUICAO, FALTA DO '='\n", (*linha), (*coluna), (*t)->identificador);
					exit(0);
			 }	
	}else{
		printf("ERRO na linha %i, coluna %i, ultimo token lido %s:ERRO NA ATRIBUICAO, VARIAVEL NAO DECLARADA\n", *linha, *coluna, (*t)->identificador);
		exit(0);
	}	
}

int buscar_variavel(char idenficador[]) {
	
	Tlista *aux = tabela_simbolos;	

	if (aux == NULL) 
		return 0;	
	else {
		while (aux != NULL) {
			if (strcmp(idenficador, aux->identificador) == 0) {
				if ( aux->code == V_INT) 				
					return V_INT;				
				else if ( aux->code == V_FLOAT) 					
					return V_FLOAT;				
				else if ( aux->code == V_CHAR) 				
					return V_CHAR;				
			}
			else
				aux = aux->prox;
		}
		return 0;
	}
}

void comando (Ttoken ** t,FILE * arq, int * linha, int * coluna){			 
	
	if( (*t)->code == ID || (*t)->code == CHAVE_ESQUERDA){
		comando_basico(t,arq,linha,coluna);	
	}else if( (*t)->code == WHILE || (*t)->code == DO ){
		iteracao(t,arq,linha,coluna);	
	}else if( (*t)->code == IF){
		comando_if(t,arq,linha,coluna);		
	}
}

void comando_basico (Ttoken ** t,FILE * arq, int * linha, int * coluna){
	
	int escopo=0;

	if( (*t)->code == ID ){
		atribuicao(t,arq,linha,coluna);	
	}else if( (*t)->code == CHAVE_ESQUERDA){
		bloco(t,arq,linha,coluna);
		(*t)=scan(arq,linha,coluna); 
	}
}

void comando_if(Ttoken ** t, FILE * arq, int * linha, int * coluna){	
	
	(*t)=scan(arq,linha,coluna);

	if ( (*t)->code  !=PARENTESES_ESQUERDO ) {
		printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ERRO NO COMANDO 'IF', FALTA DE '('\n", (*linha), (*coluna), (*t)->identificador);
		exit(0);
	}
	(*t)=scan(arq,linha,coluna);
	expr_relacional(t,arq,linha,coluna);

	if ( (*t)->code  != PARENTESES_DIREITO) {
		printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ERRO NO COMANDO 'IF', FALTA DE ')'\n", (*linha), (*coluna), (*t)->identificador);
		exit(0);
	}

	(*t)=scan(arq,linha,coluna);
	comando(t,arq,linha,coluna);	

	if ( (*t)->code  == ELSE) {
		(*t)=scan(arq,linha,coluna);
		comando(t,arq,linha,coluna);
	}	
}

void iteracao(Ttoken ** t, FILE * arq, int * linha, int * coluna){	
		
		if( (*t)->code == DO ){

		(*t)=scan(arq,linha,coluna);	
			comando(t,arq,linha,coluna);

				if( (*t)->code == WHILE){

					(*t)=scan(arq,linha,coluna);

						if( (*t)->code == PARENTESES_ESQUERDO ){

							(*t)=scan(arq,linha,coluna);
								expr_relacional(t,arq,linha,coluna);
									(*t)=scan(arq,linha,coluna);
											
						}else{
							printf("ERRO na linha %i, coluna %i, ultimo token lido %s:ERRO NA ITERACAO, ESPERAVA '('\n", (*linha), (*coluna), (*t)->identificador);
							exit(0);
						}	

						if( (*t)->code == PONTO_VIRGULA){
							(*t)=scan(arq,linha,coluna);
							return;
						}else{
							printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ERRO NA ITERACAO FALTA DE UM ';'\n", (*linha), (*coluna), (*t)->identificador);
							exit(0);
						}			
				}else{
					printf("ERRO na linha %i, coluna %i, ultimo token lido %s:ERRO NA ITERACAO FALTA DO 'while'\n", (*linha), (*coluna), (*t)->identificador);
					exit(0);
				}
		}else if( (*t)->code == WHILE ){

			(*t)=scan(arq,linha,coluna);

			if( (*t)->code == PARENTESES_ESQUERDO ){

				(*t)=scan(arq,linha,coluna);
					expr_relacional(t,arq,linha,coluna);

						(*t)=scan(arq,linha,coluna);
							comando(t,arq,linha,coluna);			
			}else{
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s:ERRO NA ITERACAO, FALTA DE UM '('\n", (*linha), (*coluna), (*t)->identificador);
				exit(0);
			}		
		}
}

void expr_relacional(Ttoken ** t, FILE * arq, int * linha, int * coluna){	

	int tipo_direito, tipo_esquerdo;

		tipo_direito=expr_arit(t,arq,linha,coluna);

		if( (*t)->code == MAIOR || (*t)->code == MENOR || (*t)->code == MAIOR_IGUAL || (*t)->code == MENOR_IGUAL || (*t)->code == DIF || (*t)->code == IGUALDADE){
			(*t)=scan(arq,linha,coluna);
				tipo_esquerdo=expr_arit(t,arq,linha,coluna);			

		if ( tipo_direito == V_INT && tipo_esquerdo == V_FLOAT) 
			tipo_direito = V_FLOAT;
		else if ( tipo_direito == V_FLOAT && tipo_esquerdo == V_INT) 
			tipo_direito = V_FLOAT;		

		compatibilidade(&(*t),tipo_direito,tipo_esquerdo,linha,coluna);			

		}else{
			printf("ERRO na linha %i, coluna %i, ultimo token lido %s:ERRO EM EXPRESSAO RELACIONAL, FALTA DE UM OPERADOR RELACIONAL\n", (*linha), (*coluna), (*t)->identificador);
			exit(0);
		} 	
}

int expr_arit (Ttoken ** t,FILE * arq, int * linha, int * coluna){
	
	int tipo_direito, tipo_esquerdo;

	tipo_direito = termo(t,arq,linha,coluna);
	tipo_esquerdo = expr_arit2(t,arq,linha,coluna);

	compatibilidade(&(*t),tipo_direito,tipo_esquerdo,linha,coluna);
	
	if ( tipo_direito == V_INT && tipo_esquerdo == V_FLOAT) 
		tipo_direito = V_FLOAT;

	return tipo_direito;		
}

void compatibilidade(Ttoken ** t, int tipo_direito, int tipo_esquerdo, int * linha, int * coluna){

		if ( tipo_esquerdo == V_INT || tipo_esquerdo == V_FLOAT ||  tipo_esquerdo == V_CHAR ) {

		 if ( tipo_direito == V_CHAR && tipo_esquerdo != V_CHAR) {
			 printf("ERRO na linha %i, coluna %i, ultimo token lido |%s| CHAR OPERA APENAS COM CHAR \n",*linha, *coluna,(*t)->identificador);
			exit(0);			
		}else if ( tipo_direito == V_FLOAT && tipo_esquerdo == V_CHAR) {
			printf("ERRO na linha %i, coluna %i, ultimo token lido |%s| CHAR OPERANDO COM FLOAT \n",*linha, *coluna,(*t)->identificador);			
			exit(0);			
		}else if ( tipo_direito == V_INT && tipo_esquerdo != V_INT) {
			printf("ERRO na linha %i, coluna %i, ultimo token lido |%s| INT OPERA APENAS COM INT \n", *linha, *coluna,(*t)->identificador);			
			exit(0);					
		}
	}		
}

int expr_arit2 (Ttoken ** t, FILE * arq, int * linha, int * coluna){
	
		int tipo_direito=0, tipo_esquerdo = 0;

	if ((*t)->code == SOMA|| (*t)->code == SUB) {

		(*t)=scan(arq,linha,coluna);
		
		tipo_direito = termo(t,arq,linha,coluna);
		tipo_esquerdo = expr_arit2(t,arq,linha,coluna);

		}

	if ( tipo_direito == V_INT && tipo_esquerdo == V_FLOAT) 
		tipo_direito = V_FLOAT;

	compatibilidade(&(*t),tipo_direito,tipo_esquerdo,linha,coluna);
	
	
	return tipo_direito;


}

int termo (Ttoken ** t, FILE * arq, int * linha, int * coluna){
	
	int tipo_direito, tipo_esquerdo;


	tipo_direito = fator(t,arq,linha,coluna);

	while ((*t)->code == MULT || (*t)->code == DIV ) {

		if ((*t)->code == DIV) {
			divisao = 1;
		}
		else
			divisao = 0;

		(*t)=scan(arq,linha,coluna);
		tipo_esquerdo = fator(t,arq,linha,coluna);

		if ( tipo_esquerdo == V_CHAR || tipo_esquerdo == V_INT || tipo_esquerdo == V_FLOAT) {
			
			 compatibilidade(&(*t),tipo_direito,tipo_esquerdo,linha,coluna);
			 			

			if (( tipo_direito == V_INT && tipo_esquerdo == V_INT && divisao == 1) || ( tipo_direito == V_INT && tipo_esquerdo == V_FLOAT) ) 
				tipo_direito = V_FLOAT; 
		}	
	}
	return tipo_direito;
}

int fator(Ttoken ** t, FILE * arq, int * linha, int * coluna ){
	
	int tipo=0;

	if( (*t)->code == PARENTESES_ESQUERDO){
		
		(*t)=scan(arq,linha,coluna);

			tipo = expr_arit(t,arq,linha,coluna);

			if((*t)->code != PARENTESES_DIREITO  ){
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s:ERRO NO FATOR, FALTA DE UM ')'\n", (*linha), (*coluna), (*t)->identificador);
				exit(0);	
			}else {			
				(*t)=scan(arq,linha,coluna);
				return tipo;
			}
		
	
	}else if( (*t)->code == V_INT || (*t)->code == V_FLOAT || (*t)->code == V_CHAR ){
		tipo=(*t)->code;
		(*t)=scan(arq,linha,coluna);	
		return tipo;
	}else if( (*t)->code == ID  ){
		
		tipo = buscar_variavel((*t)->identificador);

			if (tipo) {
				(*t)=scan(arq,linha,coluna);
					return tipo;																					
			}else{
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: VARIAVEL NAO DECLARADA\n", (*linha), (*coluna), (*t)->identificador);				
				exit(0);																			
			}
	
	}else {
		printf("ERRO na linha %i, coluna %i, ultimo token lido %s:ERRO NO FATOR ESPERAVA UM ID OU VALOR\n", (*linha), (*coluna), (*t)->identificador);
		exit(0);	
	}	
}


void Abrir_Arquivo(char nome[] , FILE ** arq){	

	*arq=fopen(nome,"r");

	if(*arq!=NULL){
		
	}else{
		printf("Erro de Abertura do Arquivo!");
	}

	printf("\n\n");

}

Ttoken * scan(FILE * arq, int * linha, int * coluna){

	Ttoken * t=NULL;
	char buffer[tam]={'\0'};
	static char l_h = ' ';
	int pos_buffer=0;	

	t=(Ttoken*)malloc(sizeof(Ttoken));

	while ( !feof(arq) ){		

		//ID
		if( l_h == '_' || isalpha(l_h) ){
			
			while ( verificar_id (l_h,arq) )				
				leitura(arq,&l_h,linha,coluna,buffer,&pos_buffer);			
		
			strcpy(t->identificador,buffer);
			t->code=Palavra_Reversada(buffer,pos_buffer);	
			return t;		
		}
		//ID

		//NUMERO
		else if( isdigit(l_h) || l_h == '.' ){

			while ( verificar_numero(l_h,arq) )
				leitura(arq,&l_h,linha,coluna,buffer,&pos_buffer);
			

			if( l_h == '.' ){

				leitura(arq,&l_h,linha,coluna,buffer,&pos_buffer);			

				if( isdigit(l_h) ){

					while ( verificar_numero(l_h,arq) )
						leitura(arq,&l_h,linha,coluna,buffer,&pos_buffer);

							t->code=V_FLOAT;
							strcpy(t->identificador,buffer);	
							return t;

				}else{

					t->code=ERRO_FLOAT;
					strcpy(t->identificador,buffer);
					printf("ERRO na linha %i, coluna %i, ultimo token lido %s: MAL FORMACAO DE FLOAT\n", *linha, *coluna, t->identificador);
					exit(0);
				}
			}
			t->code=V_INT;
			strcpy(t->identificador,buffer);							
			return t;
		}
		//NUMERO

		//CHAR
		else if( l_h == 39 ){

			leitura(arq,&l_h,linha,coluna,buffer,&pos_buffer);
			
			if( isalnum(l_h) ){

				leitura(arq,&l_h,linha,coluna,buffer,&pos_buffer);

				if( l_h != 39 ){
					buffer[pos_buffer]=l_h;
					t->code=ERRO_CHAR;
					strcpy(t->identificador,buffer);
					printf("ERRO na linha %i, coluna %i, ultimo token lido %s: MAL FORMACAO DE CHAR\n", *linha, *coluna, t->identificador);
					exit(0);

				}else{
					buffer[pos_buffer]=l_h;
					pos_buffer++;

					strcpy(t->identificador,buffer);
					t->code=V_CHAR;	
					
					fread(&l_h,sizeof(l_h),1,arq); 
					(*coluna)++;					

					return t;
				}

			}else{
				t->code=ERRO_CHAR;
				strcpy(t->identificador,buffer);
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: MAL FORMACAO DE CHAR\n", *linha, *coluna, t->identificador);
				exit(0);				
			}
		} 
		//CHAR		

		//RELACIONAL
		else if( l_h == '>' || l_h == '<'){ 

			leitura(arq,&l_h,linha,coluna,buffer,&pos_buffer);

			if( buffer[0] == '>' ){

				t->code=MAIOR;
				t->identificador[0]=buffer[0];

			}else{

				t->code=MENOR;
				t->identificador[0]=buffer[0];

			}

			if( l_h != '='){

				t->identificador[1]='\0';
				return t;

			}else{
				if( t->code == MAIOR )
					t->code=MAIOR_IGUAL;
				else
					t->code=MENOR_IGUAL;

				t->identificador[1]=l_h;
				t->identificador[2]='\0';

				fread(&l_h,sizeof(l_h),1,arq);
				(*coluna)++;
									
				return t;
			}

		}else if( l_h == '='){

			leitura(arq,&l_h,linha,coluna,buffer,&pos_buffer);
			
			if( feof(arq) ){
				
				t->code=ATRIBUICAO;
				t->identificador[0]=buffer[0];
				t->identificador[1]='\0';

				return t;			
			}

			if( l_h != '='){
				t->code=ATRIBUICAO;
				t->identificador[0]=buffer[0];
				t->identificador[1]='\0';		

				return t;

			}else{				
				t->code=IGUALDADE;
				t->identificador[1]=l_h;
				t->identificador[2]='\0';

				fread(&l_h,sizeof(l_h),1,arq);
				(*coluna)++;

				return t;
			}
		}else if( l_h == '!'){

			leitura(arq,&l_h,linha,coluna,buffer,&pos_buffer);

			t->code=DIF;

			if( l_h != '=' ){

				if(!feof(arq))
				buffer[pos_buffer]=l_h;
				
				t->code=ERRO_EXCLAMACAO;

				strcpy(t->identificador,buffer);
				
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: EXCLAMACAO NAO FOI PRECEDIDA DE IGUAL\n", *linha, *coluna, t->identificador);
				exit(0);
			}else{	

				t->identificador[0]=buffer[0];
				t->identificador[1]=l_h;
				t->identificador[2]='\0';

				fread(&l_h,sizeof(l_h),1,arq);
				(*coluna)++;
				return t;
			}
		}
		//RELACIONAL

		//COMENTARIO
		else if( l_h == '/' ){

			leitura(arq,&l_h,linha,coluna,buffer,&pos_buffer);
						
			 if( l_h == '/' && !feof(arq)){

				while ( l_h !='\n' && !feof(arq) ){

					fread(&l_h,sizeof(l_h),1,arq);
					(*coluna)++;					
									
				}
						
			}else if( l_h == '*' ){	

					while (!feof(arq)){						

						fread(&(l_h),sizeof(char),1,arq);
						(*coluna)++;

						if( '\n' == l_h ){
							(*linha)++;
							(*coluna)=0;
						}

						if( l_h == '*' ){

							fread(&(l_h),sizeof(char),1,arq);
							(*coluna)++;
															
							if( l_h == '/' ){

								fread(&l_h,sizeof(l_h),1,arq);
								(*coluna)++;		
								
								return scan(arq,linha,coluna);	

							}else if( l_h == '*' && !feof(arq)){
								fseek(arq,-sizeof(char),1);	
								pos_buffer--;
								(*coluna)--;
							}
							
							if( l_h == '\n'){
								
							(*linha)++;
							(*coluna)=0;
								
							}						
						}
					}
				strcpy(t->identificador,"FIM DE ARQUIVO\0");
				t->code=ERRO_COMENTARIO;
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: FINAL DE ARQUIVO DENTRO DO COMENTARIO MULTILINHA\n", *linha, *coluna, t->identificador);
				exit(0);
			 }else{ 
				t->code=DIV;
				strcpy(t->identificador,buffer);				
				return t;
			 }
			pos_buffer=0;
		}
		//COMENTARIO	

		//ESPECIAL 				
		else if(')' == l_h ||'(' == l_h ||'{' == l_h ||'}' == l_h ||',' == l_h ||';' == l_h ||'+' == l_h ||'-' == l_h || '*' == l_h){	
			
			t=simbolos(t,l_h);
			
			fread(&l_h,sizeof(l_h),1,arq);
			(*coluna)++;			
			return t;

		}else if( l_h == ' ' ||  l_h == '\t' ||  l_h == '\n' ){

			if( l_h == ' ')		
				(*coluna)++;		
			else if( l_h == '\t')		
				(*coluna)+=4;		
			else if( l_h == '\n'){			
				(*linha)++;		
				(*coluna)=0;
			}
			fread(&l_h,sizeof(l_h),1,arq);
				
		}		
		else{
			t->code=CARACTER_INVALIDO;
			t->identificador[0]=l_h;
			printf("ERRO na linha %i, coluna %i, ultimo token lido %c: CARACTER INVALIDO \n", *linha, *coluna, t->identificador[0]);	
			exit(0);
		}
		//ESPECIAL			
	}
	t->code=EOF;
	strcpy(t->identificador,buffer);
	return t;
}

int verificar_id( char l_h , FILE * arq ){
	
	if( feof(arq) )
		return 0;
	else if( l_h == '_' )
		return 1;
	else if( isalnum(l_h) )
		return 1;

	return 0;
}

int verificar_numero( char l_h , FILE * arq ){

	if( feof(arq) )
		return 0;
	else if( isdigit(l_h) )
		return 1;
	
	return 0;
}

void leitura(FILE * arq,char * l_h, int * linha, int * coluna, char buffer[], int * pos_buffer){
	
	buffer[*pos_buffer]=(*l_h);
	(*pos_buffer)++;

	fread(&(*l_h),sizeof(char),1,arq);
	(*coluna)++;
}

Ttoken * simbolos(Ttoken * t,char l_h){

	int i;
	char simbolos[10]={'(',')','{','}',',',';','+','-','*'};


	for (i = 0; i < 10; i++){
		if(simbolos[i] == l_h){
			t->code=i+13;
			t->identificador[0]=simbolos[i];
			t->identificador[1]='\0';	
			return t;
		}
	}

	return NULL;
}

int Palavra_Reversada( char buffer[], int pos_buffer){

	int i;	
	char palavra_reservada[10][10] = {"main", "if", "else", "while", "do", "for", "int", "float", "char"};	

	for (i = 0; i < 10; i++){		
		if( strcmp(buffer,palavra_reservada[i]) == 0 )
			return i;
	}
	return 9;
}

void remover(Tlista ** tabela){

	Tlista *aux;

	if (*tabela == NULL)
		return;
	else {
		aux = *tabela;
		while (aux->escopo == escopo) {
			*tabela = aux->prox;
			free(aux);
			if (tabela == NULL)
				return;
		}
	}
}

void inserir(char identicidador[], int tipo){
    
	
	Tlista * aux;
	
	criar_no_tabela(&aux,identicidador,tipo);	

	if (tabela_simbolos == NULL) {
		tabela_simbolos = aux;
		aux->prox = NULL;
	}else {
		aux->prox = tabela_simbolos;
		tabela_simbolos = aux;
	}
}

Tlista * criar_no_tabela( Tlista ** novo, char identicidador[], int tipo ){

	(*novo) = (Tlista*) malloc( sizeof (Tlista) );
	strcpy( (*novo)->identificador , identicidador );
	(*novo)->escopo = escopo;
	(*novo)->code = tipo;

	return (*novo);
}

int verificar_tipo(Ttoken * t, int *linha, int *coluna) {

	if ( t->code == INT )
		return V_INT;	
	else if ( t->code == FLOAT) 		
		return V_FLOAT;	
	else if ( t->code == CHAR) 		
		return V_CHAR;	
	else {
		printf("ERRO na linha %i, coluna %i, ultimo token lido %c: TIPO INVALIDO \n", *linha, *coluna, t->identificador);		
		exit(0);
	}
}

int consultar(char identificador[]){

	Tlista * aux = tabela_simbolos;

	if (aux == NULL)
		return 1;	
	else{
			while (aux != NULL) {
				if (aux->escopo < escopo)
					return 1; 
				if (strcmp(aux->identificador, identificador) == 0)
					return 0; //achou
				aux = aux->prox;
			}
	    }
	return 1;
}