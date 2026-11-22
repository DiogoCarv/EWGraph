# Grafos Ponderados Não Direcionados em C++ (EWGraph)

Uma implementação de estruturas e algoritmos para análise de **Grafos Ponderados Não Direcionados (*Edge-Weighted Graphs*)** desenvolvida em **C++**.

O sistema realiza a leitura de grafos com pesos nas arestas a partir de arquivos de texto utilizando lista de adjacências, permitindo encontrar a Árvore Geradora Mínima (MST) pelos algoritmos de **Kruskal** e **Prim**, além de particionar os vértices em agrupamentos de espaçamento máximo através de **$k$-Clustering**.

---

## 📌 Funcionalidades

* **Representação e Estruturas Auxiliares:** Leitura de grafos ponderados via arquivo de texto por lista de adjacências, exportação para visualização no formato Graphviz (`DOT`) com rótulos de peso e implementações dedicadas de **Union-Find** (com compressão de caminho e união por rank) e **Fila de Prioridade Mínima Indexada (`IndexMinPQ`)**.
* **Árvore Geradora Mínima (MST) — Kruskal:** Seleção gulosa das arestas de menor peso em complexidade $O(E \log E)$, utilizando *Union-Find* para evitar a formação de ciclos.
* **Árvore Geradora Mínima (MST) — Prim (*Eager Version*):** Crescimento da árvore a partir dos vértices em complexidade $O(E \log V)$, atualizando dinamicamente os menores custos de conexão via `IndexMinPQ` (com suporte a grafos desconexos).
* **$k$-Clustering de Espaçamento Máximo:** Particionamento dos vértices em $k$ grupos distintos via variação do algoritmo de Kruskal, maximizando a menor distância (*spacing*) entre clusters diferentes (configurável via argumento no terminal).

---

## 🛠️ Tecnologias e Estrutura

* **Linguagem:** C++
* **Estrutura do Repositório:**

  ```text
  EWGraph/
  ├── EWGraph.h         # Definição de UnionFind, IndexMinPQ, Edge e EWGraph (lista de adjacências e E/S)
  ├── mainEWGraph.cpp   # Implementação da classe Process (Kruskal, Prim e k-Clustering) e rotina principal
  ├── README.md         # Documentação do projeto
  └── tinyEWG.txt       # Instância de grafo ponderado para validação e testes
  ```

### 📂 Arquivos de Teste Inclusos

* **`tinyEWG.txt`**: Grafo ponderado não direcionado clássico (*Tiny Edge-Weighted Graph*, 8 vértices e 16 arestas com pesos decimais entre `0.16` e `0.93`) da biblioteca de Sedgewick, utilizado para validar o peso total da MST (`1.81`) nos algoritmos de Kruskal e Prim e testar o particionamento em $k$ clusters.

---

## 🚀 Como Executar

### Pré-requisitos

* Compilador G++ / Clang / MSVC instalado.

### Compilação e Execução

1. **Clone o repositório:**

   ```bash
   git clone [https://github.com/DiogoCarv/EWGraph.git](https://github.com/DiogoCarv/EWGraph.git)
   cd EWGraph
   ```

2. **Compile o código-fonte:**

   ```bash
   g++ mainEWGraph.cpp -o ewgraph
   ```

3. **Execute o programa passando o arquivo de grafo (e, opcionalmente, o número $k$ de clusters, padrão `3`):**

   * **Linux/macOS:**

     ```bash
     ./ewgraph tinyEWG.txt
     ./ewgraph tinyEWG.txt 4
     ```

   * **Windows:**

     ```bash
     ewgraph.exe tinyEWG.txt
     ewgraph.exe tinyEWG.txt 4
     ```

---

## 👤 Autor

Desenvolvido por [Diogo Carvalho](https://github.com/DiogoCarv).
