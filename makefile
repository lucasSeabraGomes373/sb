
CFLAGS ?= -Wall -g
RM = rm -f 

SRC = main.c leitor.c inits.c instrucoes.c attributeDecoding.c constantPoolDecoding.c utils.c executorInstrucoes.c java_frontend.c
OBJ = $(SRC:.c=.o)
EXEC = leitor.exe 

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(OBJ) $(EXEC)

