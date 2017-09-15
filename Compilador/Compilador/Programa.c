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
void msg (Ttoken * t, int linha, int coluna, int * erro);
void leitura(FILE * arq,char * l_h, int * linha, int * coluna, char buffer[], int * pos_buffer);
int verificar_id( char l_h , FILE * arq);
int verificar_numero( char l_h , FILE * arq );

int main( int argc, char* argv[] ){

	FILE * arquivo;
	Ttoken * t=NULL;
	int linha=1, coluna=-1, erro=0;	
	
	
	if (argc < 2 || argc > 2) {
		printf("ERRO NOS PARAMETROS!");
		return 0;
	}

	Abrir_Arquivo(argv[1],&arquivo);

	while (!feof(arquivo) && erro!=1){

		t=scan(arquivo,&linha,&coluna);	

		msg(t,linha,coluna,&erro);

	}
	return 0;
}

void msg (Ttoken * t, int linha, int coluna, int * erro){
	
		*erro=1;

		if( t != NULL ){

			if( t->code == 30 )
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: MAL FORMACAO DE FLOAT\n", linha, coluna, t->identificador);				
			else if( t->code == 31)
					printf("ERRO na linha %i, coluna %i, ultimo token lido %s: MAL FORMACAO DE CHAR\n", linha, coluna, t->identificador);					
			else if( t->code == 32)
					printf("ERRO na linha %i, coluna %i, ultimo token lido %s: EXCLAMACAO NAO FOI PRECEDIDA DE IGUAL\n", linha, coluna, t->identificador);					
			else if( t->code == 33)
					printf("ERRO na linha %i, coluna %i, ultimo token lido %s: FINAL DE ARQUIVO DENTRO DO COMENTARIO MULTILINHA\n", linha, coluna, t->identificador);
			else if( t->code == 34)
					printf("ERRO na linha %i, coluna %i, ultimo token lido %c: CARACTER INVALIDO \n", linha, coluna, t->identificador[0]);		
			else
			    		*erro=0;
		}
}

void Abrir_Arquivo(char nome[] , FILE ** arq){	

	*arq=fopen(nome,"r");

	if(*arq==NULL)
		printf("Erro de Abertura do Arquivo!");	
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

				if( feof(arq) ){

					t->code=ERRO_FLOAT;
					strcpy(t->identificador,buffer);	
					return t;

				}else if( isdigit(l_h) ){

					while ( verificar_numero(l_h,arq) )
						leitura(arq,&l_h,linha,coluna,buffer,&pos_buffer);

							t->code=FLOAT;
							strcpy(t->identificador,buffer);	
							return t;

				}else{

					t->code=ERRO_FLOAT;
					strcpy(t->identificador,buffer);					
					return t;
				}
			}
			t->code=INT;
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
					if( !feof(arq) )
					buffer[pos_buffer]=l_h;

					t->code=ERRO_CHAR;
					strcpy(t->identificador,buffer);
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
				t->code=ERRO_CHAR;
				strcpy(t->identificador,buffer);
				return t;
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
				
				return t;
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
				return t;
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
			return t;
		}
		//ESPECIAL			
	}	
	return NULL;
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

int Palavra_Reversada( char buffer[], int pos_buffer){

	int i;	
	char palavra_reservada[10][10] = {"main", "if", "else", "while", "do", "for", "int", "float", "char"};	

	for (i = 0; i < 10; i++){		
		if(strcmp(buffer,palavra_reservada[i])==0)
			return i;
	}
	return 9;
}