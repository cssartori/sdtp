#include "dist_alg_bf.hpp"

namespace sdtp::ka{

    DistAlgBf::DistAlgBf(const Instance& instance_, util::PreciseTimer& timer_) : instance(instance_), queue(instance.num_variables()+2), timer(timer_), dummy(instance.num_variables()+1)
    {
        path.assign(instance.num_variables()+1, util::LenParData());
    }

    bool DistAlgBf::run_sp(long time_limit, std::vector<int>& dist){
        for(int i=0;i<static_cast<int>(instance.num_variables());i++){
            dist[i] = 0;
            path[i].len = 0;
            path[i].par = i;
            queue.push(i);
        }

        bool v = compute_distances(dummy, dist);

        return v;
    }

    bool DistAlgBf::run_apsp(long time_limit, std::vector< std::vector<int> >& dist){
        for(int i=0;i<static_cast<int>(instance.num_variables());i++){
            if(timer.exceeds_us(time_limit)) return false; //timeout
            //compute dist[i] (i is the source)
            initialize(i, dist);
            bool v = compute_distances(i, dist[i]);
            if(not v) return false; //instance is inconsistent when only considering t1 constraints
        }
        return true;
    }

    void DistAlgBf::initialize(int src, std::vector< std::vector<int> >& dist){
        queue.reset();
        queue.push(src);
        path[src].len = 0;
        path[src].par = src;
        dist[src][src] = 0;
    }

    bool DistAlgBf::compute_distances(int src, std::vector<int>& d){
        bool valid = true;
        uint32_t iter = 0;

        while(not queue.is_empty() && valid){
            int n = queue.pop();
            if(not queue.has_element(path[n].par))
                valid = relax(n, d, src);
            iter++;
        }
        return valid;
    }

    bool DistAlgBf::relax(int i, std::vector<int>& d, int src){
        scans++;
        for(const Arc& a : instance.dgraph.arcs[i]){
            int j = a.target;
            int w = a.weight;

            int d_j = d[i] + w;
            if(d_j < d[j]){
                d[j] = d_j;

                if(j == src){
                    return false; //negative cycle
                }

                path[j].len = path[i].len + 1;
                if(path[j].len >= static_cast<int>(instance.num_variables())){
                    return false; //negative cycle
                }
                path[j].par = i;

                if(not queue.has_element(j)){
                    queue.push(j);
                }
            }
        }
        return true;
    }

    long DistAlgBf::get_num_scans() const{
        return scans;
    }

}
