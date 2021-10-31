CFLAGS = -Wall -Werror -mmacosx-version-min=11.5 -lpthread -g
COMPILER = gcc

compile: 
	$(COMPILER) $(CLAGS) -o hbd hbd.c

run:
	hbd start
