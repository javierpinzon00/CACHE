//CACHEsym.c
//JAVIER PINZON DIAZ
//HUGO ROMAN
//INSO 2A
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE_TEXTO 100
#define SIZE_RAM 1024
//FILE *f1;
typedef struct {
 short int ETQ;
 short int Datos[8];
} T_LINEA_CACHE;

T_LINEA_CACHE cache[4];//Array de 4 de T_LINEA_CACHE

//DECLARACION FUNCIONES
void imprimir_cache(T_LINEA_CACHE cache[]);
char* act_texto(int dato,char* texto);
void error(int* numfallos,int* tiempoglobal,int direcc,int etq,int linea,int palabra,int bloque,T_LINEA_CACHE* cache,unsigned char RAM[]);
void acierto(int tiempoglobal,int direcc,int etq,int linea,int palabra,int dato);
// DECLARACION FUNCIONES***

int main(int argc, char *argv[]) {
		
	int i,j,tiempoglobal=0,numfallos=0,numero_accesos=0;
	char texto[SIZE_TEXTO]="";	
	
	//Establecer datos cache
	for(i=0;i<4;i++){
		cache[i].ETQ=0xFF;		
		for(j;j<8;j++){
			cache[i].Datos[j]=0x0;
		}
	}
	//Establecer datos cache***
	
	//Lectura RAM.bin
	FILE *fRAM;
	fRAM = fopen ("RAM.bin", "rb");
		if (fRAM==NULL){	//Error apertura fichero RAM.bin
   			printf("No se puede abrir RAM.bin");
   			exit(-1);
		}
	unsigned char RAM[SIZE_RAM];
	fread (RAM, 1, SIZE_RAM, fRAM);
	
	fclose(fRAM);
	//Lectura RAM.bin***
	
	//lectura accesos_memoria.txt
	FILE* fACCESOS_MEMORIA;
	char buffer[4];//Array donde se va a ir almacenando cada direccion de memoria
	
	int direcc, etq, linea, palabra, bloque;
	
	fACCESOS_MEMORIA = fopen("accesos_memoria.txt", "r");
		if (fACCESOS_MEMORIA==NULL){	//Error apertura fichero accesos_memoria.txt
   			printf("No se puede abrir accesos_memoria.txt");
   			exit(-1);
		}
	//Analizamos direcciones de memoria
	while(fgets(buffer, 255, (FILE*) fACCESOS_MEMORIA)) {	//Coge linea a linea del documento
        //printf("\n\nBuffer: %s", buffer);
		numero_accesos++; //Aumentamos el numero de accesos
		tiempoglobal++;	//Aumentamos en 1 el tiempo global
			
		direcc=(int)strtol(buffer,NULL,16);	//Pasa el buffer a decimal
		
		palabra=direcc & 0b111;			//Selecciona los 3 bits menos significativos.
		linea=direcc >> 3 & 0b11;		//Desplaza la direccion 3 veces y alamcena 2 bits.
		etq=direcc >> 5 & 0b11111;		//Desplaza la direccion 5 veces y alamcena 5 bits.
		bloque=direcc >> 3 & 0b1111111;	//El bloque es la etiqueta y la linea juntas.
		
		int dato; //Cuando hay acierto
		
		short fallo;
		if(etq != cache[linea].ETQ){fallo=1;}else{fallo=0;}	//Comprobar si la ETQ de la direccion esta cargada en la cache.
		switch(fallo) {
			case 1:	//Si hay error se carga los datos y pasa a la funcion acierto
				//FALLO			
				error(&numfallos,&tiempoglobal,direcc,etq,linea,palabra,bloque,cache,RAM);//Funcion error.
			case 0:		
				//ACIERTO
				dato=cache[linea].Datos[palabra];	//Sacamos el dato.
				acierto(tiempoglobal, direcc, etq, linea, palabra, dato);	//Funcion acierto.
				imprimir_cache(cache);	//Imprime la cache.
				act_texto(dato,texto);	//Actualia la variable texto.			
			break;
		}
    }
	//Analizamos direcciones de memoria***
	fclose(fACCESOS_MEMORIA);
	//lectura accesos_memoria.txt***
	
	float t_medio=(float)tiempoglobal/numero_accesos;	//Calculamos el tiempo medio de acceso.
	printf("--------------------------------");
	printf("\nNumero total accesos: %d, Numero fallos: %d, Tiempo medio: %.3fs.",numero_accesos,numfallos,t_medio);//Imprime el numero de accesos, los fallos y el tiempo medio de acceso
	
	printf("\n\nTEXTO: %s",texto);	//Te imprime el texto final

	return 0;
}//FIN MAIN
void imprimir_cache(T_LINEA_CACHE cache[]){
	
	int i,j;
	printf("\n");
	for(i=0;i<4;i++){
		printf("\nETQ:%X\t",cache[i].ETQ);
		printf("Datos:");
		for(j=7;j>-1;j--){			
			printf(" %X",cache[i].Datos[j]);
		}
	}
	printf("\n");
	sleep(2);	//Sleep de 2 segundos
	
}
char* act_texto(int dato,char* texto){//Guarda el dato en un string para poder concatenarlo con strcat()
	char temp[3]="";
	temp[0]=dato;
	strcat(texto,temp);
	return texto;
}
void error(int* numfallos,int* tiempoglobal,int direcc,int etq,int linea,int palabra,int bloque,T_LINEA_CACHE* cache,unsigned char RAM[]){

			*numfallos+=1;
			
			//Mensaje de fallo de cache
			printf("\nT: %d, Fallo de CACHE %d, ADDR %04X ETQ %X linea %02X palabra %02X bloque %02X",*tiempoglobal,*numfallos,direcc,etq,linea,palabra,bloque);
			*tiempoglobal+=10;	//Se aumenta el tiempo en 10		
			
			printf("\nCargando el bloque %02X en la linea %02X",bloque,linea);	
			
			//cargar datos RAM
			cache[linea].ETQ=etq;
			
			int var=bloque*8;
			int var_2=var;
			int tope=var+8;
			for(var;var<tope;var++){
				cache[linea].Datos[var-var_2]=RAM[var];
			}
}
void acierto(int tiempoglobal,int direcc,int etq,int linea,int palabra,int dato){
	printf("\nT: %d, Acierto de CACHE, ADDR %04X ETQ %X linea %02X palabra %02X DATO %02X",tiempoglobal,direcc,etq,linea,palabra,dato); //Mensaje de acierto en cache
}