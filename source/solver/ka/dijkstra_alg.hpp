#ifndef H_SDTP_KA_DIJKSTRA_ALG_H
#define H_SDTP_KA_DIJKSTRA_ALG_H

#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "../../util/heaps/nary_heap.hpp"
#include "../../util/heaps/sanders/knheap.h"
#include "../../util/heaps/sanders/knheap.C"
#include "../../common/dist_graph.hpp"
#include "../../util/precise_timer.hpp"

namespace sdtp::ka{
    class DijkstraAlg{
    public:
        enum class Type{STANDARD, SANDERS};
        DijkstraAlg(const DistGraph& graph_, const std::vector<int>& we_, Type type_);
        ~DijkstraAlg();
        void run(int source, std::vector<int>& dist);
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
        long scans;

        void run_std(int source, std::vector<int>& dist);
        void run_sanders(int source, std::vector<int>& dist);
    };
}

#endif // H_SDTP_KA_DIJKSTRA_ALG_H
