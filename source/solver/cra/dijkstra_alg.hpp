#ifndef H_SDTP_CRA_DIJKSTRA_ALG_H
#define H_SDTP_CRA_DIJKSTRA_ALG_H

#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "../../util/heaps/nary_heap.hpp"
#include "../../util/heaps/sanders/knheap.h"
#include "../../util/heaps/sanders/knheap.C"
#include "../../common/dist_graph.hpp"
#include "../../util/precise_timer.hpp"

namespace sdtp::cra{
    class DijkstraAlg{
    public:
        enum class Type{STANDARD, SANDERS};
        DijkstraAlg(const DistGraph& graph_, const std::vector<int>& we_, Type type_);
        ~DijkstraAlg();
        void run(int source, std::vector<int>& dist, std::vector<int>& reachable);
        void reset();
        long get_num_scans() const;
        long eltime=0;

    private:
        const DistGraph& graph;
        const std::vector<int>& we;
        Type type;
        util::NaryHeap nheap;
        KNHeap<int, int> kheap;
        boost::dynamic_bitset<> visited;
        util::PreciseTimer timer;
        long scans;

        void run_std(int source, std::vector<int>& dist, std::vector<int>& reachable);
        void run_sanders(int source, std::vector<int>& dist, std::vector<int>& reachable);
    };
}

#endif // H_SDTP_CRA_DIJKSTRA_ALG_H
