/*
 * Parsers.c
 * This file is responsible for all parsers works.
 * */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Enum.h"
#include "TypeAnalysis.h"
#include "Parsers.h"
#include "LabelWorks.h"

/*private function*/
Boolean PrevObjectCopy(int , int , OperandType	);
void updateObjects(int , OperandType , AddressingMethod , int , int );

int tempIC;
AddressingMethod	prevAddresing	=	Off;

/*Main Parse function, will build an objects for each cmd and it rules.*/
Boolean ParaseCommands(int cmdIndex, char *fixedLine, int lineNumber,PassCycles Pass){
	int coding=codingCycles;
	int tempCmdIndex	=	cmdIndex;
	tempIC	=	IC;
	/*making sure that previous has been reset*/
	if(IC==0)
		previousObjectLocation	=	IC;
	/*opcode must have 1 or 2 codingCycles. EX: mov2*/
	if(coding<=0	||	coding>2){
		printf("[line:\t%d]\tCould not get any coding cycles for command.\n", lineNumber);
		return false;
	}
	while(coding>0){/*Doing all coding works*/
	/*******************************************************************************************
	 *	Self	Encoding	Into	Machine	Code and updating Variables.
	 * *****************************************************************************************/
	/*coding into machine code*/
	/*Group	-	11-10*/
	Objects[tempIC].Machine.code	=	(Objects[tempIC].Machine.code)	|	(opcode[cmdIndex].group<<10);
	/*Opcode	-	9-6*/
	Objects[tempIC].Machine.code	=	(Objects[tempIC].Machine.code)	|	(cmdIndex<<6);
	/*update decAddress*/
	Objects[tempIC].decAddress	=	BASE_ADDRESS	+	tempIC;
	/*Mark as 'First' word*/
	Objects[tempIC].ObjectType.cmd	=	true;

	/*******************************************************************************************
	 *Sending The	Rest	Of	Encoding	To	Other	Functions
	 * *****************************************************************************************/
		/*Case of one Operand*/
		if(opcode[tempCmdIndex].group==1){
			if(!(oneOperand(fixedLine,lineNumber,Pass,coding)))	return false;	}/*Found Error*/
		/*Case of two Operands*/
		else if(opcode[tempCmdIndex].group==2){
			if(!(twoOperands(fixedLine,lineNumber,Pass,coding)))	return false;}
		/*Case of none Operands*/
		else if (opcode[tempCmdIndex].group==0){/*taking care of 'rts' and 'stop' cmd, all they need is opcode bits*/
			Objects[tempIC].size=1;/*'stop' or 'rts' don't care about operands*/
			IC++;
			previousObjectLocation=tempIC;
			prevAddresing	=	Off;
			return true;
		}
		else{/*Something went wrong, going back.*/
			return false;
		}
	/*Define the size of the current object*/
	if((Objects[tempIC].size=(IC-tempIC+1))>3){
		printf("[line:\t%d]\tSomething went wrong, Cmd coding size can not be bigger then 3.\n", lineNumber);
		return false;
		}
	cmdIndex=tempCmdIndex;
	tempIC	=	++IC;
	coding--;
	}

	/*End Function*/
	return true;
}
/*Building the operands Objects.*/
Boolean ParseOperands	(OperandType	opType, int lineNumber, PassCycles pass){
	int tempRegIndex,	labelAddres,	number;
	/*******************************************************************************************
	 *Step	1:	Checks if 'Direct Register Addressing'?	Step	2	:	skip.
	 *Step	2:	Coding	'Direct Register Addressing'	into	machine	code.
	 * *****************************************************************************************/
	if(isValidReg(&tempRegIndex,currentCmd,registers)){
		/*Operand is an Register*/
		/*Checks if the Addressing Method is legal*/
			if(IsLegalAddressingMethod(cmdIndex,opType,Register)){

				/*If Source Register operand*/
				if(opType==Source){
					/*update the cmd machine code*/
					Objects[tempIC].Machine.code	=	(Objects[tempIC].Machine.code)	|(0x3<<4);
					/*Update the register*/
						IC++;
						Objects[IC].Machine.code	=	0;
						/*update the Objects variables*/
						updateObjects((BASE_ADDRESS+IC),	opType,	Register,	tempRegIndex,	IC);
						/*Update MachineCode*/
						Objects[IC].Machine.code	=	(Objects[IC].Machine.code)	|	(tempRegIndex<<7);


						/*Mark as Absolute and update the bits*/
						Objects[IC].Type=Absolute;
						Objects[IC].Machine.code	=	Objects[IC].Machine.code	&	(MACHINE_MASK);
						/*Mark this operand as Register, In case next operand is an register too*/
						prevAddresing	=	Register;/*RESET THIS!*/
						return true;
				}
				else{/*Updating for Destination*/
					/*update the cmd machine code*/
					Objects[tempIC].Machine.code	=	(Objects[tempIC].Machine.code)	|(0x3<<2);
					/*Register is the only operand or prev operand isn't Register*/
						if((tempIC	==	IC)	||(prevAddresing!=Register)){
								IC++;
								Objects[IC].Machine.code	=	0;
								/*update the Objects variables*/
								updateObjects((BASE_ADDRESS+IC),	opType,	Register,	tempRegIndex,	IC);
								/*Update machineCode*/
								Objects[IC].Machine.code	=	(Objects[IC].Machine.code)	|	(tempRegIndex<<2);

								/*Mark as Absolute and update the bits*/
								Objects[IC].Type=Absolute;
								Objects[IC].Machine.code	=	Objects[IC].Machine.code	&	(MACHINE_MASK);
								return true;
							}
						/*The previous Operand is an Register, then we should work in the same IC*/
						else	if(prevAddresing	==	Register){
								/*update the Objects variables*/
								updateObjects((BASE_ADDRESS+IC),	Both,	Register,	tempRegIndex,	IC);/*CHECK THIS CASE!**************************************************/

								/*in case of both then we do self update*/
								Objects[IC].ObjectType.Type	=	Both;
								Objects[IC].ObjectType.DestinationValue	=	tempRegIndex;

								/*update two registers into one machine code*/
								Objects[IC].Machine.code	=	(Objects[IC].Machine.code)	|	(tempRegIndex<<2);
								return true;
						}
					}
				}
			/*Operand is an Illegal Register, Print error and return false*/
			else{
				printf("[line:\t%d]\tIllegal Addressing for %s Register operand.\n", lineNumber, currentCmd);
				return false;
			}
			return true;
	}
	/*******************************************************************************************
	 *Step	1:	Checks if 'Direct Addressing'?	Step	2	:	skip.
	 *Step	2:	Coding	'Direct Addressing'	into	machine	code.
	 * *****************************************************************************************/
	/*Checks if the operand is an Label*/
	if(isValidLabel(currentCmd,registers,opcode)){
		/*Operand is an Label, Checks if the Addressing is legal*/
		if(IsLegalAddressingMethod(cmdIndex, opType, Direct)){

			if(opType==Source){/*Update the Source	specific bits*/
				Objects[tempIC].Machine.code	=	(Objects[tempIC].Machine.code)	|(0x1<<4);
				IC++;
				Objects[IC].Machine.code	=	0;
				updateObjects((BASE_ADDRESS+IC),	Source,	Direct,	-1,	IC);
				/*Mark as Relocatable*/
				Objects[IC].Type	=	Relocatable;
			}

			else{/*Updating for Destination*/
				Objects[tempIC].Machine.code	=	(Objects[tempIC].Machine.code)	|(0x1<<2);
				IC++;
				Objects[IC].Machine.code	=	0;
				updateObjects((BASE_ADDRESS+IC),	Destination,	Direct,	-1,	IC);
				/*Mark as Relocatable*/
				Objects[IC].Type	=	Relocatable;
			}
			/*In the second pass we should update the machine code into label address*/
			if(pass	==Second){
				labelAddres	=	GetLabelAddress(currentCmd,Labels,Externals);
				if(labelAddres!=-1 && labelAddres!=0){
				Objects[IC].Machine.code	=	(Objects[IC].Machine.code)	|	(labelAddres<<2);
				/*Mark as Relocatable*/
				Objects[IC].Machine.code	=	(Objects[IC].Machine.code)&(MACHINE_MASK);
				Objects[IC].Machine.code	=	(Objects[IC].Machine.code)|	(0x2);
				}

				/*Update the ExternalsLabel*/
				else if(labelAddres==-1){
					/*Mark as External*/
					Objects[IC].Machine.code	=	0;
					Objects[IC].Machine.code	=	(Objects[IC].Machine.code)	|	(0x1);
					/*Update Externals table*/
					Externals[externLabelCounter].decAddress	=	BASE_ADDRESS	+	IC;
					strncpy(Externals[externLabelCounter].name,currentCmd,strlen(currentCmd));
					externLabelCounter++;
				}
				else{
					printf("[line:\t%d]\tInvalid Addressing for %s Label operand.\n", lineNumber, currentCmd);
					return false;
				}
			}
		}
		else{
			printf("[line:\t%d]\tIllegal Direct Addressing for %s operand.\n", lineNumber, currentCmd);
			return false;
		}
		return true;
	}
	/*******************************************************************************************
	 *Step	1:	Checks if 'Immediate Addressing'?	Step	2	:	skip.
	 *Step	2:	Coding	'Immediate Addressing'	into	machine	code.
	 * *****************************************************************************************/
	if (isValidNumber(currentCmd,	&number)	){
		/*Operand is an valid number, Checks if the Addressing is legal*/
		if(IsLegalAddressingMethod(cmdIndex, opType, Immediate)){
			if(opType==Source){
				/*Update the Source	specific bits*/
				Objects[tempIC].Machine.code	=	(Objects[tempIC].Machine.code)	|(0x0<<4);
				IC++;
				updateObjects((BASE_ADDRESS+IC),	Source,	Immediate,	number,	IC);
			}
			else{
				/*Updating for Destination*/
				Objects[tempIC].Machine.code	=	(Objects[tempIC].Machine.code)	|(0x0<<2);
				IC++;
				updateObjects((BASE_ADDRESS+IC),	Destination,	Immediate,	number,	IC);
			}

			Objects[IC].Type	=	Absolute;
			Objects[IC].Machine.code	=	(Objects[IC].Machine.code)	|	(number<<2);
			Objects[IC].Machine.code	=	(Objects[IC].Machine.code)	&	(MACHINE_MASK);
			}
		else{
			printf("[line\t%d]\tIllegal Immediate Addressing for %s operand.\n", lineNumber, currentCmd);
			return false;
		}
		return true;
		}

	/*******************************************************************************************
	 *Step	1:	Checks if 'Previous	Addressing'?	Step	2	:	skip.
	 *Step	2:	Coding	'Previous	Addressing'	into	machine	code.
	 * *****************************************************************************************/
	if(isValidPrevious(currentCmd)){
		/*Operand is an previous Addressing, Checks if the Addressing is legal*/
		if(previousObjectLocation==0){
			/*ERROR	-	FIRST COMMAND CAN'T BE PREV*/
			printf("[line:\t%d]\tPrevious Addressing for %s operand can't be in first input.\n", lineNumber, currentCmd);
			return false;
		}
		/*Checks if legal Addressing*/
	if(IsLegalAddressingMethod(cmdIndex, opType, Previous)){

			if(opType==Source){/*Update the Source	specific bits*/
				IC++;
				Objects[IC].Machine.code	=	0;
				if(!PrevObjectCopy(IC,previousObjectLocation,opType)){
					printf("[line:\t%d]\tPrevious Addressing for %s operand has failed.\n", lineNumber, currentCmd);
					return false;
				}
				/*Updating the Source operand machine code addressing*/
				if(Objects[IC].ObjectType.Addressing==Immediate)
					Objects[tempIC].Machine.code	=	(Objects[tempIC].Machine.code)	|(0x0<<4);
				else if(Objects[IC].ObjectType.Addressing==Direct)
					Objects[tempIC].Machine.code	=	(Objects[tempIC].Machine.code)	|(0x1<<4);
				else if(Objects[IC].ObjectType.Addressing==Register)
					Objects[tempIC].Machine.code	=	(Objects[tempIC].Machine.code)	|(0x3<<4);

			}
			else{/*Updating for Destination*/
				/*Checks if the Source Operand is an Register*/
				if(prevAddresing!=Register){
					IC++;
					Objects[IC].Machine.code	=	0;
				}
				if(!PrevObjectCopy(IC,previousObjectLocation,opType)){
					printf("[line:\t%d]\tPrevious Addressing for %s operand has failed.\n", lineNumber, currentCmd);
					return false;
					}
				/*Updating the Source operand machine code addressing*/
				if(Objects[IC].ObjectType.Addressing==Immediate)
					Objects[tempIC].Machine.code	=	(Objects[tempIC].Machine.code)	|(0x0<<2);
				else if(Objects[IC].ObjectType.Addressing==Direct)
					Objects[tempIC].Machine.code	=	(Objects[tempIC].Machine.code)	|(0x1<<2);
				else if(Objects[IC].ObjectType.Addressing==Register)
					Objects[tempIC].Machine.code	=	(Objects[tempIC].Machine.code)	|(0x3<<2);
				}
			return true;
			}
		else{
			printf("[line:\t%d]\tIllegal Previous Addressing for %s operand.\n", lineNumber, currentCmd);
			return false;
			}
	}
	/*******************************************************************************************
	 *END OF FUNCTION, something went wrong!
	 * *****************************************************************************************/
	printf("[line:\t%d]\t Could not Analyze %s Operand.\n",lineNumber, currentCmd);
	return false;
}


