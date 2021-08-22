

#ifndef LABELWORKS_H_
#define LABELWORKS_H_
#include "Settings.h"
#include "Structures.h"
#include "Enum.h"

extern int	labelCounter;
extern int externLabelCounter;
extern	LabelList	*Labels	;
extern	EntriesLabel	*Entries;
extern	ExternalsLabel	*Externals;
extern int IC;

void buildLabels(LabelList *, int *, char	*,int , int ,	char	*);
void buildExternalLabels(ExternalsLabel	*,char	*,	int	, int *);
void	buildEntriesLabels(EntriesLabel	*,	char	*,	int	,	int	*);
int GetLabelAddress(char *, LabelList *, ExternalsLabel *);
void EntryUpdate(ArgumentType , char *);
Boolean labelIsExist(char *, LabelList *);
void UpdateDataLabels();


#endif /* LABELWORKS_H_ */
