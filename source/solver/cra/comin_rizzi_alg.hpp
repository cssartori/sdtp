#ifndef H_SDTP_CRA_ALG_H
#define H_SDTP_CRA_ALG_H

#include <vector>
#include <set>
#include <list>
#include "../sdtp_solver.hpp"
#include "../../common/dist_graph.hpp"
#include "../../common/instance.hpp"
#include "bellman_ford_alg.hpp"
#include "dijkstra_alg.hpp"
#include "../../util/precise_timer.hpp"

// Implementation of Comin-Rizzi algorithm from https://doi.org/10.4230/LIPIcs.TIME.2018.10
// Ref.: Comin, C., & Rizzi, R. (2018). On Restricted Disjunctive Temporal Problems: Faster Algorithms and Tractability Frontier. In 25th International Symposium on Temporal Representation and Reasoning (TIME 2018). Schloss Dagstuhl-Leibniz-Zentrum fuer Informatik.

namespace sdtp::cra{
    class CominRizziAlg : public Solver{
    public:
        CominRizziAlg(const Instance& instance_);
        const Instance& get_instance() const;
        std::string get_name() const;
        void extract_solution(std::vector<int>& sol) const;
        long get_runtime_microseconds() const;
        long get_runtime_milliseconds() const;
        long get_num_scans() const;
        bool timedout() const;
        bool run(long time_limit=def::MAX_LONG);
        void reset();

    private:
        std::string name;
        const Instance& instance;
        BellmanFordAlg bfa;
        DijkstraAlg dijkstra;
        std::vector<int> t0; //first feasible solution from BFA
        std::vector<int> t; //current feasible solution
        std::vector< std::vector<int> > dist; //distance matrix
        std::vector<int> vdi; //variable domain index
        boost::dynamic_bitset<> compd;
        std::vector< std::vector<int> > reachable_from;
        std::list<int> linf; //list of infeasible nodes (w.r.t. their domains)
        std::vector< std::list<int>::iterator > linf_pointers;
        util::PreciseTimer timer;
        bool timeout;
        long scans;

        bool solve(long time_limit);
        int update_domain_index(int i);
        bool update_rules(int x);
    };
}


#endif // H_SDTP_CRA_ALG_H
