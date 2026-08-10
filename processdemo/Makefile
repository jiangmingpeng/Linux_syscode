BIN=process
Cc=gcc
SRC=$(wildcard *.c)
OBJ=$(BIN:.c=.o)

$(BIN):$(SRC)
	$(Cc) -o $@ $^

%.o:%.c
	$(Cc) -c $^

.PHONY:
clean:
	rm -f $(BIN) $(OBJ)

