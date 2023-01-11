#include "bellman_ford_alg.hpp"
#include <algorithm>

namespace sdtp::cra{

    BellmanFordAlg::BellmanFordAlg(const Instance& inst_)
        : instance(inst_), graph(instance.rgraph), queue(graph.num_nodes()+1)
    {

    }

    bool BellmanFordAlg::solve(std::vector<int>& dist){
        initialize(dist);
        return compute_distances(dist);
    }

    void BellmanFordAlg::initialize(std::vector<int>& dist){
        scans = 0;
        queue.reset();
        queue.push(graph.node_zero());
        path.assign(graph.num_nodes(), util::LenParData());
        path[graph.node_zero()].par = graph.node_zero();
        dist[graph.node_zero()] = 0;
    }

    bool BellmanFordAlg::compute_distances(std::vector<int>& dist){
        bool valid = true;
        uint32_t iter = 0;

        while(not queue.is_empty() && valid){
            int n = queue.pop();
            if(not queue.has_element(path[n].par))
                valid = relax(n, dist);
            iter++;
        }

        return valid;
    }

    bool BellmanFordAlg::relax(int i, std::vector<int>& dist){
        scans++;
        for(const Arc& a : graph.arcs[i]){
            int j = a.target;
            int w = a.weight;

            //store distances in positive form to avoid additional effort to make them positive after BF
            int dj = -dist[i] + w;
            if(dj < -dist[j]){
                dist[j] = -dj;
                if(j == graph.node_zero()) return false; //negative cycle
                if(-dj > instance.var_domains[j].back().ub) return false;
                path[j].len = path[i].len + 1;
                if(path[j].len >= static_cast<int>(graph.num_nodes())) return false; //negative cycle
                path[j].par = i;

                if(not queue.has_element(j)){
                    queue.push(j);
                }
            }
        }
        return true;
    }

    long BellmanFordAlg::get_num_scans() const{
        return scans;
    }
}
