CFLAGS = -Wall -Werror -lpthread
COMPILER = gcc

compile: 
	$(COMPILER) $(CLAGS) -o hbd hbd.c

hbd:
	./hbd
