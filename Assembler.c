/*
 * Assembler.c
 * Project:	Assembler projects, receives 'x.as' file and parser it.
 * 				If Success then Assembler will create output files and will stop,
 * 				otherwise Assembler will print errors outputs and will stop after it will find all possible errors.
 * 				The Assembler will ask you nicely to fix a specific lines so you'll know what are the errors.
 *
 * NOTES:	1) Usually the Assembler will only print on console errors and will keep running to the end of the file,
* 					However It won't be forgiving in certain cases and will stop immediately.
* 					In case like that It will print an specific error so you'll know the case.
*
* 				2) The Assembler is relatively flexible, You can change an specific Settings in the Settings.h file.
* 					The reason for this note is, In case you'll find any error, you'll know what main settings I did and you won't need to do some annoying searches.
*				3) In each x.c and x.h file there is an short description about the works it done.
 * */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Enum.h"
#include "Settings.h"
#include "Structures.h"
#include "TypeAnalysis.h"
#include "LabelWorks.h"
#include "Parsers.h"
#include "Assembler.h"
#include "Output.h"

void Reset();
void Parsing(char *, int ,	PassCycles);
void Start(char	[]);
void Reset();


/*-----Main	Memory	Management-----*/
ObjectsList Objects[MAX_MEMORY_SIZE];
MemoryData	memory[MAX_MEMORY_SIZE];
int IC=0;
int DC=0;
/*-----End	Main	Memory	Management-----*/

/*-----Labels	Management-----*/
LabelList	*Labels	;
EntriesLabel	*Entries;
ExternalsLabel	*Externals;
/*-----End	Labels	Management-----*/

/*-----Previous	Operands	Management-----*/
int previousObjectLocation	=0;


/*-----Parsers	Tools-----*/
ArgumentType	argType;
Boolean	labelExist	=	false;
Boolean Error	=	false;


/*counters and accessories variables */
int correctIndex=0;
int cmdIndex=-1;
int regIndex=-1;
int codingCycles=-1;
char fixedLine[INS_COMMAND_LEN];
char	*currentArg;
char *currentCmd;

/*-----Labels Counters-----*/
int labelCounter=0;
int externLabelCounter=0;
int	entryLabelCounter=0;


/*Define valid registers*/
/*Each register - (index, name)*/
RegistersList registers[REGISTERS_AMOUNT]={
	{0,"r0"},
	{1,"r1"},
	{2,"r2"},
	{3,"r3"},
	{4,"r4"},
	{5,"r5"},
	{6,"r6"},
	{7,"r7"}
};
/*Define valid opcode commands*/
/*Each opcode - (name, index, group/number of operands)*/
commandsList opcode[OPCODE_AMOUNT]={
		{"mov",0,2},
		{"cmp",1,2},
		{"add",2,2},
		{"sub",3,2},
		{"not",4,1},
		{"clr",5,1},
		{"lea",6,2},
		{"inc",7,1},
		{"dec",8,1},
		{"jmp",9,1},
		{"bne",10,1},
		{"red",11,1},
		{"prn",12,1},
		{"jsr",13,1},
		{"rts",14,0},
		{"stop",15,0}
};

/*Main function*/
 int main(int argc, char *argv[]){
	 /*reading files*/
	 char *inputFile;
	 int i;
	 /*Case there are no input files*/
	 if (argc	==	1){
		 printf("ERROR!\tNo input has been found!\n");
		 printf("Put:\t./Assembler\t<file1>[<file2>\t<file3>..]\n");
		 return EXIT_FAILURE;
	 }
	 /*Taking care of multi inputs*/
	 for(i=1; i<argc;	i++){
		 /*making sure to reset variables*/
		 if(i>1)
			 Reset();
		 /*Define memory variables and sending to work*/
		 	inputFile	=	(char*)malloc((int)strlen(argv[i])+(int)strlen(INPUT_FILE)+1);
		 	strcpy(inputFile, argv[i]);
		 	strcat(inputFile,INPUT_FILE);
			Labels=(LabelList*) malloc(LABEL_AMOUNT*(sizeof(LabelList)));
			Entries=(EntriesLabel*)malloc(LABEL_AMOUNT*(sizeof(EntriesLabel)));
			Externals=(ExternalsLabel*)malloc(LABEL_AMOUNT*(sizeof(ExternalsLabel)));

		Start(inputFile);

		free(inputFile);
		free(Labels);
		free(Entries);
		free(Externals);
	 }

return	EXIT_SUCCESS;

}

 /*Main parsing function.
  * Receives line input, line number and PassCycles.
  *  sending to getType function and analyze it the input line.*/
