#include "EWGraph.h"
#include <queue>
#include <limits>
#include <map>

using namespace std;

class Process
{
private:
    EWGraph G; // Cópia do grafo ponderado não direcionado recebido

public:
    // Construtor principal da classe de processamento
    Process(const EWGraph &G) : G(G) {}

    // =========================================================================
    // 1. ALGORITMO DE K-CLUSTERING (Via MST / Kruskal)
    // =========================================================================
    // O objetivo do k-clustering de espaçamento máximo é dividir os vértices do 
    // grafo em 'k' grupos (clusters) distintos de forma que a menor distância 
    // (peso da aresta) entre dois clusters diferentes seja a maior possível.
    // Lógica: Executa o algoritmo de Kruskal e para assim que restar exatamente 'k' 
    // componentes conexas. A próxima aresta que uniria dois clusters define o "espaçamento".
    class KClustering
    {
    private:
        int k;                         // Número de clusters desejado
        double spacing;                // Espaçamento máximo do agrupamento (peso da menor aresta entre clusters)
        vector<int> clusterId;         // Mapeamento: clusterId[v] indica a qual cluster o vértice 'v' pertence
        vector<vector<int>> clusters;  // Lista com os vértices agrupados por cada cluster
        vector<Edge> clusteringEdges;  // Arestas selecionadas que mantêm os vértices unidos dentro dos clusters

    public:
        KClustering(const EWGraph &G, int k) : k(k), spacing(numeric_limits<double>::infinity())
        {
            int V = G.getV();
            if (k <= 0 || k > V) return;

            // Estrutura Disjoint-Set / Union-Find para gerenciar componentes desconexas
            UnionFind uf(V);

            // Obtém todas as arestas do grafo e ordena pelo peso em ordem crescente O(E log E)
            vector<Edge> edges = G.getAllEdges();
            sort(edges.begin(), edges.end()); 

            int components = V; // Inicialmente, cada vértice é a sua própria componente/cluster (total: V)

            for (const Edge &e : edges)
            {
                int v = e.either();
                int w = e.other(v);

                // Verifica se os vértices 'v' e 'w' pertencem a clusters diferentes
                if (!uf.connected(v, w))
                {
                    // Se já alcançámos a quantidade exata de 'k' componentes, 
                    // a menor aresta restante que conectaria dois clusters distintos 
                    // define o espaçamento (spacing) do agrupamento!
                    if (components == k)
                    {
                        spacing = e.weight;
                        break; // Algoritmo concluído
                    }

                    // Une as duas componentes e armazena a aresta
                    uf.unionSets(v, w);
                    clusteringEdges.push_back(e);
                    components--; // Reduz o número total de componentes ativos
                }
            }

            // --- Mapeamento final dos clusters ---
            // Agrupa os vértices por id numérico (0 até k-1) com base nas raízes do Union-Find
            clusterId.resize(V);
            map<int, int> rootToClusterIndex;
            int currentClusterIndex = 0;

            for (int v = 0; v < V; v++)
            {
                int root = uf.find(v); // Encontra o representante da componente
                if (rootToClusterIndex.find(root) == rootToClusterIndex.end())
                {
                    rootToClusterIndex[root] = currentClusterIndex++;
                }
                int cId = rootToClusterIndex[root];
                clusterId[v] = cId;
            }

            // Organiza a lista final de subconjuntos de vértices
            clusters.resize(currentClusterIndex);
            for (int v = 0; v < V; v++)
            {
                clusters[clusterId[v]].push_back(v);
            }
        }

        int getK() const { return k; }
        double getSpacing() const { return spacing; }
        int getClusterOf(int v) const { return clusterId[v]; }
        vector<vector<int>> getClusters() const { return clusters; }
        vector<Edge> getClusteringEdges() const { return clusteringEdges; }
    };

    // =========================================================================
    // 2. ÁRVORE GERADORA MÍNIMA (MST) - ALGORITMO DE KRUSKAL
    // =========================================================================
    // Algoritmo guloso (greedy) focado em arestas:
    // 1. Ordena todas as arestas do grafo em ordem crescente de peso.
    // 2. Adiciona a aresta de menor peso à MST desde que não forme ciclo (usando Union-Find).
    // Complexidade: O(E log E)
    class MSTKruskal
    {
    private:
        vector<Edge> mst;   // Armazena as (V - 1) arestas pertencentes à Árvore Geradora Mínima
        double totalWeight; // Soma total dos pesos das arestas da MST    