/*parse, analyze,analyse*/
/*oneOperand:
 * Getting parameters of one Operand Addressing opcode command, encoded the operand and returns true.
 * Also checking if there are more then one Operand,  will ignore any other operand.
 * */
Boolean oneOperand(char *fixedLine, int lineNumber,	PassCycles pass,int cycles){
	/*setting a pointer to the current location*/
	char	*operands=(fixedLine+(currentArg-fixedLine)+strlen(currentArg)+1);
	char temp[INS_COMMAND_LEN];

	 /*Copy into 'temp' the current operand, and checks if found operand.*/
		strcpy(temp, operands);
		currentCmd	=	strtok(temp,	" ,\t\n");
		if(!currentCmd){
			printf("[line:\t%d]\tCould not get any Destination operand.\n", lineNumber);
			return false;
		}

		 /*Sending the operand to the decoder, Checks if it's fine.*/
		if(!ParseOperands(Destination, lineNumber, pass)){
			printf("[line:\t%d]\t Could not Analyze Destination Operand.\n",lineNumber);
			return false;
		}
		/*Checks if there are more then one Operand*/
		currentCmd	=	strtok(NULL,	" ,\t\n");
		while(currentCmd){
			printf("[line:\t%d]\t ERROR! Too many operands, Ignore '%s'.\n", lineNumber, currentCmd);
			currentCmd	=	strtok(NULL,	" ,\t\n");
		}
		/*If there are no more CodingCycles then should the prev pointer*/
		if(cycles==1)
			previousObjectLocation=tempIC;
		prevAddresing=Off;
	return true;
}
/*twoOperands:
 * Doing the same as 'oneOperand' function,	But in this case we're working with two operands.
 * */
