Assembler: Assembler.o Parsers.o LabelWorks.o Output.o TypeAnalysis.o
	gcc -g -ansi -Wall -pedantic Assembler.o Parsers.o LabelWorks.o Output.o TypeAnalysis.o -o Assembler

Assembler.o: Assembler.c Settings.h Structures.h Enum.h
	gcc -c -ansi -Wall -pedantic Assembler.c -o  Assembler.o
Parsers.o: Parsers.c Parsers.h
	gcc -c -ansi -Wall -pedantic Parsers.c -o  Parsers.o
LabelWorks.o: LabelWorks.c LabelWorks.h
	gcc -c -ansi -Wall -pedantic LabelWorks.c -o LabelWorks.o
Output.o: Output.c Output.h
	gcc -c -ansi -Wall -pedantic Output.c -o Output.o
TypeAnalysis.o: TypeAnalysis.c TypeAnalysis.h
	gcc -c -ansi -Wall -pedantic TypeAnalysis.c -o TypeAnalysis.o
