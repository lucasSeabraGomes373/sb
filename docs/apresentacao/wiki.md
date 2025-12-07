# JVM - Grupo 9

> Implementação de uma Máquina Virtual Java (JVM) desenvolvida em linguagem C.

## 🏛️ Instituição

**Universidade de Brasília (UnB)** **Departamento:** Ciência da Computação (CiC)  
**Professor:** Marcelo Ladeira  
**Disciplina:** Software Básico

---

## 👥 Integrantes do Grupo

| Nome | Matrícula |
| :--- | :--- |
| **LUCAS SEABRA GOMES OLIVEIRA** | 242041334 |
| **Henrique Valente Lima** | 211055380 |
| **Marcelo Marques Rodrigues** | 221018960 |
| **Rychard Ryan Alves de Moraes** | 190095229 |
| **Fernando Nunes de Freitas** | 222014661 |

## 1. Objetivos do Projeto

Este projeto implementa uma **Máquina Virtual Java (JVM) simplificada** em linguagem C, com foco **didático**.  
Os principais objetivos são:

- Compreender na prática como uma JVM interpreta **bytecode**.
- Visualizar o funcionamento de uma **máquina baseada em pilha**.
- Estudar a estrutura de **classfiles** e sua representação em C.
- Servir como base para extensões (mais instruções, objetos, GC etc.).

---

## 2. Visão Geral da Arquitetura

A JVM educacional é organizada em três grandes partes:

- **Leitura e decodificação do Classfile**
- **Estruturas de dados e modelo de execução (frames, pilha, memória)**
- **Interpretador de bytecode e loop de execução**

O fluxo geral é:

1. Ler o arquivo `.class` e convertê-lo para estruturas internas em C.
2. Inicializar as estruturas da JVM (pilha de frames, área de métodos, etc.).
3. Localizar o método `main`.
4. Criar o frame inicial e iniciar o **interpretador de bytecode**.
5. Executar instrução a instrução até o término do programa.

---

## 3. Leitura e Representação do Classfile

A leitura de arquivos `.class` e sua representação interna são tratadas por um conjunto de arquivos dedicados.

### 3.1. Arquivos principais de leitura

- `leitor.c`  
  - Funções para abrir o arquivo `.class`, ler bytes e preencher as estruturas definidas em `formatoClassFile.h`.

- `formatoClassFile.h`  
  - Define as estruturas que representam um classfile em C:
    - cabeçalho (magic, versão),
    - constant pool,
    - tabela de métodos,
    - atributos, etc.

- `constantPoolDecoding.c`  
  - Implementa a decodificação dos diferentes tipos de entradas da **constant pool**.
  - Converte os bytes lidos em estruturas mais fáceis de usar pelo restante da JVM.

- `attributeDecoding.c`  
  - Trata da leitura e interpretação dos **atributos** do classfile, em especial o atributo `Code`, que contém:
    - tamanho da pilha,
    - número de variáveis locais,
    - vetor de bytecode,
    - tabela de exceções (se suportada),
    - outros atributos relevantes.

- `attributeStructs.h`  
  - Estruturas em C que representam os vários tipos de atributos (por exemplo, atributo `Code`).

- `byteTypes.h`  
  - Tipos auxiliares (por exemplo, tipos de dados para bytes, u1, u2, u4, etc.).

- `accessFlags.h`  
  - Define flags de acesso (public, static, etc.) usadas em classes e métodos.

### 3.2. Estrutura do Classfile Interno

O classfile interno inclui:

- **Informações gerais**: magic, versão.
- **Constant Pool**: vetor de entradas tipadas (literais, nomes, referências).
- **Tabela de métodos**: cada método com:
  - nome,
  - descritor,
  - flags de acesso,
  - atributo `Code` com o bytecode e metadados de execução.

Essas estruturas são utilizadas posteriormente pelo módulo de execução para:

- encontrar o método `main`,
- acessar o código de bytecode,
- saber o tamanho da pilha e das variáveis locais de cada método.

---

## 4. Estruturas de Execução: Frames, Pilha e Estruturas Básicas

O modelo de execução é baseado em **frames de ativação** organizados em uma **pilha de execução**.

### 4.1. Arquivos principais de estruturas

- `frames.h`  
  - Define o que é um **frame de execução**, incluindo:
    - pilha de operandos (operand stack),
    - vetor de variáveis locais,
    - referência ao método associado,
    - contador de programa (PC) para aquele método, se armazenado por frame.

- `estruturasBasica.h`  
  - Estruturas auxiliares usadas em mais de um módulo:
    - pilha de frames,
    - representações gerais de objetos/valores (se existirem),
    - tipos básicos usados na execução.

- `inits.c` / `inits.h`  
  - Funções de **inicialização** da JVM:
    - criação das estruturas globais necessárias,
    - inicialização da pilha de frames,
    - preparação do primeiro frame para o método `main`.

### 4.2. Frames e Pilha de Execução

Em execução, a JVM mantém:

- uma **pilha de frames**, onde:
  - cada chamada de método cria um novo frame,
  - cada retorno de método remove o frame do topo;
- em cada frame:
  - **operand stack**: onde as instruções empilham e desempilham valores;
  - **local variables**: onde ficam parâmetros e variáveis locais.

---

## 5. Interpretador de Bytecode e Instruções

A lógica que interpreta e executa os bytecodes está em um conjunto de arquivos específicos.

### 5.1. Arquivos principais de execução

- `executorInstrucoes.c` / `executorInstrucoes.h`  
  - Contêm o **laço principal de execução** (loop da JVM).
  - Buscam, decodificam e executam cada instrução.
  - Atualizam a pilha de operandos, variáveis locais e PC.