Boolean twoOperands(char	*fixedLine,	int	lineNumber,	PassCycles Pass, int cycles){
	/*Setting pointer to the current location*/
	char	*operands	=	(fixedLine	+	(currentArg	-	fixedLine)	+strlen(currentArg)+1);
	char	temp[INS_COMMAND_LEN];
	/*Copy into 'temp' the current operand, and checks if found operand.*/
	strcpy(temp,operands);
	currentCmd	=	strtok(temp,	" ,\t");
		if(!currentCmd){
			printf("[line:\t%d]\tCould not get any Source operand.\n", lineNumber);
			return false;
		}

	/*Sending the operand to the decoder, Checks if it's fine.*/
		if(!ParseOperands(Source, lineNumber,Pass)){
			printf("[line:\t%d]\t Could not Analyze Source Operand.\n",lineNumber);
			prevAddresing	=	Off;
			return false;
		}
		/*Moving	to the next operand and sending to the decoder.*/
		/*Next operand*/
		currentCmd	=	strtok(NULL,	" ,\t\n");
		if(!currentCmd){
			printf("[line:\t%d]\tCould not get any Destination operand.\n", lineNumber);
			prevAddresing	=	Off;
			return false;
		}

		if(!ParseOperands(Destination, lineNumber, Pass)){
			printf("[line:\t%d]\t Could not Analyze Destination Operand.\n",lineNumber);
			prevAddresing	=	Off;
			return false;
		}
		/*Checks if there are more then Two Operands*/
		currentCmd	=	strtok(NULL,	" ,\t\n");
		while(currentCmd){
			printf("[line:\t%d]\t ERROR! Too many operands, Ignore '%s'.\n", lineNumber, currentCmd);
			currentCmd	=	strtok(NULL,	" ,\t\n");
		}
		/*If there are no more CodingCycles then should the prev pointer*/
		if(cycles==1)
			previousObjectLocation=tempIC;
		prevAddresing=Off;
	return true;
}
/*ParseData
 * Doing all Data works for '.String' and '.data'.*/
