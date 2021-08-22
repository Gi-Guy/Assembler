/*Output.c
 * Manages the files output system*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Enum.h"
#include "Settings.h"
#include "Structures.h"
#include "TypeAnalysis.h"
#include "Parsers.h"
#include "LabelWorks.h"
#include "Output.h"

/*Private function*/
void	WriteExternals(char *,	ExternalsLabel	*,int	);
void WriteEntries(char *, EntriesLabel *, int );
long convertToBase(long , int );
void WriteObjects(char *, ObjectsList *, MemoryData *, int , int );

/*Main manager
 * making the output files and calling to other function for the Entries and Externals files*/
void WriteManager(char	*fileName){
	char inputFile[FILE_NAME];
	strcpy(inputFile,fileName);

	/*Write into '.ext' file*/
	if(externLabelCounter>0){
		printf("\nWriting '%s.ext' Externals output file.\n", fileName);
		strcat(inputFile,	".ext");
		/*sending to the externals file*/
		WriteExternals(inputFile,Externals,externLabelCounter);
		printf("Externals output file completed.\n\n");
	}

	/*Write into '.ent' file.*/
	if(entryLabelCounter>0){
		printf("Writing '%s.ent' Entries output file.\n", fileName);
		strcpy(inputFile,fileName);/*reset the file name since we used it in the .ext part*/
		strcat(inputFile,	".ent");
		/*Sending to the Entries file*/
		WriteEntries(inputFile, Entries, entryLabelCounter);
		printf("Entries output file completed.\n\n");
	}

	/*Write into '.ob' file*/
	printf("Writing '%s.ob' Objects output file.\n", fileName);
	/*Reset inputFile*/
	strcpy(inputFile, fileName);
	strcat(inputFile, ".ob");
	/*Sending to Object file*/
	WriteObjects(inputFile, Objects, memory,IC,DC);
	printf("Objects output file completed.\n\n");

}

/*making and write the '.ext" file*/
void	WriteExternals(char *inputFile,	ExternalsLabel	*list,int	counter){
	int i;
	char	line[LINE_LENGTH];

	/*create the file*/
	FILE *file;
	file	=	fopen(inputFile,	"w");
		/*Checks if file created*/
	if(!file){
		printf("ERROR!\t can't create '%s' External file.\n", inputFile);
		return;
	}
	/*Wrtie into file*/
	else if (file){
		for(i=0;	i<counter;	i++){
			if(list[i].decAddress>=BASE_ADDRESS)
				sprintf(line, "%s\t %ld\n",list[i].name, convertToBase(list[i].decAddress,OUTPUT_BASE));
				fputs(line,file);
		}
		fclose(file);
	}
}

void WriteEntries(char *inputFile, EntriesLabel *list, int counter){
	int i;
	char line[LINE_LENGTH];

	/*Create the file*/
	FILE *file;
	file	=	fopen(inputFile, "w");
		/*Check if file created*/
	if(!file){
		printf("ERROR!\t can't create '%s' Entries file.\n", inputFile);
		return;
	}
	else if(file){
		for(i=0; i<counter; i++){
			sprintf(line, "%s\t %ld\n",list[i].name, convertToBase(list[i].decAddress, OUTPUT_BASE));
			fputs(line, file);
		}
		fclose(file);
	}
}
void WriteObjects(char *inputFile, ObjectsList *objects, MemoryData *memory, int tempIC, int tempDC){
	int i;
	long number;
	char line[LINE_LENGTH];
	/*Create new file*/
	FILE *file;
	file	=	fopen(inputFile, "w");
		/*Checks if file created*/
		if(!file){
			printf("ERROR!\t can't create '%s' Objects file.\n", inputFile);
			return;
		}
		/*print the amount of commands */
		else if(file){
			sprintf(line,"\t%ld		%ld\n", convertToBase(tempIC,OUTPUT_BASE), convertToBase(tempDC,OUTPUT_BASE));
			fputs(line, file);

			/*print the IC side*/
			for (i=0; i<tempIC; i++){
				number	=	objects[i].Machine.code;
				sprintf(line,	"%ld\t%.6ld\n", convertToBase(objects[i].decAddress,OUTPUT_BASE), convertToBase(number,OUTPUT_BASE));
				fputs(line, file);
			}
			for (i=0; i<tempDC; i++){
				number = memory[i].Machine.code;
				sprintf(line,"%ld\t%.6ld\n", convertToBase(tempIC+BASE_ADDRESS+i, OUTPUT_BASE), convertToBase(number,OUTPUT_BASE));
				fputs(line, file);
			}
			fclose(file);
		}
}
/*Converts decimal number to 'base' number*/
long convertToBase(long num, int base){
	long newValue = 0;
	int carry;

	carry =1;
	while(num > 0){
		newValue += carry * (num % base);
		num /= base;
		carry *= 10;
	}
	return newValue;
}
