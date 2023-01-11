#include "dist_alg_ja.hpp"
#include "dist_alg_bf.hpp"
#include "dijkstra_alg.hpp"

namespace sdtp::ka{

    DistAlgJa::DistAlgJa(const Instance& instance_, std::vector< std::vector<int> >& dist_, util::PreciseTimer& timer_) : instance(instance_), dist(dist_), queue(instance.num_variables()+1), timer(timer_)
    {
        len.assign(instance.num_variables(), 0);
    }

    bool DistAlgJa::run(long time_limit){
        std::vector<int> d0(instance.num_variables(), def::MAX_INT);
        DistAlgBf dbf(instance, timer);
        bool v = dbf.run_sp(time_limit, d0);
        if(not v) return false; //negative cycle detected

        DijkstraAlg dijk(instance.dgraph, d0, DijkstraAlg::Type::SANDERS);
        for(int i=0;i<static_cast<int>(instance.num_variables());i++){
            if(timer.exceeds_us(time_limit)) return false; //timeout
            dijk.run(i, dist[i]);
            scans += dijk.get_num_scans();
            dijk.reset();
        }

        return true;
    }

    long DistAlgJa::get_num_scans() const{
        return scans;
    }

}