Boolean ParseData(ArgumentType	argType,	char	*fixedLine,	int lineNumber){
	int location,	i;
	char	*temp;
	unsigned int data;

	/*checking case if it's '.data'*/
	if(argType==InstructionData){
		currentArg	=	strtok(NULL,	" ,\t");
		if(!currentArg){
			printf("[line:\t%d]\t Could not find any '.data' entries.\n",lineNumber);
			return false;
		}/*Else*/
		/*save into memory*/
		while(currentArg){
			if(sscanf(currentArg,"%u",&data)){
				memory[DC].Machine.code	=	0;
				memory[DC].Machine.code	=	(memory[DC].Machine.code)	|	data;
				DC++;
			}
			else{/*Something went wrong, error output*/
				printf("[line:\t%d]\t Invalid parameter has been found for '%s'.\n",lineNumber,currentArg);
				return false;
			}
			currentArg	=	strtok(NULL,	" ,\t");
		}
	}
	/*checking case if it's '.String'*/
	else if(argType==InstructionString){
		currentArg	=	strtok(NULL, " \t\n");
		if(!currentArg){
			printf("[line:\t%d]\t No string has been found after '.string'.\n",lineNumber);
			return false;
		}
		/*Point to the string*/
		location	=	strlen(fixedLine)	-	strlen(currentArg)-1;
		/*Save string from the first '"' sign*/
		temp	=	strrchr(fixedLine,	'"');

		if(fixedLine[location]!='"' ||	!temp){
			printf("[line:\t%d]\t Invalid string pattern.\n",lineNumber);
			return false;
		}
		location++;
		for(i=location	;	fixedLine[i]!='"'; i++){
			memory[DC].Machine.code	=	0;
			memory[DC].Machine.code	=	memory[DC].Machine.code	|	fixedLine[i];
			DC++;
		}
		DC++;
	}
	/*General Case, something went wrong.*/
	else return false;

	return true;
}

