# Grafos Ponderados Não Direcionados em C++ (EWGraph)

Uma implementação de estruturas e algoritmos para análise de **Grafos Ponderados Não Direcionados (*Edge-Weighted Graphs*)** desenvolvida em **C++**.

O sistema realiza a leitura de grafos com pesos nas arestas a partir de arquivos de texto utilizando lista de adjacências, permitindo encontrar a Árvore Geradora Mínima (MST) pelos algoritmos de **Kruskal** e **Prim**, particionar vértices em comunidades por **$k$-Clustering**, identificar **Pontos de Articulação** e calcular o **Caminho Mínimo em número de saltos (BFS)**.

---

## 📌 Funcionalidades

* **Representação e Estruturas Auxiliares:** Leitura de grafos ponderados via arquivo de texto por lista de adjacências, exportação para visualização no formato Graphviz (`DOT`) com rótulos de peso e implementações dedicadas de **Union-Find** (com compressão de caminho e união por rank) e **Fila de Prioridade Mínima Indexada (`IndexMinPQ`)**.
* **Árvore Geradora Mínima (MST) — Kruskal:** Seleção gulosa das arestas de menor peso em complexidade $O(E \log E)$, utilizando *Union-Find* para evitar a formação de ciclos.
* **Árvore Geradora Mínima (MST) — Prim (*Eager Version*):** Crescimento da árvore a partir dos vértices em complexidade $O(E \log V)$, atualizando dinamicamente os menores custos de conexão via `IndexMinPQ` (com suporte a grafos desconexos).
* **$k$-Clustering de Espaçamento Máximo:** Particionamento dos vértices em $k$ grupos distintos (comunidades) via variação do algoritmo de Kruskal, maximizando a menor distância (*spacing*) entre clusters diferentes.
* **Pontos de Articulação (Algoritmo de Tarjan):** Identificação via DFS de vértices de corte (*gatekeepers*) cuja remoção desconecta o grafo em duas ou mais componentes isoladas em tempo linear $O(V + E)$.
* **Caminho Mínimo em Vínculos (BFS):** Cálculo da menor cadeia de conexões (graus de separação) entre um vértice de origem e o vértice destino `0` utilizando Busca em Largura em $O(V + E)$.

---

## 🛠️ Tecnologias e Estrutura

* **Linguagem:** C++
* **Estrutura do Repositório:**

  ```text
  EWGraph/
  ├── EWGraph.h         # Definição de UnionFind, IndexMinPQ, Edge e EWGraph (lista de adjacências e E/S)
  ├── mainEWGraph.cpp   # Implementação da classe Process (Kruskal, Prim, k-Clustering, Articulação e BFS)
  ├── README.md         # Documentação do projeto
  └── *.txt             # Instâncias de grafos ponderados para validação e testes
  ```

### 📂 Arquivos de Teste Inclusos

* **`redes_social.txt`**: Grafo ponderado de 12 vértices e 16 arestas simulando uma rede de interações entre usuários (onde pesos menores representam maior frequência de comunicação), dividido em três comunidades conectadas por pontes (`3 -- 4` e `6 -- 8`), utilizado para validar pontos de articulação, graus de separação até o nó `0` e detecção de 3 comunidades.
* **`tinyEWG.txt`**: Grafo ponderado não direcionado clássico (*Tiny Edge-Weighted Graph*, 8 vértices e 16 arestas com pesos decimais entre `0.16` e `0.93`) da biblioteca de Sedgewick, utilizado para validar o peso total da MST (`1.81`) nos algoritmos de Kruskal e Prim.

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

3. **Execute o programa passando o arquivo de grafo (e, opcionalmente, o número $k$ de clusters [padrão `3`] e o nó de origem para a BFS [padrão `V - 1`]):**

   * **Linux/macOS:**

     ```bash
     ./ewgraph redes_social.txt
     ./ewgraph redes_social.txt 3 11
     ```

   * **Windows:**

     ```bash
     ewgraph.exe redes_social.txt
     ewgraph.exe redes_social.txt 3 11
     ```

---

## 👤 Autor

Desenvolvido por [Diogo Carvalho](https://github.com/DiogoCarv).
