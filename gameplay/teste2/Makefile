# --- Variáveis de Configuração ---

# O compilador que queremos usar
CC = gcc

# Nome do seu programa executável final
TARGET = jogo

# Flags (opções) para o compilador:
# -Wall -Wextra = Mostra todos os avisos (MUITO recomendado)
# -g = Inclui informações de debug (para usar com o gdb)
# -std=c99 = Usa o padrão C99
CFLAGS = -Wall -Wextra -g -std=c99

# Bibliotecas (libs) que precisamos linkar:
# -lSDL2 = A biblioteca principal da SDL2
# -lm = A biblioteca de matemática (para sqrtf, etc.)
LIBS = -lSDL2 -lm

# --- Arquivos do Projeto ---

# Lista de todos os seus arquivos-fonte (.c)
SRC = main.c init.c input.c update.c render.c

# Converte a lista de .c para uma lista de .o (arquivos-objeto)
# Ex: "main.c init.c" vira "main.o init.o"
OBJ = $(SRC:.c=.o)


# --- Regras (Targets) ---

# A primeira regra é a padrão (o que 'make' faz sozinho)
# "all" (todos) depende do nosso executável (TARGET)
all: $(TARGET)

# Regra para "linkar" o programa final:
# Para criar o $(TARGET), ele precisa de todos os $(OBJ)
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

# Regra "padrão" para compilar:
# Diz como transformar QUALQUER arquivo .c em um .o
# -c = "compile, mas não linke ainda"
# $< = (variável automática) o nome do arquivo .c (dependência)
# $@ = (variável automática) o nome do arquivo .o (o alvo)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra de "limpeza":
# Não cria nenhum arquivo, por isso é "PHONY"
clean:
	rm -f $(TARGET) $(OBJ)

# Declara quais regras não criam arquivos (são "falsas")
.PHONY: all clean
