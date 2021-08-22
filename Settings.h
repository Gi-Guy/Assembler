/*
 * settings.h
 * Setting different rules for the main assembler.
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_
#include <stdio.h>

/*Hardware settings*/
	#define		MAX_MEMORY_SIZE	1000 /*Define the max size of the ram memory*/
	#define 	BINARY_WORD		12	/*Define binary word bit amount*/
	#define		BASE_ADDRESS	100	/*Define base address*/
	#define		OBJECTS_TYPE	2	/*Define the max len of Group object type, A,R,E*/
	#define		OUTPUT_BASE	4	/*Define the Output base for output functions*/
	#define 	MACHINE_MASK	0xffC/*Define an mask to reset the 2 first bit	-	the mask is	111111111100*/

/*Virtual software settings*/
	#define		OPCODE_LEN	5	/*Define the max length of opcode name*/
	#define		OPCODE_AMOUNT 16	/*Define the max amount of opcode commands*/
	#define		REGISTERS_LEN	2	/*Define registers digit length, r0-r7*/
	#define		REGISTERS_AMOUNT	8	/*Define the max amount of Registers*/
	#define		LABEL_LEN	30	/*Define max length of Label*/
	#define 	LABEL_AMOUNT	100 /*Define max amount of labels*/
	#define		INS_COMMAND_LEN	80	/*Define max length of Instruction Command*/
	#define		LINE_LENGTH	80	/*Define the max length of file input*/
	#define 	FILE_NAME	100	/*Define the max length for file's name*/
	#define 	SEEK	0/*FIX THIS!***************************************************************/
	#define 	INPUT_FILE	".as"

/*Label temporary identity*/
	#define 	CODE	"code"	/*Define for '.entry' and '.extern' codes*/
	#define 	DATA	"data"	/*Define for '.data' and '.string' codes*/


#endif /* SETTINGS_H_ */
