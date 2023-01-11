#include "bellman_ford_alg.hpp"

namespace sdtp::ka{

    BellmanFordAlg::BellmanFordAlg(const Instance& instance_, std::vector<int>& vdi_)
        : instance(instance_), vdi(vdi_), queue(instance.num_variables()+1)
    { }

    bool BellmanFordAlg::solve(std::vector<int>& dist){
        initialize(dist);
        return compute_distances(dist);
    }

    void BellmanFordAlg::initialize(std::vector<int>& dist){
        scans = 0;
        queue.reset();
        queue.push(instance.node_zero);
        path.assign(instance.num_variables(), util::LenParData());
        path[instance.node_zero].par = instance.node_zero;
        dist[instance.node_zero] = 0;
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
        for(const Arc& a : instance.dgraph.arcs[i]){
            int j = a.target;
            int w = a.weight;

            if(i == instance.node_zero) w = instance.var_domains[j][vdi[j]].ub;
            else if(j == instance.node_zero) w = -instance.var_domains[i][vdi[i]].lb;

            int dj = dist[i] + w;
            if(dj < dist[j]){
                dist[j] = dj;

                if(j == instance.node_zero) return false; //negative cycle
                path[j].len = path[i].len + 1;
                if(path[j].len >= static_cast<int>(instance.num_variables())) return false; //negative cycle
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
