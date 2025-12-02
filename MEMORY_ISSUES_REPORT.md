# 🔴 Relatório de Problemas de Memória

## Resumo
- **Arquivos afetados**: `double_aritmetica.class`, `interface_test.class`
- **Sintoma**: `munmap_chunk(): invalid pointer` durante cleanup
- **Causa raiz**: Buffer overflow na leitura de constant pool

---

## Problema 1: Alocação Insuficiente para CONSTANT_Long/Double

### Localização
`leitor.c`, linha 82:
```c
cp_info * readConstantPool = (cp_info *) malloc((constant_pool_count-1)*sizeof(cp_info));
```

### O Problema
- A JVM constant pool começa no índice 1 (índice 0 é reservado)
- Por isso, aloca `constant_pool_count - 1` slots
- **MAS**: CONSTANT_Long e CONSTANT_Double ocupam 2 slots cada
- Quando o código encontra Long/Double, faz `aux++` para pular o segundo slot
- Isso pode fazer o ponteiro `aux` acessar memória fora dos limites alocados

### Exemplo Prático
```
Constant Pool Count: 15
Aloca: 14 slots (índices 1-14)

Durante leitura:
  idx 1: CONSTANT_Utf8 → aux++
  idx 2: CONSTANT_Utf8 → aux++
  ...
  idx 8: CONSTANT_Double → aux++ (pula para idx 9)
                         → aux++ (novamente no loop!)
  idx 9: LEITURA FORA DO LIMITE ALOCADO!
```

---

## Problema 2: Limpeza Incompleta

### Localização
`leitor.c`, linhas 1102-1108:
```c
void freeConstantPool(cp_info *cp, byte2 count) {
    if (!cp) return;
    for (int i = 0; i < count - 1; i++) {  // ← DEVE SER count, não count-1
        if (cp[i].tag == CONSTANT_Utf8 ; cp[i].UnionCP.CONSTANT_UTF8.bytes) {
            free(cp[i].UnionCP.CONSTANT_UTF8.bytes);
        }
    }
    free(cp);
}
```

### O Problema
- Itera até `count - 1`, deixando o último UTF8 sem ser liberado
- Se o último slot é um UTF8 com memória alocada, há vazamento
- Mais importante: se houver Long/Double, os slots podem estar corrompidos

---

## Por Que double_aritmetica.class Crasha?

Provável sequência:
1. Arquivo tem CONSTANT_Double na constant pool
2. Durante leitura, `aux++` salta slots além do limite alocado
3. Memória heap é corrompida
4. Programa executa normalmente (sem usar essa área corrupta)
5. Na saída, `freeConstantPool` tenta liberar memória corrompida
6. glibc detecta corrupção e chama `munmap_chunk(): invalid pointer`

---

## Solução Proposta

### Fix 1: Aumentar alocação
```c
// Antes:
cp_info * readConstantPool = (cp_info *) malloc((constant_pool_count-1)*sizeof(cp_info));

// Depois - alocar com margem segura:
cp_info * readConstantPool = (cp_info *) malloc((constant_pool_count+5)*sizeof(cp_info));
```

### Fix 2: Corrigir freeConstantPool
```c
void freeConstantPool(cp_info *cp, byte2 count) {
    if (!cp) return;
    for (int i = 0; i < count; i++) {  // Mude count-1 para count
        if (cp[i].tag == CONSTANT_Utf8 ; cp[i].UnionCP.CONSTANT_UTF8.bytes) {
            free(cp[i].UnionCP.CONSTANT_UTF8.bytes);
        }
    }
    free(cp);
}
```

### Fix 3: Rastrear índices com precisão
Seria ideal manter um contador separado de quantos slots foram realmente alocados, considerando Long/Double.

---

## Impacto

- **Risco**: Médio (apenas 2 arquivos afetados de 20)
- **Severidade**: Alta (corrupção de memória)
- **Frequência**: Determinística para arquivos com CONSTANT_Double
- **Scope**: Limpeza de memória apenas (não afeta execução)

