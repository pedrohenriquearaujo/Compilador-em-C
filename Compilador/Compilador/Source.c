#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#define tam 256
#define true 1
//DIGITO
#define INT 10
#define FLOAT 11
#define CHAR 12
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
#define ERRO_CHAR_2 34
#define ERRO_EXCLAMACAO 32
#define ERRO_COMENTARIO 33
//COMENTARIO
#define COMENTARIO_LINHA 35
#define COMENTARIO_MULT 36
//CARACTER
#define CARACTER_INVALIDO 37

typedef struct token{

	int code;
	char identificador[tam];

}Ttoken;

void Abrir_Arquivo(char nome[] , FILE ** arq);
Ttoken * scan(FILE * arq, int * linha, int * coluna);
int Palavra_Reversada( char buffer[], int pos_buffer);
int verificar_caracter(char l_h, int * linha, int * coluna, FILE * arq);
Ttoken * simbolos(Ttoken * t,char l_h);
int alfabeto(char l_h);
void msg(int code,Ttoken * t,int linha, int coluna);

int main(){

	FILE * arquivo;
	Ttoken * t=NULL, * ultimo=NULL;
	int linha=0, coluna=0;	
	ultimo=(Ttoken*)malloc(sizeof(Ttoken));
	strcpy(ultimo->identificador,"SEM ANTERIOR");

	Abrir_Arquivo("arquivo.txt",&arquivo);

	while (true){

		t=scan(arquivo,&linha,&coluna);	

		if(t==NULL)		
			break;
		
		

		if( t->identificador[0] != '\0'){
			ultimo=t;	
			printf("Code: %i  ", t->code);		
			printf("Lex: {%s}	", t->identificador);		
			printf("Linha: %i	", linha);
			printf("Coluna: %i	", coluna);
			printf("\n");
			
		}else{
			msg(t->code,ultimo,linha,coluna);
		}
	}
	return 0;
}

void msg (int code,Ttoken * t, int linha, int coluna){

	
	if( code == 30 )
		printf("ERRO na linha %i, coluna %i, ultimo token lido {%s}: ERRO NA FORMACAO DO FLOAT\n", linha, coluna, t->identificador);
	if( code == 31)
		printf("ERRO na linha %i, coluna %i, ultimo token lido {%s}: ERRO NA FORMACAO DO CHAR\n", linha, coluna, t->identificador);
	if( code == 32)
		printf("ERRO na linha %i, coluna %i, ultimo token lido {%s}: ERRO NA FORMACAO DO '!='\n", linha, coluna, t->identificador);
	if( code == 33)
		printf("ERRO na linha %i, coluna %i, ultimo token lido {%s}: ERRO DE COMENTARIO\n", linha, coluna, t->identificador);
	if( code == 34)
		printf("ERRO na linha %i, coluna %i, ultimo token lido {%s}: ERRO NA FORMACAO DO CHAR_2\n", linha, coluna, t->identificador);
	if( code == 35)
		printf("COMETARIO LINHA");
	if( code == 36)
		printf("COMETARIO MULT");
	if( code == 37)
		printf("CARACTER INVALIDO\n");
	printf("\n");
}

