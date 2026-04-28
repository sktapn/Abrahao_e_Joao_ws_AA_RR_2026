# 📦 Tabela Hash — Análise de Algoritmos

> Seminário da disciplina **Análise de Algoritmos** — Universidade Federal de Roraima (UFRR)  
> Prof. Dr. Herbert Oliveira Rocha

---

## 📋 Sobre o Projeto

Implementação e análise completa de uma **Tabela Hash com Encadeamento Separado (Chaining)** em linguagem C, desenvolvida como trabalho avaliativo da disciplina de Análise de Algoritmos.

O trabalho cobre:
- Função de custo e complexidade assintótica
- Implementação do algoritmo em C
- Experimentação com diferentes tamanhos de entrada
- Análise gráfica do comportamento assintótico

---

## 🗂️ Estrutura do Repositório

```
📁 repositorio/
├── 📄 tabela_hash.c          # Código principal da Tabela Hash
├── 📄 codigo_hash.c          # Código do experimento (coleta de métricas)
├── 📊 resultados_hash_crescente.csv
├── 📊 resultados_hash_decrescente.csv
├── 📊 resultados_hash_aleatorio.csv
├── 📑 slides.pptx            # Apresentação do seminário
└── 📄 README.md
```

---

## ⚙️ Como Compilar e Executar

### Código principal
```bash
gcc tabela_hash.c -o hash
./hash
```

### Código do experimento
```bash
gcc codigo_hash.c -o codigo_hash

# Entrada Crescente
./codigo_hash 1

# Entrada Decrescente
./codigo_hash 2

# Entrada Aleatória (seed obrigatória)
./codigo_hash 3 42
```

---

## 🧠 Como Funciona

### Estrutura de Dados

```c
typedef struct No {
    int chave;
    struct No* proximo;
} No;

typedef struct {
    No** buckets;
} TabelaHash;
```

Cada posição da tabela (`bucket`) armazena uma **lista encadeada**. Quando duas chaves geram o mesmo índice (colisão), o novo elemento é adicionado no início da lista daquele bucket.

### Função Hash

```c
h(k) = k mod m
```

Onde `k` é a chave e `m` é o tamanho da tabela. Custo **O(1)**.

---

## 📊 Complexidade

| Operação | Melhor Caso | Caso Médio | Pior Caso |
|---|---|---|---|
| Inserção | O(1) | O(1) | O(n) |
| Busca | O(1) | O(1 + α) | O(n) |
| Remoção | O(1) | O(1 + α) | O(n) |
| Espaço | — | — | O(n + m) |

> **α = n/m** (fator de carga) — manter α ≤ 0,75 garante O(1) esperado em todas as operações.
>
> O pior caso O(n) ocorre quando todas as chaves colidem no mesmo bucket, degenerando a tabela em uma lista ligada.

---

## 🧪 Experimento

O experimento testou o algoritmo com **3 tipos de entrada** e **5 tamanhos diferentes**:

### Tipos de Entrada
| Tipo | Descrição |
|---|---|
| Crescente | Chaves inseridas de 1 até n |
| Decrescente | Chaves inseridas de n até 1 |
| Aleatório | Chaves geradas com `rand()` + seed fixa |

### Tamanhos Testados
`10³` · `10⁴` · `10⁵` · `10⁶` · `10⁷`

Cada teste foi repetido **13 vezes** e os gráficos foram gerados a partir da **média** dos resultados.

### Métricas Coletadas
- ⏱️ Tempo de execução (segundos)
- 🔁 Número de comparações
- 💾 Consumo de memória (KB)

### Resultados Observados
- **Crescente e Decrescente** → comportamento próximo de **O(1)** — zero colisões
- **Aleatório** → crescimento **linear O(n)** — colisões causadas por valores repetidos do `rand()`
- **Memória** → crescimento linear **O(n + m)** para todos os tipos, como esperado

> Testes realizados em máquina virtual Linux Mint (Debian) 64-bit — 8192 MB RAM — 8 núcleos 2.10 GHz

---

## 🔧 Funções Implementadas

| Função | Descrição | Complexidade |
|---|---|---|
| `funcaoHash()` | Calcula o índice via `k mod m` | O(1) |
| `inicializar()` | Zera todos os buckets | O(m) |
| `inserir()` | Insere com verificação de duplicata | O(1) esperado |
| `buscar()` | Retorna o nó ou NULL | O(1) esperado |
| `remover()` | Remove e libera memória | O(1) esperado |
| `exibir()` | Mostra o estado atual da tabela | O(n + m) |
| `liberar()` | Libera toda a memória alocada | O(n + m) |
| `experimento()` | Mede tempo por tamanho de entrada | — |

---

## 📚 Referências

- CORMEN, T. H. et al. **Introdução a Algoritmos**. 3. ed. MIT Press, 2009. Cap. 11 — Hash Tables.
- SEDGEWICK, R.; WAYNE, K. **Algorithms**. 4. ed. Addison-Wesley, 2011.

---
