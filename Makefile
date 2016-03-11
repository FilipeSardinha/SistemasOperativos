CAMPUS= TagusPark
CURSO= LEIC
GRUPO= 67
ALUNO1= 71035

main: commandlinereader.o par-shell-terminal.o list.o main.o 
	gcc -pthread -Wno-unused -o par-shell commandlinereader.o par-shell-terminal.o list.o main.o

parshellterminal: par-shell-terminal.o main.o list.o commandlinereader.o
	gcc -pthread -Wall -g -o par-shell-terminal par-shell-terminal.o main.o list.o commandlinereader.o

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc -Wall -g -c commandlinereader.c

par-shell-terminal.o: par-shell-terminal.c par-shell-terminal.h
	gcc -Wall -Wno-unused -g -c par-shell-terminal.c

list.o: list.c list.h
	gcc -Wall -g -c list.c

main.o: main.c commandlinereader.h par-shell-terminal.h
	gcc -Wall -pthread -Wno-unused -g -c main.c

clean:
	rm -f *.o par-shell

cleanzip: 
	rm -f *.tgz

wipe:
	rm -f *.o *.tgz par-shell

zip:
ifndef CAMPUS
	@echo "ERROR: Must setup macro 'CAMPUS' correcly."
else
ifndef CURSO
	@echo "ERROR: Must setup macro 'CURSO' correcly."
else
ifndef GRUPO
	@echo "ERROR: Must setup macro 'GRUPO' correcly."
else
	tar -czf project-$(CAMPUS)-$(CURSO)-$(GRUPO)-$(ALUNO1).tgz * 
endif
endif
endif
