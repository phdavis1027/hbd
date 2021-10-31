CFLAGS = -Wall -Werror -mmacosx-version-min=11.5
COMPILER = gcc

compile: 
	$(COMPILER) $(CLAGS) -o hbd hbd.c

hbd:
	./hbd