void Parsing(char *line, int lineNumber,	PassCycles	pass){
	LabelList	*tempL;
	ExternalsLabel	*tempEx;
	EntriesLabel	*tempEn;
	char currentLine[INS_COMMAND_LEN];
	static char labelName[LABEL_LEN];
	int labelsCounter	=	0;

	/*Get line and keeps original as backup*/
	strncpy(currentLine, line, INS_COMMAND_LEN);
	strncpy(fixedLine, line, INS_COMMAND_LEN);
	currentArg	=	strtok(currentLine,	" \t\n");

	/*We run at least once on that line to analyze it*/
	do{

		if(currentArg)
			argType=getType(currentArg,registers, opcode, lineNumber,&cmdIndex);
		else{/*Case there is no currentArg*/
			argType=Empty;
			printf("[Line:\t%d]\t line is empty.\n",lineNumber);
		}
		/*Case is argType is an Label?*/
		if(	argType	==	Label	){
			/*Counting number of labels per line, and checking the current amount*/
			labelsCounter++;
			/*Line can not have more then one symbol*/
			if(	labelsCounter>1	){
				printf("[Line:\t%d]\tIt can't be more than one Label per line.\n",lineNumber);
				Error=true;
				return;
			}
			/*Checking if label already exist, Will print only Error and will keep running*/
			if (labelIsExist(currentArg, Labels) && pass==First){
				printf("[line:\t%d]\t Warning! Label '%s' already exist and been used.\n", lineNumber, currentArg);
				Error=true;
			}
			/*If first Pass cycles, then keeps label into memory*/
			if(	pass==First	){
				labelExist	=	true;
				memset(labelName,0,LABEL_LEN);
				/*keeps the label name without the ':'*/
				strncpy(labelName, currentArg, strlen(currentArg) -1);

				/*Checks if label list is empty*/
				if(((labelCounter+1)%LABEL_AMOUNT)	==	0){
					tempL	=	(LabelList*)	realloc	(Labels,	(LABEL_AMOUNT	+	labelCounter)	*	sizeof(LabelList));
					if(tempL)
						Labels=tempL;
					else{/*Allocate failed*/
						printf("ERROR:\t	Can't	allocate	memory	for	symbol	table.\n");
						exit(1);
					}
				}
			}
			/*If pass!=First, then skip*/
			/*Moving to the next argument on line*/
			currentArg=strtok(NULL," \t\n");
		}

		/*Case	is	argType is an InstructionData or InstructionString?*/
		else	if(argType==InstructionData	||	argType==InstructionString){
			/*If first Pass Cycles then(...), else skip*/
			if(	pass==First	){
				/*Keeps the Label we have found.*/
				if(labelExist){
					buildLabels(Labels,&labelCounter,labelName,0,DC,DATA);
					labelExist=false;

				}
				/*Sending to parseData to keeps in memory*/
				if(!ParseData(argType, fixedLine, lineNumber)){
					printf("[Line:\t%d]\tFailed to analyze data.\n",lineNumber);
				}
			}
			return;
		}
		/*Case is argType is an InstructionExtern?*/
		else if(argType==InstructionExtern){
			/*calling the label name after '.extern'*/
			currentArg	=	strtok(NULL,	" \t\n");
			/*if first pass and there is a label name, then should update Extern list*/
			if (pass==First){
				if(currentArg){
					/*Checks if the list is empty*/
					if(((externLabelCounter+1)%LABEL_AMOUNT)==0){
						tempEx=	(ExternalsLabel	*)	realloc(Labels,	((LABEL_AMOUNT+externLabelCounter)*sizeof(ExternalsLabel)));
						if(tempEx)
							Externals=tempEx;
						else{/*Allocate failed*/
							printf("ERROR:\t	Can't	allocate	memory	for	Externals	symbol	table.\n");
							exit(1);
						}
					}
					/*Sending to other function to build*/
				buildExternalLabels(Externals,currentArg,0,&externLabelCounter);
				}
				/*ERROR	-	there is no label after .extern*/
				else{
					printf("[Line:\t%d]\tNo Label has been found after '.extern'.\n",lineNumber);
					Error=true;
					return;
				}
			}
			return;
		}
		/*Case	is argType is an InstructionEntry?*/
		else if(argType==InstructionEntry){
			/*calling the label name after '.entry'.*/
			currentArg	=	strtok(NULL,	" \t\n");
			/*if first pass then should update Entry list*/
			if(pass==First){
				if(currentArg){
					/*Checks if list is empty*/
					if((entryLabelCounter+1)%LABEL_AMOUNT==0){
						tempEn=(EntriesLabel	*)	realloc	(Labels,	((LABEL_AMOUNT+entryLabelCounter)*sizeof(EntriesLabel)));

						if(tempEn)
							Entries=tempEn;
						else{/*Allocate failed*/
							printf("ERROR:\t	Can't	allocate	memory	for	Entries		symbol	table.\n");
							exit(1);
						}
					}
					/*Sending to other function to build*/
					buildEntriesLabels(Entries, currentArg,0,	&entryLabelCounter);
				}
					/*No label has been found*/
				else{
					printf("[Line:\t%d]Error:\t No	Label	has	been	found	after	'.entry'.\n",lineNumber);
					Error=true;
					return;
				}
			}
			else{/*In the second pass we could update the Entry labels*/
				EntryUpdate(argType, currentArg);
			}
			return;
		}
		/*Case	is argTyps is and OpcodeCommand? */
		else if(argType==OpcodeCommand){
			/*If there is a label and command, then keeps the labels on list*/
			if((pass==First)	&&	labelExist){
				buildLabels(Labels, &labelCounter,labelName,(BASE_ADDRESS+IC),IC,CODE);
				labelExist=false;
			}
			/*Then sending to ParaseCommands to build the objects and operands*/
			if(!ParaseCommands(cmdIndex,fixedLine,lineNumber,pass)){
				printf("[line:\t%d]\tERROR!\tCould not analyze %s.\n",lineNumber, currentArg);
			}
			return;
		}
		/*line is empty or comment*/
		else if(	argType==Empty	||	argType==Comment	){
			return;
		}
		else{/*Something went wrong, Print error and keep running.*/
			printf("[line:\t%d]\tERROR!\tUnknown instruction for '%s' input.\n",lineNumber,currentArg );
			Error=true;
			return;
		}
	}while(	currentArg!=NULL	&&	argType!=Empty	);
}
/*Starting the Assembler works.
 * Receives inputFile and will read lines and send to Parsing.
 * Note: This function is responsible for both First pass and Second Pass.*/
