/*
 * structures.h
 *Set up different structures
 */

#ifndef STRUCTURES_H_
#define STRUCTURES_H_
#include "Settings.h"
#include "Enum.h"


/*Define registers struct type*/
	typedef struct	_registers{
		int index;	/*0-7*/
		char name[REGISTERS_LEN];
	}RegistersList;

/*Define opcode commands*/
	typedef struct _cmd{
		char name[OPCODE_LEN];
		int index;
		int group;
	}commandsList;

/*Define all the different Labels types */
	/*main labels pattern*/
	typedef	struct	_label{
		char name[LABEL_LEN];	/*Name of label*/
		int value;	/*label value*/
		char	*type;	/*Argument type*/
		int decAddress;	/*keeps the decimal address*/
	}LabelList;

	/*entries labels*/
	typedef	struct	_entry{
		char name[LABEL_LEN];	/*Name of label*/
		int decAddress;	/*keeps the decimal address*/
	}EntriesLabel;

	/*externals labels*/
	typedef	struct	_extern{
		char name[LABEL_LEN];	/*Name of label*/
		int decAddress;	/*keeps the decimal address*/
	}ExternalsLabel;

	/*Define machine code pattern*/
	typedef	struct	_machineCode{
		unsigned int code	:	12;	/*define as use of 12bit only*/
	}MachineCode;
	/*Define memory Data*/
	typedef	struct	_memoryData{
		MachineCode	Machine;
	}MemoryData;

	/*Define Object Type*/
	/*Keeps in memory the Objects type and it data information*/
	typedef	struct	_objectType{
		Boolean cmd;
		OperandType	Type;	/*Source Or Destination*/
		AddressingMethod	Addressing;
		int SourceValue;
		int DestinationValue;
	}ObjectsType;

	/*Define ObjectsList*/
	typedef	struct	_objects{
		int decAddress;	/*Keeps the current dec address*/
		int size;	/*Keeps how much memory current command needs*/
		CodingType Type;	/*Keeps the coding type of the command	(A,R,E)	or None if there is no such type*/
		MachineCode	Machine;	/*Keeps the 12bit of the current machine code*/
		ObjectsType	ObjectType;
	}ObjectsList;


#endif /* STRUCTURES_H_ */
