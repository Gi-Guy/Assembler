/*This file do all labels works*/
#include <string.h>
#include <ctype.h>
#include "LabelWorks.h"
#include "Settings.h"

/*Build label in (*list) where (*labelCounter) point to.*/
void buildLabels(LabelList *list, int *labelCounter, char	*labelName,int decAddress, int value,	char	*type){
	strcpy(list[(*labelCounter)].name,labelName);
	list[(*labelCounter)].decAddress=decAddress;
	list[(*labelCounter)].type=type;
	list[(*labelCounter)].value=value;

	(*labelCounter)++;
}
/*Build External label in (*list) where (*counter) point to.*/
void buildExternalLabels(ExternalsLabel	*list,char	*labelName,	int	decAddress, int *counter){
	strcpy(list[(*counter)].name,labelName);
	list[(*counter)].decAddress=decAddress;
	(*counter)++;
}
/*Build Entries label in (*list) where (*counter) point to.*/
void	buildEntriesLabels(EntriesLabel	*list,	char	*labelName,	int	decAddress,	int	*counter){
	strcpy(list[(*counter)].name,labelName);
	list[(*counter)].decAddress=decAddress;
	(*counter)++;
}
/*Return if there is an label with 'str' name, otherwise return false.*/
Boolean labelIsExist(char *str, LabelList *list){
	int i;
	/*search 'str' in LabelList*/
	for (i=0;	i<labelCounter && strlen(list[i].name)>0;	i++)
		if(strncmp(str, list[i].name, strlen(list[i].name))==0)
			return true;
	return false;
}
/*Return the *str label address*/
int GetLabelAddress(char *str, LabelList *labelList, ExternalsLabel *externalsList){
	int i;
	/*search 'str' in LabelList*/
	for (i	=	0;	i<labelCounter	&&	strlen(labelList[i].name)>0;	i++){
		if (strncmp(str, labelList[i].name, strlen(labelList[i].name))==0)
			return (labelList[i].decAddress);
		}
		/*search 'str' int ExternalsLabel*/
		for (i	=	0;	i<labelCounter	&&	strlen(externalsList[i].name)>0;	i++){
			if (strncmp(str, externalsList[i].name, strlen(externalsList[i].name))==0)
				return -1;
		}

	return 0;
}
/*Update the entry labels after the first pass*/
void EntryUpdate(ArgumentType type, char *label){
	int tempAddress, i;
	/*Looking for the label in the list*/
	for(i=0; i<labelCounter; i++){
		if(strlen(Labels[i].name)>0	&&	strncmp(Labels[i].name, label,strlen(Labels[i].name))==0){
			/*Found the label, keep the address*/
			tempAddress	=	Labels[i].decAddress;
			break;
		}
	}
	/*Update the entry label list*/
	for(i=0; i<labelCounter;i++){
		/*checking if the label is the current label in list*/
		if(strlen(Entries[i].name)>0	&&	strncmp(Entries[i].name, label,strlen(Entries[i].name))==0){
			/*Found a match and updating the label*/
			Entries[i].decAddress=tempAddress;
			return;
		}
	}
}
/*Update all Data labels after the first pass*/
void UpdateDataLabels(){
	int i;
	for(i=0;	i<labelCounter	&& strlen(Labels[i].name)>0;	i++){
		/*Looking for DATA labels*/
		if(strncmp(Labels[i].type, DATA,strlen(DATA))==0){
			Labels[i].decAddress	=	BASE_ADDRESS	+	IC	+	Labels[i].value;
		}
	}
}

