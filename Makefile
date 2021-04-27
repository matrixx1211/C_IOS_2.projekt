#############################
#		Marek Bitomský		#
#		2. projekt IOS		#
#		 02. 05. 2021		#
#############################

COMPILER = gcc 
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic # -g # BCS of 
THREAD = -pthread

.PHONY = all clear

#############################
all: proj2
#############################


#############################
# PROJ2 - main program
#############################
proj2: proj2.c 
	$(COMPILER) $(CFLAGS) $^ -o $@ $(THREAD)
#santa.o elf.o reindeer.o


#############################
# SANTA
#############################
santa.o: santa.c santa.h 
	$(COMPILER) $(CFLAGS) -c santa.c 

#############################
# ELF
#############################
elf.o: elf.c elf.h 
	$(COMPILER) $(CFLAGS) -c elf.c 

#############################
# REINDEER
#############################
reindeer.o: reindeer.c reindeer.h 
	$(COMPILER) $(CFLAGS) -c reindeer.c 


#############################
# PACK - make zip 
#############################
pack: proj2.c
	zip proj2.zip proj2.c Makefile


#############################
# CLEAR - remove files
#############################
clear: 
	rm -f *.o *.zip proj2 santa elf reindeer