    public:
        MSTKruskal(const EWGraph &G) : totalWeight(0.0)
        {
            // Estrutura Union-Find para detetar ciclos em tempo quase constante
            UnionFind uf(G.getV());

            // Coleta e ordena as arestas pelo peso de forma ascendente
            vector<Edge> edges = G.getAllEdges();
            sort(edges.begin(), edges.end());   

            for (const Edge &e : edges)
            {
                int v = e.either();
                int w = e.other(v);

                // Se 'v' e 'w' não estão no mesmo conjunto, incluir 'e' não cria ciclo
                if (!uf.connected(v, w))
                {
                    uf.unionSets(v, w);    // Une os dois conjuntos
                    mst.push_back(e);       // Adiciona a aresta à MST
                    totalWeight += e.weight; // Acumula o peso
                }
            }   
        }

        vector<Edge> getMST() const { return mst; }
        double getTotalWeight() const { return totalWeight; } 
    };

    // =========================================================================
    // 3. ÁRVORE GERADORA MÍNIMA (MST) - ALGORITMO DE PRIM (Eager Version)
    // =========================================================================
    // Algoritmo guloso focado em vértices:
    // Cresce a MST a partir de um vértice inicial, adicionando a cada passo 
    // a aresta de menor peso que conecta a árvore a um vértice fora dela.
    // Utiliza Fila de Prioridade Indexada (IndexMinPQ) para atualizar dinamicamente
    // o menor custo de conexão para cada vértice fora da árvore.
    // Complexidade: O(E log V)
    class MSTPrim
    {
    private:
        vector<Edge> mst;           // Arestas constituintes da MST
        double totalWeight;         // Soma do peso de todas as arestas da MST
        vector<Edge> edgeTo;        // edgeTo[v] = aresta mais leve que conecta 'v' à MST atual
        vector<double> distTo;      // distTo[v] = peso da aresta mais leve (edgeTo[v])
        vector<bool> marked;        // marked[v] = true se o vértice 'v' já faz parte da MST
        IndexMinPQ<double> pq;      // Fila de prioridade com os vértices candidatos externos e suas distâncias

        // Avalia todas as arestas incidentes ao vértice 'v' recém-adicionado à MST
        void scan(EWGraph &G, int v)
        {
            marked[v] = true; // Marca 'v' como incluído na MST

            for (const Edge &e : G.getAdj(v))
            {
                int w = e.other(v);

                // Se o vizinho 'w' já está na MST, ignora a aresta
                if (marked[w]) continue; 

                // Se a aresta 'e' oferece um caminho mais barato para conectar 'w' à MST do que tínhamos antes:
                if (e.weight < distTo[w])
                {
                    distTo[w] = e.weight;
                    edgeTo[w] = e;

                    // Atualiza a chave de 'w' na Min-PQ se já existir, senão insere
                    if (pq.contains(w)) pq.decreaseKey(w, distTo[w]);
                    else pq.insert(w, distTo[w]);
                }
            }
        }

        // Executa o algoritmo de Prim a partir do vértice de origem 's'
        void prim(EWGraph &G, int s)
        {
            distTo[s] = 0.0;
            pq.insert(s, 0.0);

            while (!pq.isEmpty())
            {
                // Remove o vértice 'v' com a aresta de menor custo incidente à MST
                int v = pq.delMin();

                // Adiciona a aresta correspondente à lista final (exceto na raiz inicial)
                if (edgeTo[v].either() != -1) 
                {
                    mst.push_back(edgeTo[v]);
                    totalWeight += edgeTo[v].weight;
                }

                // Explora as arestas de 'v' para atualizar os vizinhos fora da MST
                scan(G, v);
            }
        }

    public: 
        MSTPrim(const EWGraph &G) : totalWeight(0.0), pq(G.getV())
        {
            // Inicialização das estruturas de apoio
            for (int i = 0; i < G.getV(); i++)
            {
                distTo.push_back(numeric_limits<double>::infinity());
                edgeTo.push_back(Edge(-1, -1, 0.0));
                marked.push_back(false);
            }

            // Dispara a Prim para cada componente conexa (trata grafos desconexos)
            for (int i = 0; i < G.getV(); i++)
            {
                if (!marked[i]) prim(const_cast<EWGraph&>(G), i);
            }
        }
        
        vector<Edge> getMST() const { return mst; }
        double getTotalWeight() const { return totalWeight; } 
    };