/*Private PrevObjectCopy.
 * This function is taking care of Previous	Addressing case only.
 * It will copy the operand from 'fromIC' to 'toIC' and return.*/
Boolean PrevObjectCopy(int toIC, int fromIC, OperandType	Type){

	int prevIndex	=	0, prevOperandIndex, regValue;
	prevOperandIndex	=	fromIC;
	/*Checks if both PrevOperands are Registers*/
	if(Objects[fromIC].ObjectType.cmd){
		prevIndex++;
		prevOperandIndex++;
		if(prevIndex<Objects[fromIC].size){

			if(Objects[prevOperandIndex].ObjectType.Addressing	!=	Register){/*FIX THIS *****************************************************************/
				Objects[toIC]	=	Objects[prevOperandIndex];
				Objects[toIC].decAddress	=	BASE_ADDRESS	+	toIC;
				return true;
				}

			/*checks if prevAddresing is Register*/
			else if(Objects[prevOperandIndex].ObjectType.Addressing	==	Register){
				/*Get the value of the operand*/
				if(Objects[prevOperandIndex].ObjectType.Type	==	Source)
					regValue	=	Objects[prevOperandIndex].ObjectType.SourceValue;

				else if	(Objects[prevOperandIndex].ObjectType.Type	==	Destination)
					regValue	=	Objects[prevOperandIndex].ObjectType.DestinationValue;

				else if	(Objects[prevOperandIndex].ObjectType.Type	==	Both)
					regValue	=	Objects[prevOperandIndex].ObjectType.SourceValue;

				/*Keeps the regValue in current Operand*/
				if(Type==Source){
					prevAddresing=Register;
					updateObjects((BASE_ADDRESS+toIC),	Type,	Register,	regValue,	toIC);
					Objects[toIC].Machine.code	=	(Objects[toIC].Machine.code)	|	(regValue<<7);
				}
				else{
					updateObjects((BASE_ADDRESS+toIC),	Type,	Register,	regValue,	toIC);
					Objects[toIC].Machine.code	=	(Objects[toIC].Machine.code)	|	(regValue<<2);
				}

				return true;
			}
			/*Not Register!*/
			/*There is no need to check anything in other cases, just copy the Operand, which we already did.*/
			}
		}
	return false;
}

/*Private function updateObjects
 * This function will update any object with all the parameters it gets
 * */
void updateObjects(int decAddress, OperandType opType, AddressingMethod addressing, int value, int currentIC){
	Objects[currentIC].decAddress	=	decAddress;
	Objects[currentIC].ObjectType.Type	=	opType;
	Objects[currentIC].ObjectType.Addressing	=	addressing;

	if(opType	==	Source)
		Objects[currentIC].ObjectType.SourceValue	=	value;
	else	if(opType==Destination)
		Objects[currentIC].ObjectType.DestinationValue	=	value;
}

