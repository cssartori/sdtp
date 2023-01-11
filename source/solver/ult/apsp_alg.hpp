#ifndef H_SDTP_ULT_APSP_ALG_H
#define H_SDTP_ULT_APSP_ALG_H

#include "../../common/dist_graph.hpp"
#include "ult_model.hpp"
#include "../../util/precise_timer.hpp"

// Implemntation of classic Floyd-Warshal algorithm

namespace sdtp::ult{

    class ApspAlg{
    private:
        const UltModel& model;
        const DistGraph& graph;
        std::vector< std::vector<int> >& matrix;
        util::PreciseTimer& timer;
        long scans;

    public:
        ApspAlg(const UltModel& model_, std::vector< std::vector<int> >& m_, util::PreciseTimer& timer_);
        void reset_matrix();
        bool solve(long time_limit);
        long get_num_scans() const;
    };
}
#endif //H_SDTP_ULT_APSP_ALG_H
