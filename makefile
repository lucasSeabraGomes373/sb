
SANITIZE_FLAGS ?= -fsanitize=address -fno-omit-frame-pointer
CFLAGS ?= -Wall -g $(SANITIZE_FLAGS)
RM = rm -f 

SRC = main.c leitor.c instrucoes.c attributeDecoding.c constantPoolDecoding.c utils.c executorInstrucoes.c java_frontend.c
OBJ = $(SRC:.c=.o)
EXEC = leitor.exe 

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lm $(SANITIZE_FLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(OBJ) $(EXEC)