void Start(char	inputFile[]){
	FILE *file;
	char	fileName[FILE_NAME];
	char	*temp, *str;
	char line[LINE_LENGTH];
	int lineCounter=0;
	PassCycles pass;

	/*Checks if the file's name isn't bigger then allowed - Value can be changed in Setting file*/
	if(strlen(inputFile)	>	FILE_NAME){
		printf("Error!\t File '%s' name can not be bigger than allowed.\n", inputFile);
		return;
	}
	/*File allowed, get to work*/
	else{
		file	=	fopen(inputFile,"r");
		if(file){
			/*Starting the first pass*/
			printf("#(1)\tStarting the first pass in file %s\n", inputFile);
			pass = First;
			/*Reset counter and sending to analyze*/
			lineCounter	=	0;
			temp = fgets(line,LINE_LENGTH, file);

			/*Checks if file is empty*/
			if(!temp){
				printf("ERROR!\tfile '%s' empty.\nThe Assembler can not work with empty file!\n",inputFile);
				return;
			}
			/*Analyze each line and update the lineCounter*/
			while(temp){
				lineCounter++;
				Parsing(line,	lineCounter, pass);
				temp	=	fgets(line, LINE_LENGTH, file);
			}
			/*Checks if found any error*/
			if(!Error)
				/*No errors*/
				printf("#(1)The First pass completed for file '%s' with total of %d lines.\n", inputFile,lineCounter);
			else
				/*Found error*/
				printf("Warning:\tFound errors in the first Pass.\n");
			/*In the end of the First pass We could update the 'DATA' labels Address*/
			UpdateDataLabels();

			/*END OF THE FIRST PASS!*/
			/*Starting the second pass*/
			printf("\n\n#(2)Starting the Second pass in file %s\n", inputFile);
			pass=Second;
			/*Reset variables*/
			IC=0;
			lineCounter=0;
			/*Reading the file again*/
			fseek(file,0,SEEK);
			temp	=	fgets(line, LINE_LENGTH, file);

			/*Update each line with what it need.*/
			while(temp){
				lineCounter++;
				Parsing(line,	lineCounter, pass);
				temp	=	fgets(line, LINE_LENGTH, file);
			}
			/*Checks if found any Errors*/
			if(!Error){/*MAKE SURE TO RESET THE ERROR AFTER THE FIRST PASS!*******************************************************/
				printf("#(2)The Second pass completed for file '%s' with total of %d lines.\n", inputFile, lineCounter);
				/*If there are no errors (OMG NO ERRORS!) then we	(FUCKING YEAH!) could update the output files (I LOVE NO ERRORS STATE! :D)*/
				strcpy(fileName,	inputFile);
				str	=	strrchr(fileName,	'.');
				*str	=	'\0';
				WriteManager(fileName);
				printf("\n#Success!\t The process completed successfully for '%s' file.\n", inputFile);
			}
			else{/*Found Error!*/
				printf("Warning:\tFound errors in the second Pass.\n");
				printf("ERROR!\tCan not make output files, Please fix Error lines.\n");
			}

			fclose(file);
		}
		/*There is a problem with the file, or can't be open*/
		else{
			printf("ERROR!\tAssembler can't open '%s' file!\n", inputFile);
			return;
		}
	}
}
void Reset(){
	if (IC>0)
		memset(Objects, 0, (IC-1) * sizeof(*Objects));
	labelExist	=	false;
	labelCounter	=	0;
	entryLabelCounter	=	0;
	externLabelCounter	=	0;
	cmdIndex	=	-1;
	IC	=	0;
	DC	=	0;
	Error	=	false;
	previousObjectLocation	=	0;
}

