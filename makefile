all: ps
	
clean:
	rm *.o *~

ps: ps.c
	gcc ps.c -g -F dwarf -lm -o ps
