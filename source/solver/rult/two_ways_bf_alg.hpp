#ifndef H_SDTP_RULT_TWO_WAYS_BF_ALG_H
#define H_SDTP_RULT_TWO_WAYS_BF_ALG_H

#include <map>
#include <boost/dynamic_bitset.hpp>
#include "../../common/dist_graph.hpp"
#include "../../common/domain.hpp"
#include "../../util/vec_queue.hpp"
#include "../../util/len_par_data.hpp"

namespace sdtp::rult{
    class TwoWaysBfAlg{
    public:
        TwoWaysBfAlg(const DistGraph& g_dir, const DistGraph& g_rev);
        bool solve(std::vector<int>& dist_d, std::vector<int>& dist_r);
        long get_num_scans() const;

    private:
        const DistGraph& dir_graph;
        const DistGraph& rev_graph;
        std::vector<util::LenParData> path_d, path_r;
        util::VecQueue queue;
        boost::dynamic_bitset<> dd, dr;
        long scans;

        void initialize(std::vector<int>& dist_d, std::vector<int>& dist_r);
        bool compute_distances(const DistGraph& graph, std::vector<util::LenParData>& path, std::vector<int>& dist);
        bool relax(const DistGraph& graph, int i, std::vector<util::LenParData>& path, std::vector<int>& dist);
    };
}
#endif //H_SDTP_RULT_TWO_WAYS_BF_ALG_H
