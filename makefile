CC = gcc
CFLAGS = -Wall -g

SRC = main.c leitor.c instrucoes.c attributeDecoding.c constantPoolDecoding.c utils.c
OBJ = $(SRC:.c=.o)
EXEC = leitor

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lm


%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(EXEC)