void Abrir_Arquivo(char nome[] , FILE ** arq){	

	*arq=fopen(nome,"r");

	if(*arq!=NULL){
		printf("Abertura do Arquivo!\n");	
	}else{
		printf("Erro de Abertura do Arquivo!\n");
	}
}
Ttoken * scan(FILE * arq, int * linha, int * coluna){

	Ttoken * t=NULL, *j=NULL;
	char buffer[tam]={'\0'};
	static char l_h = ' ';
	int pos_buffer=0;	

	t=(Ttoken*)malloc(sizeof(Ttoken));

	while (!feof(arq)){		

		//ID
		if(l_h == '_' || isalpha(l_h) ){

			buffer[pos_buffer]=l_h;
			pos_buffer++;

			fread(&l_h,sizeof(l_h),1,arq);
			(*coluna)++;




			if( verificar_caracter(l_h,linha,coluna,arq) == 1){			

				t->code=Palavra_Reversada(buffer,pos_buffer);
				strcpy(t->identificador,buffer);
				return t;
			}
			while ( l_h == '_' || isalnum(l_h) ){

				buffer[pos_buffer]=l_h;
				pos_buffer++;

				fread(&l_h,sizeof(l_h),1,arq);
				(*coluna)++;

				if( verificar_caracter(l_h,linha,coluna,arq) == 1)
					break;				
			}	
			strcpy(t->identificador,buffer);
			t->code=Palavra_Reversada(buffer,pos_buffer);	
			return t;		
		}
		//ID

		//NUMERO
		if( isdigit(l_h) || l_h == '.' ){

			while ( isdigit(l_h)){

				buffer[pos_buffer]=l_h;
				pos_buffer++;

				fread(&l_h,sizeof(l_h),1,arq);
				(*coluna)++;

				if(feof(arq)){
					t->code=INT;
					strcpy(t->identificador,buffer);	
					return t;
				}
			}
			
			if(l_h == '.'){
				buffer[pos_buffer]=l_h;
				pos_buffer++;

				fread(&l_h,sizeof(l_h),1,arq);
				(*coluna)++;

				if(feof(arq)){
					t->code=ERRO_FLOAT;
					t->identificador[0]='\0';	
					return t;
				}
				if(isdigit(l_h)){
					while ( isdigit(l_h) ){

					buffer[pos_buffer]=l_h;
					pos_buffer++;

					fread(&l_h,sizeof(l_h),1,arq);
					(*coluna)++;

						if(feof(arq)){
							t->code=FLOAT;
							strcpy(t->identificador,buffer);	
							return t;
						}
					}
				}else{
					t->code=ERRO_FLOAT;
					t->identificador[0]='\0';	
					return t;
				}			
			}
							t->code=INT;
							strcpy(t->identificador,buffer);							
							return t;
		}//NUMERO
		
		if( l_h == 39){//CHAR
			buffer[pos_buffer]=l_h;
			pos_buffer++;

			fread(&l_h,sizeof(l_h),1,arq);
			(*coluna)++;




			if(isalnum(l_h)){
				buffer[pos_buffer]=l_h;
				pos_buffer++;

				fread(&l_h,sizeof(l_h),1,arq);
				(*coluna)++;

				if( l_h!=39 ){
							t->code=ERRO_CHAR;
							t->identificador[0]='\0';
							return t;
				}else{
					buffer[pos_buffer]=l_h;
					pos_buffer++;

					strcpy(t->identificador,buffer);
					t->code=CHAR;	

					fread(&l_h,sizeof(l_h),1,arq);
					(*coluna)++;

					return t;
				}
			
			}else{
					t->code=ERRO_CHAR_2;
					t->identificador[0]='\0';
					return t;
			}
		}//CHAR		
		
		if( l_h == '>' || l_h == '<'){ //RELACIONAL
			
			buffer[pos_buffer]=l_h;
			pos_buffer++;
			
			if( l_h == '>' ){
				t->code=MAIOR;
				t->identificador[0]=l_h;
			}else{
				t->code=MENOR;
				t->identificador[0]=l_h;
			}

			fread(&l_h,sizeof(l_h),1,arq);
			(*coluna)++;

			if( l_h != '='){

			t->identificador[1]='\0';
			return t;

			}else{
				if(t->code==MAIOR)
					t->code=MAIOR_IGUAL;
				else
					t->code=MENOR_IGUAL;
							
				t->identificador[1]=l_h;
				t->identificador[2]='\0';
				
				fread(&l_h,sizeof(l_h),1,arq);
				(*coluna)++;

			if(feof(arq))
				return t;	
				return t;
			}
		
		}else if( l_h == '='){
			
			buffer[pos_buffer]=l_h;
			pos_buffer++;			
		

			fread(&l_h,sizeof(l_h),1,arq);
			(*coluna)++;

			if(feof(arq)){
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
		
			buffer[pos_buffer]=l_h;

			t->code=DIF;		

			fread(&l_h,sizeof(l_h),1,arq);
			(*coluna)++;

			if(l_h != '='){
				t->code=ERRO_EXCLAMACAO;
				t->identificador[0]='\0';
				return t;
			}else{				
			t->identificador[0]=buffer[0];
			t->identificador[1]=l_h;
			t->identificador[2]='\0';

			fread(&l_h,sizeof(l_h),1,arq);
			(*coluna)++;

			return t;
			}
		}//RELACIONAL
					
		if( l_h == '/'){//COMENTARIO

			buffer[pos_buffer]=l_h;
			pos_buffer++;

			fread(&l_h,sizeof(l_h),1,arq);
			(*coluna)++;

			if(feof(arq) || isalnum(l_h) || isspace(l_h)){
				t->code=DIV;
				t->identificador[0]=buffer[0];
				t->identificador[1]='\0';
				return t;			
			}else if(l_h == '/'){
				
				while (l_h !='\n'){

					fread(&l_h,sizeof(l_h),1,arq);
					(*coluna)++;

					if(feof(arq)){					
						t->code=COMENTARIO_LINHA;
						t->identificador[0]='\0';
						return t; 		
					}}
				(*linha)++;
				t->code=COMENTARIO_LINHA;
				t->identificador[0]='\0';
				return t;
				
			}else if( l_h == '*'){
				
				fread(&l_h,sizeof(l_h),1,arq);
				(*coluna)++;
				
				if(feof(arq)){
					t->code=ERRO_COMENTARIO;
					t->identificador[0]='\0';
					return t;
				}
				while (!feof(arq)){

					if( l_h == '*'){
					fread(&l_h,sizeof(l_h),1,arq);
					(*coluna)++;

						if(feof(arq)){
						t->code=ERRO_COMENTARIO;
						t->identificador[0]='\0';
						return t;
						}
							if(l_h == '/'){
							t->code=COMENTARIO_MULT;
							t->identificador[0]='\0';

							fread(&l_h,sizeof(l_h),1,arq);
							(*coluna)++;

							return t;//COMENTARIO DE UMA LINHA
							}				
					}
					fread(&l_h,sizeof(l_h),1,arq);
					(*coluna)++;
				}	
						t->code=ERRO_COMENTARIO;
						t->identificador[0]='\0';
						return t;
			}
		}//COMENTARIO			
		//ESPECIAL 		
			j=simbolos(t,l_h);
		if(NULL!= simbolos(t,l_h)){	
			fread(&l_h,sizeof(l_h),1,arq);
			(*coluna)++;
			return j;
		}
		fread(&l_h,sizeof(l_h),1,arq);
		(*coluna)++;		
		if(!alfabeto(l_h) && !feof(arq) ){
				t->code=CARACTER_INVALIDO;
				t->identificador[0]='\0';
				return t;
			}
		//ESPECIAL			
}
	return NULL;
}

void leitura(FILE * arq, char * l_h, int * linha, int * coluna){
		
	fread(&l_h,sizeof(l_h),1,arq);
	(*coluna)++;


}


int alfabeto(char l_h){

	char simbolos[15]={')','(','{','}',',',';','+','-','*','/','=','>','<','!','_'};
	int i;

	if(isalnum(l_h))
		return 1;

	for (i = 0; i < 15; i++){
		if(l_h == simbolos[i])
			return 1;
	}
	return 0;
}


Ttoken * simbolos(Ttoken * t,char l_h){
	
	int i;
	char simbolos[10]={')','(','{','}',',',';','+','-','*'};


	for (i = 0; i < 10; i++){
		if(simbolos[i] == l_h){
			t->code=i+12;
			t->identificador[0]=simbolos[i];
			t->identificador[1]='\0';	
			return t;
		}
	}

	return NULL;
}



int verificar_caracter(char l_h, int * linha, int * coluna, FILE * arq){

	if( l_h == ' '){		
		(*coluna)++;		
		return 1;
	}else if( l_h == '\t'){		
		(*coluna)+=4;
		return 1;
	}else if( l_h == '\n'){			
		(*coluna)++;
		return 1;
	}else if(feof(arq))
		return 1;

	return 0;
}

int Palavra_Reversada( char buffer[], int pos_buffer){

	int i;	
	char palavra_reservada[10][10] = {"main", "if", "else", "while", "do", "for", "int", "float", "char"};	

	for (i = 0; i < 10; i++){		
		if(strcmp(buffer,palavra_reservada[i])==0)
			return i;
	}
	return 9;
}