- `instrcoes.c`  
  - Implementa as funções associadas a cada **opcode** ou grupo de opcodes:
    - aritmética,
    - carregamento/armazenamento,
    - saltos condicionais,
    - chamadas de método, etc.

- `catalogoCodigosInstrucoes.h`  
  - Define os **códigos numéricos (opcodes)** de cada instrução suportada.
  - Ajuda na organização das instruções em `instrcoes.c` e na decodificação em `executorInstrucoes.c`.

### 5.2. Ciclo de Execução

O ciclo de execução típico dentro de `executorInstrucoes.c` é:

1. Ler o `PC` atual do frame.
2. Buscar o opcode correspondente no vetor de bytecode.
3. Consultar `catalogoCodigosInstrucoes.h` para identificar a instrução.
4. Chamar a função apropriada em `instrcoes.c`.
5. Atualizar:
   - pilha de operandos,
   - variáveis locais,
   - contador de programa (PC) – com incremento normal ou salto.
6. Repetir enquanto houver instruções a executar.

As instruções usam as estruturas de `frames.h` / `estruturasBasica.h` para manipular os valores e o estado da JVM.

---

## 6. Interface, Utilidades e Entrada do Programa

### 6.1. Arquivos principais

- `main.c`  
  - Ponto de entrada da JVM.
  - Lê argumentos da linha de comando (por exemplo, o nome do arquivo `.class`).
  - Chama rotinas de:
    - leitura do classfile (`leitor.c`),
    - inicialização (`inits.c`),
    - início da execução (`executorInstrucoes.c`).

- `java_frontend.c`  
  - Camada de integração entre a JVM e o “mundo Java”.
  - Pode tratar:
    - mapeamento de métodos especiais (por exemplo, `System.out.println`),
    - lógica para resolver nomes de métodos/classe,
    - interface para execução de exemplos.

- `utils.c`  
  - Funções auxiliares (tratamento de erros, logs, conversões, debug, etc.).

- `makefile`  
  - Automatiza a compilação de todos os módulos do projeto.
  - Define regras para:
    - compilar cada `.c` em `.o`,
    - gerar o executável final da JVM.

### 6.2. Exemplos e Testes

- Diretório `exemplos/`  
  - Contém arquivos `.class` usados para testar a JVM (programas Java compilados).

- Diretório `java_src/`  
  - Contém os códigos-fonte Java (`.java`) que geram os `.class` em `exemplos/`.

---

## 7. Conjunto de Bytecodes Suportados

A JVM educacional suporta um **subconjunto** das instruções de bytecode Java, suficiente para rodar programas simples. Exemplos típicos (ajuste para o seu caso real):

- **Carregamento e constantes**
  - `iconst_m1`, `iconst_0` … `iconst_5`
  - `bipush` (carrega constante byte)
  - `iload`, `iload_0`, `iload_1`, etc.

- **Armazenamento**
  - `istore`, `istore_0`, `istore_1`, etc.

- **Operações Aritméticas**
  - `iadd`, `isub`, `imul`, `idiv`
  - Possivelmente `irem` (resto), se implementado.

- **Controle de Fluxo**
  - `goto`
  - Condicionais como `ifeq`, `ifne`, `iflt`, `ifge`, `ifgt`, `ifle` (ou subconjunto).

- **Chamadas e Retornos**
  - `invokestatic` (chamada de método estático).
  - `ireturn`, `return` (retorno de métodos).

A tabela de opcodes correspondente é definida em `catalogoCodigosInstrucoes.h` e implementada em `instrcoes.c`.

---

## 8. Como Compilar e Executar

### 8.1. Compilação

Com o `makefile` configurado, a compilação costuma ser feita com:

make
Isso gera o executável da JVM, por exemplo:
./jvm

(ajuste o nome do binário conforme definido no seu `makefile`).

### 8.2. Execução de um Programa Java

1. **Compilar o programa Java** (na pasta `java_src/`):

javac Teste.java


O arquivo `.class` resultante pode ser movido para `exemplos/` ou usado diretamente.

2. **Executar o `.class` usando a JVM educacional**:

./jvm exemplos/Teste.class ou ./jvm java_src/Teste.class

---

## 9. Limitações e Possíveis Extensões

### 9.1. Limitações Atuais

Entre as limitações típicas deste tipo de JVM educacional:

- Subconjunto de **bytecodes** implementado.
- Foco principal em tipos primitivos (frequentemente `int`).
- Suporte parcial ou inexistente a:
- objetos completos (`new`, campos, métodos de instância),
- herança, interfaces,
- exceções,
- threads.
- **Garbage collector** ausente ou simples.
- Trata algumas funcionalidades da biblioteca padrão de forma especial ou não as suporta totalmente.

### 9.2. Trabalhos Futuros

Extensões possíveis incluem:

- Adicionar novas instruções de bytecode e tipos de dados.
- Implementar suporte a **objetos**, campos e métodos de instância.
- Introduzir um **garbage collector** mais elaborado.
- Melhorar mensagens de erro e ferramentas de depuração (modo verbose de execução).
- Implementar um **desmontador (disassembler)** de bytecode para facilitar inspeção de `.class`.

---

## 10. Conclusão

Esta JVM educacional em C reúne, em um único projeto, os elementos essenciais de uma máquina virtual:

- leitura e decodificação de **classfiles**,
- representação interna de classes e métodos,
- **frames de execução** empilhados,
- **interpretador de bytecode** baseado em pilha.

Com isso, ela serve tanto como ferramenta de estudo de **arquitetura de VMs e execução de bytecode**, quanto como base para experimentos e extensões que aproximam o modelo de uma JVM completa, sem perder a clareza e simplicidade necessárias para fins didáticos.