    // =========================================================================
    // 4. PONTOS DE ARTICULAÇÃO (Algoritmo de Tarjan via DFS)
    // =========================================================================
    // Identifica vértices de corte cuja remoção aumenta o número de componentes
    // conexas do grafo, fragmentando a rede em subgrupos isolados.
    // Complexidade: O(V + E)
    class ArticulationPoints
    {
    private:
        const EWGraph &G;       // Referência ao grafo analisado
        vector<bool> visited;   // Marca os vértices já visitados na DFS
        vector<int> tin;        // Tempo de descoberta de cada vértice
        vector<int> low;        // Menor tempo de descoberta alcançável na subárvore
        vector<bool> isArtic;   // Indica se o vértice é um ponto de articulação
        int timer;              // Contador incremental de tempo da busca

        // Explora o grafo em profundidade atualizando os tempos de descoberta e retorno
        void dfs(int v, int parent = -1)
        {
            visited[v] = true;
            tin[v] = low[v] = timer++;
            int children = 0; // Quantidade de filhos diretos na árvore DFS

            for (const Edge &e : G.getAdj(v))
            {
                int w = e.other(v);
                if (w == parent) continue; // Ignora a aresta bidirecional de retorno ao pai

                if (visited[w])
                {
                    // Aresta de retorno (back-edge): atualiza o menor ancestral alcançável
                    low[v] = min(low[v], tin[w]);
                }
                else
                {
                    children++;
                    dfs(w, v);
                    low[v] = min(low[v], low[w]);

                    // Vértice interno sem caminho alternativo acima dele na árvore DFS
                    if (parent != -1 && low[w] >= tin[v])
                    {
                        isArtic[v] = true;
                    }
                }
            }

            // Raiz da árvore DFS com dois ou mais ramos independentes
            if (parent == -1 && children > 1)
            {
                isArtic[v] = true;
            }
        }

    public:
        ArticulationPoints(const EWGraph &G) : G(G), timer(0)
        {
            int V = G.getV();
            visited.assign(V, false);
            tin.assign(V, -1);
            low.assign(V, -1);
            isArtic.assign(V, false);

            // Executa a busca para todas as componentes conexas
            for (int v = 0; v < V; v++)
            {
                if (!visited[v])
                {
                    dfs(v);
                }
            }
        }

        vector<int> getArticulationPoints() const
        {
            vector<int> points;
            for (int v = 0; v < G.getV(); v++)
            {
                if (isArtic[v])
                {
                    points.push_back(v);
                }
            }
            return points;
        }
    };

    // =========================================================================
    // 5. CAMINHO MÍNIMO EM NÚMERO DE ARESTAS (BFS / Graus de Separação)
    // =========================================================================
    // Calcula a menor sequência de saltos (arestas) entre um vértice de origem
    // e os demais vértices do grafo utilizando Busca em Largura (BFS).
    // Complexidade: O(V + E)
    class ShortestChainBFS
    {
    private:
        const EWGraph &G;       // Referência ao grafo analisado
        int source;             // Vértice de partida
        vector<bool> marked;    // Marca os vértices alcançados a partir da origem
        vector<int> edgeTo;     // Armazena o predecessor de cada vértice no caminho
        vector<int> distTo;     // Armazena a quantidade mínima de arestas desde a origem

        // Executa a busca em largura por camadas a partir de 's'
        void bfs(int s)
        {
            queue<int> q;
            marked[s] = true;
            distTo[s] = 0;
            q.push(s);

            while (!q.empty())
            {
                int v = q.front();
                q.pop();

                for (const Edge &e : G.getAdj(v))
                {
                    int w = e.other(v);
                    if (!marked[w])
                    {
                        edgeTo[w] = v;
                        distTo[w] = distTo[v] + 1;
                        marked[w] = true;
                        q.push(w);
                    }
                }
            }
        }

    public:
        ShortestChainBFS(const EWGraph &G, int s) : G(G), source(s)
        {
            int V = G.getV();
            marked.assign(V, false);
            edgeTo.assign(V, -1);
            distTo.assign(V, numeric_limits<int>::max());

            if (s >= 0 && s < V)
            {
                bfs(s);
            }
        }

        bool hasPathTo(int v) const
        {
            return (v >= 0 && v < G.getV()) && marked[v];
        }

        int distanceTo(int v) const
        {
            return hasPathTo(v) ? distTo[v] : -1;
        }

        // Reconstrói a sequência ordenada de vértices da origem até 'v'
        vector<int> pathTo(int v) const
        {
            vector<int> path;
            if (!hasPathTo(v)) return path;

            for (int x = v; x != source; x = edgeTo[x])
            {
                path.push_back(x);
            }
            path.push_back(source);
            reverse(path.begin(), path.end());
            return path;
        }
    };
};

