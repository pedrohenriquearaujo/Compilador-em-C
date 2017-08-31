#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define tam 256

void Abrir_Arquivo(char nome[] , FILE * arq);

int main(){

	FILE * arquivo=NULL;

	char nome_arq[tam]="arquivo.txt";

	Abrir_Arquivo(nome_arq,arquivo);


	



	return 0;
}

void Abrir_Arquivo(char nome[] , FILE * arq){	

	arq=fopen(nome,"r");

	if(arq!=NULL){
			
	}else{
		printf("Erro de Abertura do Arquivo!\n");
	}
}