#include <stdio.h>
#include <stdlib.h>
#include <string.h>
FILE *f1;
typedef struct {
 short int ETQ;
 short int Datos[8];
} T_LINEA_CACHE;
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
//T_LINEA_CACHE cache[4];


void imprimir_cache(T_LINEA_CACHE cache[]);
char* act_texto(int dato,char* texto);
int main(int argc, char *argv[]) {
	//printf("HOLA MUNDO");
	T_LINEA_CACHE cache[4];
	int i;
	int j;
	int tiempoglobal=0;
	int numfallos=0;
	int numero_accesos=0;
	char texto[100]="";
	
	
	//establecer datos
	for(i=0;i<4;i++){
		cache[i].ETQ=0xFF;		
		for(j;j<8;j++){
			cache[i].Datos[j]=0x0;
		}
	}
	//establecer datos***
//Lectura RAM.bin
/* Apertura del fichero original, para lectura en binario*/
f1 = fopen ("RAM.bin", "rb");
	if (f1==NULL){
   		printf("No se puede abrir RAM.bin");
   		//return -1;
   		exit(-1);
	}
	unsigned char RAM[1024];
	fread (RAM, 1, 1024, f1);
	
	fclose(f1);
	//_lectutad RAM.bin
	
	//lectura accesos_memoria-----------------------------------------------------------------
	FILE* fp;
	char buffer[4];
	
	int direcc;
	int etq;
	int linea;
	int palabra;
	int bloque;
	
	fp = fopen("accesos_memoria.txt", "r");
	//Saco la direccion de memoria
	//fgets(buffer, 255, (FILE*) fp);
	while(fgets(buffer, 255, (FILE*) fp)) {
        printf("\n\nBuffer: %s", buffer);
        
		numero_accesos++;
		tiempoglobal++;
		
		char binario[16]="";
		
		
		//TRADUCIR DIRECCIONES
	
		direcc=(int)strtol(buffer,NULL,16);
		
	
		palabra=direcc & 0b111;
		linea=direcc >> 3 & 0b11;
		etq=direcc >> 5 & 0b11111;
		bloque=direcc >> 3 & 0b1111111;
		
		
		if(etq != cache[linea].ETQ){
			//printf("\n\tERROR");
			numfallos+=1;
			
			printf("\nT: %d, Fallo de CACHE %d, ADDR %04X ETQ %X linea %02X palabra %02X bloque %02X",tiempoglobal,numfallos,direcc,etq,linea,palabra,bloque);
			tiempoglobal+=10;
			
			//cargar datos RAM
			printf("\nCargando el bloque %02X en la linea %02X",bloque,linea);
			
			
			cache[linea].ETQ=etq;
			//printf("Datos:");
			int var=bloque*8;
			int var_2=var;
			int tope=var+8;
			for(var;var<tope;var++){
				cache[linea].Datos[var-var_2]=RAM[var];
				
			}
			//cargar datos RAM***
			int dato=cache[linea].Datos[palabra];
			
			printf("\nT: %d, Acierto de CACHE, ADDR %04X ETQ %X linea %02X palabra %02X DATO %02X\n",tiempoglobal,direcc,etq,linea,palabra,dato);
			imprimir_cache(cache);
			//sleep(2);
			
			act_texto(dato,texto);
			
		}else{
			//printf("\n\tACIERTO");
			int dato=cache[linea].Datos[palabra];
			
			printf("\nT: %d, Acierto de CACHE, ADDR %04X ETQ %X linea %02X palabra %02X DATO %02X\n",tiempoglobal,direcc,etq,linea,palabra,dato);
			imprimir_cache(cache);
			act_texto(dato,texto);
			
			//sleep(2);
			
			
		}
    }//fin<<~~	
	fclose(fp);
	float t_medio=(float)tiempoglobal/numero_accesos;
	printf("\nt global: %d",tiempoglobal);
	printf("\nN accesos: %d",numero_accesos);
	printf("\nt medio: %f",t_medio);
	printf("\n\nNumero total accesos: %d, Numero fallos: %d, Tiempo medio: %.2f",numero_accesos,numfallos,t_medio);
	
	printf("\n\nTEXTO: %s",texto);
	
	
	
	//_lectura accesos_memoria----------------------------------------------------------------
	
	

	printf("\n--------------------------------");

	return 0;
}
void imprimir_cache(T_LINEA_CACHE cache[]){
	
	int i,j;
	for(i=0;i<4;i++){
		printf("\nETQ:%X\t",cache[i].ETQ);
		printf("Datos:");
		for(j=7;j>-1;j--){			
			printf(" %X",cache[i].Datos[j]);
		}
	}
	
}
char* act_texto(int dato,char* texto){
	char temp[3]="";
	temp[0]=dato;
	strcat(texto,temp);
	//printf("\n\nTEXTO: %s\n\n",texto);

	return texto;
}

