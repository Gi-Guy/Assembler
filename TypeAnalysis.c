/*
 * Analyzing the input line and what are the Arguments values.
 *
 * */

#include <string.h>
#include <ctype.h>
#include "Enum.h"

#include "Settings.h"
#include "TypeAnalysis.h"


/*ArgumentType
 * Returns the input Instruction argument Type.
 * */
ArgumentType	getType(char *str,RegistersList *registers, commandsList *opcode, int lineNumber, int *cmdIndex){
	/*Checking if 'str' isn't Empty*/
	if(strlen(str)>0){
		/*Checking if 'str' is a Comment*/
		if(str[0]==';')
			return Comment;
		/*Checking if 'str' is an Instruction command*/
		else if(str[0]=='.'){
			if(!strcmp(str, ".string"))
				return InstructionString;
			else if (!strcmp(str, ".data"))
				return InstructionData;
			else	if	(!strcmp(str, ".entry"))
				return	InstructionEntry;
			else	if	(!strcmp(str, ".extern"))
				return	InstructionExtern;
		}
		/*Checking if 'str' is Label*/
		else if(isLabel(str,registers,opcode,lineNumber))
			return Label;
		/*Checking if input is an opcode command*/
		else if(isCommand(str,cmdIndex,opcode))
			return OpcodeCommand;

	}
	else{
		printf("[Line:\t%d]\t line is empty.\n",lineNumber);
		return Empty;
	}
	return Invalid;
}
/*Checks if 'str' is legal label, return true if it does, otherwise false.*/
Boolean isLabel(char *str, RegistersList *registers, commandsList *opcode, int lineNumber){
	/*Checking if Label size is right*/
		if(	strlen(str)>LABEL_LEN	){
			printf("[line\t%d]\tLabel strlen size must be under %d.\n",lineNumber,LABEL_LEN);
				return false;
		}
	/*Checking correct pattern*/
		if(str[strlen(str)-1]!=':'){
		/*	printf("ERROR\t-\tLabel\t%s\t pattern incorrect!\n", str);*/
				return false;
		}
		/*Sending to other function to check*/
	return isValidLabel(str, registers,opcode);
}

/*Checks if 'str' is  legal label*/
Boolean isValidLabel(char *str, RegistersList *registers, commandsList *opcode){
	int index;
	if(	isLabelCommand(str,opcode)	||	isLabelReg(str,registers)	)
		return false;
	/*checks if label start with with a letter*/
	if(	isalpha(str[0])	){
		for (index=1; index<strlen(str)-1;	index++){
			/*Checks if label str is valid*/
			if(	isalnum(str[index]	))
				continue;
			else{
				printf("ERROR:\t	Invalid\t	label\n");
				return false;
			}
		}
		return true;
	}
	return false;
}
/*isCommand:
 *Return true if 'str' parameter is opcode command, otherwise returns false.
 * */
Boolean isCommand(char *str, int *cmdIndex, commandsList *opcode){
	int index;
	int cycles	=	numberOfCycles(str);
	codingCycles	=	cycles;
	if(	cycles!=0	){
		for (index	=	0;	index	<	OPCODE_AMOUNT;	index++){
			if	(	strncmp(opcode[index].name, str	, strlen(opcode[index].name))==0){
				/*keeps separately the opcode command type*/
				*cmdIndex=opcode[index].index;
					return true;
			}
		}
	}
	return false;
}
/*numberOfCycles:
 * Returns the number of coding cycles, else returns None value.
 * */
int numberOfCycles(char *str){
	int length=strlen(str)-1; /*Point to end of String*/
		if(	str[length]=='1')
			return 1;
		else if(	str[length]=='2'	)
			return 2;
	return 0;
}
/*isLabelCommand:
 * Using only when comparing LABEL into opcode command.
 * Returns true if label is an opcode command, otherwise returns false.
 * */
Boolean isLabelCommand(char *str, commandsList *opcode){
	int index;
	for (index=0;	index	<	OPCODE_AMOUNT;	index++)
		if(strncmp(opcode[index].name,	str,	strlen(opcode[index].name))==0)
			return true;
	return false;
}
/*isLabelReg:
 * Using only when comaring LABEL into register title.
 * Returns true is label is an register title, otherwise returns false.
 * */
Boolean isLabelReg(char *str, RegistersList *registers){
	int index;
	for (index=0;	index<REGISTERS_AMOUNT;	index++)
		if(strncmp(registers[index].name, str, strlen(registers[index].name))==0)
			return true;
	return false;
}

