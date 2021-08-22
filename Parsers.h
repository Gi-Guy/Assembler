
#ifndef PARSERS_H_
#define PARSERS_H_
#include "Settings.h"

extern	ObjectsList Objects[MAX_MEMORY_SIZE];
extern	MemoryData	memory[MAX_MEMORY_SIZE];
extern	int IC;
extern	int DC;

extern	LabelList	*Labels	;
extern	EntriesLabel	*Entries;
extern	ExternalsLabel	*Externals;
extern	RegistersList registers[REGISTERS_AMOUNT];
extern	commandsList opcode[OPCODE_AMOUNT];

extern	int correctIndex;
extern	int cmdIndex;
extern	int regIndex;
extern	int codingCycles;
extern	char fixedLine[INS_COMMAND_LEN];
extern	char	*currentArg;
extern	char *currentCmd;
extern	int	previousObjectLocation;


Boolean ParaseCommands(int , char *, int ,PassCycles );
Boolean ParseOperands	(OperandType	, int , PassCycles );
Boolean oneOperand(char *, int ,	PassCycles ,int );
Boolean twoOperands(char	*,	int	,	PassCycles , int );
Boolean ParseData(ArgumentType	,	char	*,	int );
#endif /* PARSERS_H_ */
