#include "dijkstra_alg.hpp"

namespace sdtp::ka{

    DijkstraAlg::DijkstraAlg(const DistGraph& graph_, const std::vector<int>& we_, Type type_) : graph(graph_), we(we_),
                                                                                                 nheap(graph.num_nodes(), 4),
                                                                                                 kheap(std::numeric_limits<int>::max(), std::numeric_limits<int>::min()){
        visited.resize(graph.num_nodes());
        scans = 0;
        type = type_;
    }

    DijkstraAlg::~DijkstraAlg(){

    }

    void DijkstraAlg::reset(){
        scans = 0;
    }

    void DijkstraAlg::run(int source, std::vector<int>& dist){
        if(type == Type::STANDARD) run_std(source, dist);
        else run_sanders(source, dist);
    }

    void DijkstraAlg::run_std(int source, std::vector<int>& dist){
        visited.reset();
        dist[source] = 0;
        nheap.reset();
        nheap.insert(source, 0);

        while(not nheap.empty()){
            int v = nheap.top();
            nheap.pop();
            visited[v] = true;

            scans++;
            for(const Arc& a : graph.arcs[v]){
                //arc (v,u,w)
                int u = a.target;
                int weight = a.weight + we[v] - we[u];

                if(not visited[u]){
                    if(dist[u] == def::MAX_INT){ //distance is "infinity"
                        dist[u] = dist[v] +  weight;
                        nheap.insert(u, dist[u]);
                    }else{
                        int new_du = std::min(dist[u], dist[v] + weight);
                        if(new_du < dist[u]){
                            nheap.update_key(u, new_du);
                            dist[u] = new_du;
                        }
                    }
                }
            }

            dist[v] = dist[v] + we[v] - we[source];
        }
    }


    void DijkstraAlg::run_sanders(int source, std::vector<int>& dist){
        visited.reset();
        dist[source] = 0;
        kheap.insert(0, source);

        while(kheap.getSize() > 0){
            int v, k;
            kheap.getMin(&k, &v);
            kheap.deleteMin(&k, &v);
            if(visited[v]) continue;

            visited[v] = true;
            scans++;
            for(const Arc& a : graph.arcs[v]){
                //arc (v,u,w)
                int u = a.target;
                int weight = a.weight + we[v] - we[u];

                if(not visited[u] && dist[v] + weight < dist[u]){
                    dist[u] = dist[v] +  weight;
                    kheap.insert(dist[u], u);
                }
            }
            dist[v] = dist[v] + we[v] - we[source];
        }
    }

    long DijkstraAlg::get_num_scans() const{
        return scans;
    }

}
