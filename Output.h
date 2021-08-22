
#ifndef OUTPUT_H_
#define OUTPUT_H_
#include "Settings.h"

extern	ObjectsList Objects[MAX_MEMORY_SIZE];
extern	MemoryData	memory[MAX_MEMORY_SIZE];
extern	int IC;
extern	int DC;
extern	LabelList	*Labels	;
extern	EntriesLabel	*Entries;
extern	ExternalsLabel	*Externals;
extern	int labelCounter;
extern	int externLabelCounter;
extern	int	entryLabelCounter;

void WriteManager(char	*);

#endif /* OUTPUT_H_ */
