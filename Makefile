#############################
#		Marek Bitomský		#
#		2. projekt IOS		#
#		 02. 05. 2021		#
#############################

COMPILER = gcc -g
#CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic

#############################
all: proj2
#############################


#############################
# PROJ2 - main program
#############################
proj2: proj2.c
	$(COMPILER) $(CFLAGS) $@.c -o $@

#############################
# PACK - make zip 
#############################
pack: proj2.c
	zip xbitom00.zip proj2.c Makefile


#############################
# CLEAR - remove files
#############################
clear: 
	rm *.o *.zip proj2 