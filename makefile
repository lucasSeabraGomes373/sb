CC = gcc
CFLAGS = -Wall -g
RM = rm -f 

SRC = main.c leitor.c instrucoes.c attributeDecoding.c constantPoolDecoding.c utils.c executorInstrucoes.c
OBJ = $(SRC:.c=.o)
EXEC = leitor.exe 

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(OBJ) $(EXEC)