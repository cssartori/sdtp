#include "dijkstra_alg.hpp"

namespace sdtp::cra{
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

    void DijkstraAlg::run(int source, std::vector<int>& dist, std::vector<int>& reachable){
        if(type == Type::STANDARD) run_std(source, dist, reachable);
        else run_sanders(source, dist, reachable);
    }

    void DijkstraAlg::run_std(int source, std::vector<int>& dist, std::vector<int>& reachable){
        visited.reset();
        dist[source] = 0;
        nheap.reset();
        nheap.insert(source, 0);

        while(not nheap.empty()){
            int v = nheap.top();
            nheap.pop();
            visited[v] = true;
            reachable.push_back(v);

            scans++;
            for(const Arc& a : graph.arcs[v]){
                //arc (v,u,w)
                int u = a.target;
                int weight = a.weight + we[u] - we[v];

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
        }
    }


    void DijkstraAlg::run_sanders(int source, std::vector<int>& dist, std::vector<int>& reachable){
        visited.reset();
        dist[source] = 0;
        kheap.insert(0, source);

        while(kheap.getSize() > 0){
            int v, k;
            kheap.getMin(&k, &v);
            kheap.deleteMin(&k, &v);
            if(visited[v]) continue;

            visited[v] = true;
            reachable.push_back(v);
            scans++;
            for(const Arc& a : graph.arcs[v]){
                //arc (v,u,w)
                int u = a.target;
                int weight = a.weight + we[u] - we[v];

                if(dist[v] + weight < dist[u]){
                    dist[u] = dist[v] +  weight;
                    kheap.insert(dist[u], u);
                }
            }
        }
    }

    long DijkstraAlg::get_num_scans() const{
        return scans;
    }

}
