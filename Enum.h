/*
 * enumSettings.h
 *Set up different default enum structures.
 */

#ifndef ENUM_H_
#define ENUM_H_
#include "Settings.h"

/*Define boolean enum*/
	typedef	enum{
		false,	/*false	==	0	||	'off'*/
		true		/*true	==	1	||	'on'*/
	}Boolean;

/*Define addressing methods values 0-3*/
	typedef	enum{
		Immediate,
		Direct,
		Previous,
		Register,
		Off
	}AddressingMethod;

/*Define commands/input type*/
	typedef	enum{
		Label,	/*0*/
		Empty,	/*1*/
		Comment,	/*2*/
		InstructionData,	/*3*/
		InstructionString,	/*4*/
		InstructionEntry,	/*5*/
		InstructionExtern,	/*6*/
		OpcodeCommand,	/*7*/
		Invalid	/*8*/
	}ArgumentType;

/*Define coding cycles for commands*/
	typedef enum{
		None,
		Once = 1,
		Twice = 2
	}Codingcycles;

	/*Define the coding type of the command*/
	typedef enum{
		Absolute,/*A*/
		Relocatable,/*R*/
		External,	/*E*/
		NoValue
	}CodingType;
	/*Define pass cycles for parsing*/
	typedef	enum{
		First,
		Second
	}PassCycles;
	/*Define operandTypes*/
	typedef enum {
		Source,
		Destination,
		Both
	}OperandType;
#endif /* ENUM_H_ */
