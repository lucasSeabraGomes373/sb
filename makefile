
CFLAGS ?= -Wall -g
RM = rm -f 

SRC = main.c leitor.c inits.c instrucoes.c attributeDecoding.c constantPoolDecoding.c utils.c executorInstrucoes.c java_frontend.c
OBJ = $(SRC:.c=.o)
EXEC = leitor.exe 

all: $(EXEC)

# Target para cross-compile para Windows (MinGW-w64)
# Uso: `make mingw` — requer `x86_64-w64-mingw32-gcc` instalado no sistema.
# Não altera o comportamento padrão (build Linux permanece como antes).
MINGW_CC ?= x86_64-w64-mingw32-gcc
MINGW_FLAGS ?= -static

mingw: $(SRC)
	$(MINGW_CC) $(MINGW_FLAGS) -O2 -o $(EXEC) $(SRC) -lm

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(OBJ) $(EXEC)


# CC := gcc
# CFLAGS := -g -O1 -fno-omit-frame-pointer -fsanitize=address
# LDFLAGS := -fsanitize=address
#	$(CC) $(CFLAGS) -o $(EXEC) $(SRC) $(LDFLAGS) -lm

