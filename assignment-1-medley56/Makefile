EXEC     = assgn1
CC       = gcc

#CFLAGS   = -Wall -Werror 
CFLAGS   = -Wall -Werror -DDEBUG

LDFLAGS  = 

SRC      = $(wildcard *.c)
OBJ      = $(SRC:.c=.o)

all: $(EXEC)

${EXEC}: $(OBJ)
		$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c %.h
		$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean

clean:
		@rm -rf assgn1 *.o
