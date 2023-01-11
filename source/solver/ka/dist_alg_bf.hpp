#ifndef H_SDTP_KA_DIST_ALG_BF_H
#define H_SDTP_KA_DIST_ALG_BF_H

#include <map>
#include "../../common/instance.hpp"
#include "../../common/domain.hpp"
#include "../../util/vec_queue.hpp"
#include "../../util/precise_timer.hpp"
#include "../../util/len_par_data.hpp"

namespace sdtp::ka{
    class DistAlgBf{
    public:
        DistAlgBf(const Instance& inst_, util::PreciseTimer& timer_);
        bool run_sp(long time_limit, std::vector<int>& dist);
        bool run_apsp(long time_limit, std::vector< std::vector<int> >& dist);
        long get_num_scans() const;

    private:
        const Instance& instance;
        util::VecQueue queue;
        std::vector<util::LenParData> path;
        long scans;
        util::PreciseTimer& timer;
        int dummy;

        void initialize(int src, std::vector< std::vector<int> >& dist);
        bool compute_distances(int src, std::vector<int>& dist);
        bool relax(int i, std::vector<int>& dist, int src);
    };
}

#endif //H_SDTP_KA_DIST_ALG_BF_H
