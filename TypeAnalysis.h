
#ifndef TYPEANALYSIS_H_
#define TYPEANALYSIS_H_
#include "LabelWorks.h"
extern char	*currentArg;
extern int codingCycles;
/*
ArgumentType	getType(char *str);
*/
ArgumentType	getType(char *,RegistersList *, commandsList *, int , int *);
Boolean isCommand(char *, int *, commandsList *);
ArgumentType inputType(char *);
int numberOfCycles(char *);
Boolean isLabelCommand(char *, commandsList *);
Boolean isLabelReg(char *, RegistersList *);
Boolean isValidLabel(char *, RegistersList *, commandsList *);
Boolean isLabel(char *, RegistersList *, commandsList *, int );
Boolean isValidReg(int *,char *, RegistersList *);
Boolean IsLegalAddressingMethod(int , OperandType , AddressingMethod );
Boolean isValidNumber(char *, int *);
Boolean isValidPrevious(char *);
#endif /* TYPEANALYSIS_H_ */