// =========================================================================
// FUNÇÃO PRINCIPAL (MAIN)
// =========================================================================
int main(int argc, char *argv[])
{
    // Validação dos argumentos passados via linha de comando
    if (argc < 2)
    {
        cerr << "Uso: " << argv[0] << " <arquivo_do_grafo> [num_clusters] [no_origem]" << endl;
        return 1;
    }

    // Abertura do arquivo de entrada contendo a estrutura do grafo
    ifstream in(argv[1]);
    if (!in)
    {
        cerr << "Erro ao abrir o arquivo: " << argv[1] << endl;
        return 1;
    }

    // Instanciação e leitura do Grafo Ponderado Não Direcionado (Edge-Weighted Graph)
    EWGraph G(in);

    // Impressão visual / estrutural do grafo
    G.show();
    G.showDot();
    
    // -------------------------------------------------------------------------
    // EXECUÇÃO DA MST - KRUSKAL
    // -------------------------------------------------------------------------
    Process::MSTKruskal kruskal(G);
    cout << endl << "------- MST Kruskal -------" << endl << endl;
    cout << "Peso total da MST (Kruskal): " << kruskal.getTotalWeight() << endl;
    cout << "Arestas na MST (Kruskal):" << endl;
    for (const Edge &e : kruskal.getMST())
    {
        cout << e.either() << " -- " << e.other(e.either()) << " (Peso: " << e.weight << ")" << endl;
    }

    // -------------------------------------------------------------------------
    // EXECUÇÃO DA MST - PRIM
    // -------------------------------------------------------------------------
    Process::MSTPrim prim(G);
    cout << endl << "------- MST Prim -------" << endl << endl;
    cout << "Peso total da MST (Prim): " << prim.getTotalWeight() << endl;
    cout << "Arestas na MST (Prim):" << endl;
    for (const Edge &e : prim.getMST())
    {
        cout << e.either() << " -- " << e.other(e.either()) << " (Peso: " << e.weight << ")" << endl;
    }

    // -------------------------------------------------------------------------
    // EXECUÇÃO DO K-CLUSTERING
    // -------------------------------------------------------------------------
    // Define o valor padrão para 3 clusters ou utiliza o número passado como segundo argumento no terminal
    int k = 3;
    if (argc >= 3)
    {
        k = atoi(argv[2]);
    }

    Process::KClustering clustering(G, k);
    cout << endl << "------- " << k << "-Clustering -------" << endl;
    cout << "Espaçamento máximo do clustering: " << clustering.getSpacing() << endl;
    
    vector<vector<int>> clusters = clustering.getClusters();
    for (size_t i = 0; i < clusters.size(); i++)
    {
        cout << "Cluster " << i << ": { ";
        for (int v : clusters[i])
        {
            cout << v << " ";
        }
        cout << "}" << endl;
    }

    // -------------------------------------------------------------------------
    // EXECUÇÃO DOS PONTOS DE ARTICULAÇÃO
    // -------------------------------------------------------------------------
    Process::ArticulationPoints ap(G);
    vector<int> articulationPoints = ap.getArticulationPoints();
    cout << endl << "------- Pontos de Articulação -------" << endl;
    if (articulationPoints.empty())
    {
        cout << "Rede Totalmente Robusta" << endl;
    }
    else
    {
        cout << "Pontos de articulação encontrados: ";
        for (size_t i = 0; i < articulationPoints.size(); i++)
        {
            cout << articulationPoints[i] << (i + 1 < articulationPoints.size() ? ", " : "");
        }
        cout << endl;
    }

    // -------------------------------------------------------------------------
    // EXECUÇÃO DO CAMINHO MÍNIMO EM VÍNCULOS (BFS) ATÉ O NÓ 0
    // -------------------------------------------------------------------------
    // Utiliza o último vértice como origem padrão ou o valor passado no terceiro argumento
    int destino = 0;
    int origem = (argc >= 4) ? atoi(argv[3]) : (G.getV() - 1);
    if (origem < 0 || origem >= G.getV())
    {
        origem = G.getV() - 1;
    }

    Process::ShortestChainBFS bfs(G, origem);
    cout << endl << "------- Caminho Mínimo (BFS) de " << origem << " para " << destino << " -------" << endl;
    if (bfs.hasPathTo(destino))
    {
        vector<int> path = bfs.pathTo(destino);
        cout << "Sequência de vértices: ";
        for (size_t i = 0; i < path.size(); i++)
        {
            cout << path[i] << (i + 1 < path.size() ? " -> " : "");
        }
        cout << endl;
        cout << "Total de vínculos percorridos: " << bfs.distanceTo(destino) << endl;
    }
    else
    {
        cout << "Não há caminho entre " << origem << " e " << destino << "." << endl;
    }

    return 0;
}
