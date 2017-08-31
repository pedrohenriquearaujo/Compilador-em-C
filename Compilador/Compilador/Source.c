#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define tam 256

typedef struct token{

	int code;
	char identificador[tam];

}token;

void Abrir_Arquivo(char nome[] , FILE * arq);
token * scan(token * t, FILE arq, int linha, int coluna);

int main(){

	FILE * arquivo=NULL;
	
	int linha=0, coluna=0;
	char c;

	char nome_arq[tam]="arquivo.txt";

	Abrir_Arquivo(nome_arq,arquivo);

	while (!feof(arquivo)){

		printf("Letras: %c", c);


	}
	



	return 0;
}

void Abrir_Arquivo(char nome[] , FILE * arq){	

	arq=fopen(nome,"r");

	if(arq!=NULL){
			
	}else{
		printf("Erro de Abertura do Arquivo!\n");
	}
}
token * scan(token * t, FILE arq, int linha, int coluna){

	char buffer[tam],l;
	int i=0;
	fread(&l,sizeof(l),1,&arq);

	while (true){

	 if(!isspace(l)){
		buffer[i]=l;
		i++;
		checar_token(buffer,t){
		
		}
	}
	 fread(&l,sizeof(l),1,&arq);
		
	}


}

void checar_token(char buffer[], token * t){

	enum tokens {tipo_inteiro, flutante};
	if(strcmp(buffer,"int")==0){
		strcpy(t->identificador,buffer);	
		t->code=tipo_inteiro;
	}else{

	}

}
