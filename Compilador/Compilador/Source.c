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

void bloco(FILE * arq, int * linha, int * coluna);
void programa(FILE * arq, int * linha, int * coluna);
void parser(FILE * arq, int * linha, int * coluna);


void declara_variavel(FILE * arq, int * linha, int * coluna);
void atribuicao(FILE * arq, int * linha, int * coluna);
void comando (Ttoken * t,FILE * arq, int * linha, int * coluna);
void comando_basico (Ttoken * t,FILE * arq, int * linha, int * coluna);
void comando_if(FILE * arq, int * linha, int * coluna);
void iteracao(FILE * arq, int * linha, int * coluna);

void expr_relacional(Ttoken ** t, FILE * arq, int * linha, int * coluna);
void expr_arit (Ttoken ** t, FILE * arq, int * linha, int * coluna);
void expr_arit2 (Ttoken ** t, FILE * arq, int * linha, int * coluna);
void termo (Ttoken ** t, FILE * arq, int * linha, int * coluna);
void termo2 (Ttoken ** t, FILE * arq, int * linha, int * coluna);
void fator (Ttoken ** t, FILE * arq, int * linha, int * coluna);
int verificar_operador(int code);


int main( int argc, char* argv[] ){

	FILE * arquivo;
	Ttoken * t=NULL;
	int linha=1, coluna=-1;	
	
	/*
	if (argc < 2 || argc > 2) {
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
			printf("PROCESSO DE COMPILACAO TERMINADO\n");			
		else if(feof(arq))
			printf("ERRO na linha %i, coluna %i, ultimo token lido %s: COMANDOS FORA DO BLOCO\n", (*linha), (*coluna), t->identificador);
}


void programa(FILE * arq, int * linha, int * coluna){
	
	Ttoken * t;
	t=scan(arq,linha,coluna);
	
		if( t->code == INT ){
			t=scan(arq,linha,coluna);
			if( t->code == MAIN ){
				t=scan(arq,linha,coluna);
				if( t->code == PARENTESES_ESQUERDO  ){
					t=scan(arq,linha,coluna);
					if( t->code == PARENTESES_DIREITO ){
						t=scan(arq,linha,coluna); 
						if( t->code == CHAVE_ESQUERDA ){
							bloco(arq,linha,coluna);
						}else{
						printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Falta Do '{'\n", (*linha), (*coluna), t->identificador);
						return;
						}
					}else{
						printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Falta Do ')'\n", (*linha), (*coluna), t->identificador);
						return;						
					}
				}else{
					printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Falta Do '('\n", (*linha), (*coluna), t->identificador);
					return;					
				}
			}else{
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Falta Do 'main'\n", (*linha), (*coluna), t->identificador);
				return;
			}						
		}else{
			printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Falta Do 'int'\n", (*linha), (*coluna), t->identificador);
			return;	
		}	
}

void declara_variavel(FILE * arq, int * linha, int * coluna){
	
	Ttoken * t=NULL;	

	t=scan(arq,linha,coluna); 
						
				if( t->code == ID ){
						t=scan(arq,linha,coluna);

						if( t->code == VIRGULA ){
								
							while( t->code == VIRGULA ){
								t=scan(arq,linha,coluna);
									if( t->code == ID){
										t=scan(arq,linha,coluna);
									}else{
										printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Ausencia do IDENTIFICADOR\n", (*linha), (*coluna), t->identificador);
										exit(0);
									}
							}
								if( t->code == PONTO_VIRGULA )
									return;
								else{									
									printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Ausencia do PONTO E VIRGULA\n", (*linha), (*coluna), t->identificador);
									exit(0);
								}			
						}else if( t->code == PONTO_VIRGULA ){
							return;
						}else{						
							printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Ausencia da VIRGULA OU PONTO E VIRGULA\n", (*linha), (*coluna), t->identificador);
							exit(0);
						}
					}else{
					printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Ausencia do IDENTIFICADOR\n", (*linha), (*coluna), t->identificador);
					exit(0);
					}
}

void tipo (FILE * arq, int * linha, int * coluna){
	
	Ttoken * t=NULL;
	
	if( t->code == CHAR){
		
		t=scan(arq,linha,coluna);
			if( t->code != V_CHAR){
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: VARIAVEL DECLARADA SEM ASPAS SIMPLES\n", (*linha), (*coluna), t->identificador);
				exit(0);
			}
	
	}else if( t->code == INT ){
		
		t=scan(arq,linha,coluna); 

			if( t->code == V_CHAR){
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: VARIAVEL DECLARADA SEM ASPAS SIMPLES\n", (*linha), (*coluna), t->identificador);
				exit(0);
			}
	
	}else if( t->code == FLOAT){
		
		t=scan(arq,linha,coluna);

		if( t->code == V_CHAR){
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: VARIAVEL DECLARADA SEM ASPAS SIMPLES\n", (*linha), (*coluna), t->identificador);
				exit(0);
			}	
	}
}


void bloco(FILE * arq, int * linha, int * coluna){
	
	Ttoken * t=NULL;	

	t=scan(arq,linha,coluna);
	
	while ( t->code != EOF){

		if( t->code == INT || t->code == FLOAT || t->code == CHAR )
			declara_variavel(arq,linha,coluna);
		else if( t->code == ID || t->code == IF || t->code == ELSE || t->code == WHILE || t->code == DO || t->code == FOR)
			comando(t,arq,linha,coluna);		
		else if( t->code == CHAVE_DIREITA )
			return ;

		t=scan(arq,linha,coluna);
	}		
	printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Ausencia '}'\n", (*linha), (*coluna), t->identificador);
	exit(0);

}

void atribuicao(FILE * arq, int * linha, int * coluna){

	Ttoken * t=NULL;	

	t=scan(arq,linha,coluna); 

	if( t->code == ATRIBUICAO ){

		t=scan(arq,linha,coluna); 
			expr_arit(&t,arq,linha,coluna);		
			
		if( t->code == PONTO_VIRGULA )
				return;	
		else{
			printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ESTRUTURA DE EXPRESSAO INCORRETA\n", (*linha), (*coluna), t->identificador);
			exit(0);
		}
	} else{
			printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Ausencia Do Igual\n", (*linha), (*coluna), t->identificador);
			exit(0);
	}
}


void comando (Ttoken * t,FILE * arq, int * linha, int * coluna){	
	
		 
	
	if( t->code == ID || t->code == CHAVE_ESQUERDA){
		comando_basico(t,arq,linha,coluna);	
	}else if( t->code == WHILE || t->code == DO ){
		iteracao(arq,linha,coluna);	
	}else if( t->code == IF){
		comando_if(arq,linha,coluna);	
	}
}

void comando_basico (Ttoken * t,FILE * arq, int * linha, int * coluna){
	
	if( t->code == ID ){
		atribuicao(arq,linha,coluna);	
	}else if( t->code == CHAVE_ESQUERDA){
		bloco(arq,linha,coluna);	
	}
}

void comando_if(FILE * arq, int * linha, int * coluna){

		Ttoken * t=NULL;
	
			t=scan(arq,linha,coluna);

			if( t->code == PARENTESES_ESQUERDO  ){
				t=scan(arq,linha,coluna);
					expr_relacional(&t,arq,linha,coluna);
					
					if( t->code == PARENTESES_DIREITO){
						t=scan(arq,linha,coluna);
							comando(t,arq,linha,coluna);								
								if( t->code == ELSE){
									t=scan(arq,linha,coluna);
										comando(t,arq,linha,coluna);
								}else{
									return;
								}								
					}else{						
							printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ESPERAVA UM FECHA PARENTESE\n", (*linha), (*coluna), t->identificador);
							exit(0);
					}
			}else{
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ESPERAVA UM ABRE PARENTESE\n", (*linha), (*coluna), t->identificador);
				exit(0);
			}		
}

void iteracao(FILE * arq, int * linha, int * coluna){

	Ttoken * t=NULL;
		
		if( t->code == DO ){
		t=scan(arq,linha,coluna);		
			comando(t,arq,linha,coluna);
			if( t->code == WHILE){
				t=scan(arq,linha,coluna);	
				expr_relacional(&t,arq,linha,coluna);
				if( t->code == PONTO_VIRGULA){					
					return;
				}else{
					printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ESPERAVA UM PONTO E VIRGULA\n", (*linha), (*coluna), t->identificador);
					exit(0);
				}			
			}else{
					printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ESPERAVA UM WHILE\n", (*linha), (*coluna), t->identificador);
					exit(0);
				}
		}else if( t->code == WHILE ){
			t=scan(arq,linha,coluna);		
				expr_relacional(&t,arq,linha,coluna);
					t=scan(arq,linha,coluna);
						comando(t,arq,linha,coluna);		
		}else{
			printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ESPERAVA COMANDO\n", (*linha), (*coluna), t->identificador);
					exit(0);
		}

}

void expr_relacional(Ttoken ** t, FILE * arq, int * linha, int * coluna){
	
	if( (*t)->code == ID){
		expr_arit(t,arq,linha,coluna);		
		if( (*t)->code == MAIOR || (*t)->code == MENOR || (*t)->code == MAIOR_IGUAL || (*t)->code == MENOR_IGUAL || (*t)->code == DIF || (*t)->code == IGUALDADE){
			(*t)=scan(arq,linha,coluna);
				expr_arit(t,arq,linha,coluna);		
		}else{
			printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ESPERAVA UM OPERADOR RELACIONAL\n", (*linha), (*coluna), (*t)->identificador);
			exit(0);
		} 
	}else{
		printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ESPERAVA UM IDENTIFICADOR\n", (*linha), (*coluna), (*t)->identificador);
		exit(0);
	}
}
//aqui
void expr_arit (Ttoken ** t,FILE * arq, int * linha, int * coluna){
	
	termo(t,arq,linha,coluna);
	expr_arit2(t,arq,linha,coluna);
		
}

void expr_arit2 (Ttoken ** t, FILE * arq, int * linha, int * coluna){
	
		if( (*t)->code == SOMA){

			(*t)=scan(arq,linha,coluna);
		
			expr_arit(t,arq,linha,coluna);
		
		}else if( (*t)->code == SUB){

			(*t)=scan(arq,linha,coluna);

			expr_arit(t,arq,linha,coluna);	
		}	
}

void termo (Ttoken ** t, FILE * arq, int * linha, int * coluna){
		
	fator(t,arq,linha,coluna);
	termo2(t,arq,linha,coluna);
}

void termo2 (Ttoken ** t, FILE * arq, int * linha, int * coluna){
	
		if((*t)->code == MULT){

		(*t)=scan(arq,linha,coluna);	

			expr_arit(t,arq,linha,coluna);
		
		return;	

		}else if( (*t)->code == DIV ){
	
		(*t)=scan(arq,linha,coluna);

			expr_arit(t,arq,linha,coluna);
		
		return;
		}
}


void fator(Ttoken ** t, FILE * arq, int * linha, int * coluna ){

	if( (*t)->code ==PARENTESES_ESQUERDO ){	
		(*t)=scan(arq,linha,coluna);
		if( !(verificar_operador((*t)->code)) ){
			expr_arit(t,arq,linha,coluna);				
				if( (*t)->code != PARENTESES_DIREITO ){
					printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ESPERAVA DE PARENTESE\n", (*linha), (*coluna), (*t)->identificador);
					exit(0);				
				}else{
					(*t)=scan(arq,linha,coluna);
				}
		}else{
			printf("ERRO na linha %i, coluna %i, ultimo token lido %s: ISOLAMENTO DE OPERADOR COM PARENTESE\n", (*linha), (*coluna), (*t)->identificador);
			exit(0);

		}
	}else if( (*t)->code == ID || (*t)->code == V_INT || (*t)->code == V_FLOAT || (*t)->code == V_CHAR){
		(*t)=scan(arq,linha,coluna);		
	}
}


int verificar_operador(int code){

	if( (code >= 18 && code <= 22) || code == 14)
		return 1;
	return 0;
}

//aqui
void Abrir_Arquivo(char nome[] , FILE ** arq){	

	*arq=fopen(nome,"r");

	if(*arq!=NULL){
		printf("Arquivo Aberto Com Sucesso!");
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
				t->identificador[1]=buffer[0];
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