/*isValidReg:
 * Return true if 'str' string has a match with some register from RegistersList.
 * parameters:	int*	-	keeps the match register index.
 * 					char*	-	string to compare.
 * 					RegisterList*	-	known register list.
 * */
Boolean isValidReg(int *regIndex,char *str, RegistersList *registers){
	int index;
	/*Checks if 'str' length isn't bigger than REGISTERS_LEN*/
	if(	!(strlen(str)	>	REGISTERS_LEN)	){
		for(index=0;	index	<	REGISTERS_AMOUNT;	index++)
			/*looking for any match*/
			if	(strncmp(registers[index].name,	str	,strlen(registers[index].name))==0){
				/*Keeps the register index*/
				*regIndex=registers[index].index;
				return true;
			}
	}
	return false;
}
/*Checks if the number we get is legal or not.*/
Boolean isValidNumber(char *str, int *number){
	int temp;
	/*the number pattern must be at least 2 digit longer*/
	if(strlen(str)<2)
		return false;
	if (str[0]=='#'){
		temp	=	sscanf(str, "#%d", number);
		if(!temp)
			return false;
		else
			return true;
	}
	return false;
}
/*Checks if operand is '$$' or not.*/
Boolean isValidPrevious(char *str){
	int i;
	/*strlen must be 2*/
	if(strlen(str)!=2)
		return false;
	for(i=0;i<2;i++)
		if(str[i]!='$')
			return false;
	return true;
}
/*IsLegalAddressingMethod:
 * Checks if command line is legal with the correct AddressingMethod in the correct OperandType.
 * parameters:	int*:	current opcode cmd to check with.
 * 					OperandType*:	current Source or Destination operand.
 * 					AddressingMethod*:	the current method	-	Immediate,	Direct,	Indexed,	Register.
 * return True if command is legal, otherwise returns false.
 * */
Boolean IsLegalAddressingMethod(int cmdIndex, OperandType opType, AddressingMethod method){


	/*Checking for: lea(6)	&	jsr(13)*/
	if(cmdIndex==6){
		if(opType==Source){
			if(method==Direct)
				return true;
			}
		/*lea allowed only for Direct and Register methods in case of Destination operand*/
		else if(method	==	Direct	||method	==	Register)
			return true;

	}
	/*'jsr' allowed only with Direct Destination operand*/
	else if(cmdIndex==13){
		if(opType==Destination	&&	method==Direct)
			return true;
	}
	/*----------------------------------------------------------------------------------------------------*/
	/*checking for:
	 * mov(0),	cmp(1),	add(2),	sub(3)*/
	else	if(cmdIndex==0	||	cmdIndex==1	||	cmdIndex==2	||	cmdIndex==3){
		/*If source operand then all methods allowed*/
		if(opType	==	Source)
			return true;
		/*not source, then:*/
		else{
			/*case 1:	if cmd is 'cmp' then all allowed*/
			if(cmdIndex	==	1)
				return true;
			/*case	2:	not 'cmp', but is	'mov',	'add' and	'sub'. if method 'Direct' or 'Register' return true*/
			else if(method	==	Direct	||method	==	Register)
				return true;
		}
	}
	/*----------------------------------------------------------------------------------------------------*/
	/*Checking for:
	 * not(4),	clr(5),	inc(7),	dec(8)*/
	else if(cmdIndex==4	||	cmdIndex==5	||	cmdIndex==7	||	cmdIndex==8){
		if(opType==Source){
			printf("ERROR:\tOperand	type	can't	be	Source!\n");
			return false;
		}
		else{
			if(method	==	Direct	||method	==	Register)
				return true;
		}
	}
	/*----------------------------------------------------------------------------------------------------*/
	/*Checking for:
	 * jmp(9),	bne(10),	red(11), prn(11)*/
	else if(cmdIndex==9	||	cmdIndex==10	||	cmdIndex==11	||	cmdIndex==12){
		if(opType==Source){
				printf("ERROR:\tOperand	type	can't	be	Source!\n");
				return false;
			}
		else{
			/*'prn' allowed with all methods*/
			if(cmdIndex==12)return true;
			/*Immediate is not allowed in this case*/
			else if(!(method==Immediate))
				return true;
		}
	}
	/*All other cases are illegal and returns false*/
	return false;
}
/*Return if the command is legal*/
Boolean commandAnalyze(int *cmdIndex,	char	*fixedLine,	int	linkNumber,	PassCycles pass,	Codingcycles	*cycles){
	if(*cmdIndex<0	||	*cmdIndex>OPCODE_AMOUNT	||	*cycles==None)
		return false;
	return false;
}
