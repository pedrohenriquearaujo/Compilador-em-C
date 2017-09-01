#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define tam 256
#define true 1

typedef struct token{

	int code;
	char identificador[tam];

}token;


void Abrir_Arquivo(char nome[] , FILE ** arq);
token * scan(FILE * arq, int * linha, int * coluna);
int Palavra_Reversada( char buffer[], int pos_buffer);
int main(){

	FILE * arquivo=NULL;
	token * t;
	int linha=0, coluna=0;
	

	char nome_arq[tam]="arquivo.txt";

	Abrir_Arquivo(nome_arq,&arquivo);

	while (!feof(arquivo)){

		t=scan(arquivo,&linha,&coluna);

		printf("Letras: %c", t->code);		
		printf("Letras: %c", t->identificador);

	}
	



	return 0;
}

void Abrir_Arquivo(char nome[] , FILE ** arq){	

	*arq=fopen(nome,"r");

	if(*arq!=NULL){
		printf("Abertura do Arquivo!\n");	
	}else{
		printf("Erro de Abertura do Arquivo!\n");
	}
}
token * scan(FILE * arq, int * linha, int * coluna){
	enum tokens {t_main, t_if, t_else, t_while, t_do, t_for, t_int, t_float, t_char};
	token * t=NULL;
	char buffer[tam];
	static char l_h = ' ';
	int pos_buffer=0;
	

	while (true){

		//ESPAÇOS
		if(l_h == ' ')		
		coluna++;		
		else if(l_h == '\t')		
		coluna+=4;		
		else if(l_h == '\n')			
		linha++;		
		
		//LETRAS
		if(l_h == '_' || isalpha(l_h) ){
			
			buffer[pos_buffer]=l_h;
			pos_buffer++;
			fread(&l_h,sizeof(l_h),1,arq);

			while (l_h == '_' || isalnum(l_h) ){
				buffer[pos_buffer]=l_h;
				pos_buffer++;
				fread(&l_h,sizeof(l_h),1,arq);
			}
			buffer[pos_buffer]='\0';
			strcpy(t->identificador,buffer);
			t->code=Palavra_Reversada(buffer,pos_buffer);			
		}

		fread(&l_h,sizeof(l_h),1,arq);
	}
}



int Palavra_Reversada( char buffer[], int pos_buffer){

	int i;	
	char palavra_reservada[10][10] = {"main", "if", "else", "while", "do", "for", "int", "float", "char"};	

	for (i = 0; i < 10; i++){		
		if(strcmp(buffer,palavra_reservada[i])==0)
			return i;
	}
	return 10;
}