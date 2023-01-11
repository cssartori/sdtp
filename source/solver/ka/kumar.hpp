#ifndef H_SDTP_KUMAR_ALG_H
#define H_SDTP_KUMAR_ALG_H

#include <vector>
#include <set>
#include "interval.hpp"
#include "../../common/instance.hpp"
#include "max_flow_graph.hpp"
#include "dist_alg_bf.hpp"
#include "../../util/precise_timer.hpp"
#include "bellman_ford_alg.hpp"
#include "../sdtp_solver.hpp"

// Implementation of Kumar's Algorithm from https://www.aaai.org/Papers/AAAI/2004/AAAI04-011.pdf
// Ref.: Kumar, T. S. (2004). A polynomial-time algorithm for simple temporal problems with piecewise constant domain preference functions. In AAAI (pp. 67-72).

namespace sdtp::ka{

    class KumarAlg : public Solver{
    public:
        enum class TypeDa{BELLMAN_FORD, JOHNSON_ALG};

        KumarAlg(Instance instance_, TypeDa type_);
        bool run(long time_limit=def::MAX_LONG);
        void extract_solution(std::vector<int>& sol) const;
        const Instance& get_instance() const;
        std::string get_name() const;
        long get_num_scans() const;
        long get_runtime_microseconds() const;
        long get_runtime_milliseconds() const;
        bool timedout() const;
        void reset();

    private:
        std::string name;
        Instance instance;
        std::vector< Interval > intervals;
        std::vector< std::vector<int> > dist;
        BellmanFordAlg kbf_alg;
        MaxFlowGraph mfg;

        util::PreciseTimer timer;
        std::vector<int> vdi;
        std::vector<int> sol;
        bool timeout;
        TypeDa type;

        bool solve(long time_limit);
        bool prepare_intervals(long time_limit);
        bool define_size2_conflicts(std::vector< std::pair<int,int> >& conf2, long time_limit);
        void bfs_reacheable_from(int source, std::vector<bool>& reach);
        void dfs_reacheable_from(int source, std::vector<bool>& reach);
        bool create_sets(const std::vector<bool>& reach, std::vector<int>& q);
        bool has_time(long time_limit);
    };

}
#endif //H_SDTP_KUMAR_ALG_H
