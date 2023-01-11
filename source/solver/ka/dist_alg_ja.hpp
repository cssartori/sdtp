#ifndef H_SDTP_KA_DIST_ALG_JA_H
#define H_SDTP_KA_DIST_ALG_JA_H

#include <map>
#include "../../common/instance.hpp"
#include "../../common/domain.hpp"
#include "../../util/vec_queue.hpp"
#include "../../util/precise_timer.hpp"

namespace sdtp::ka{
    class DistAlgJa{
    public:
        DistAlgJa(const Instance& inst_, std::vector< std::vector<int> >& dist_, util::PreciseTimer& timer_);
        bool run(long time_limit);
        long get_num_scans() const;

    private:
        const Instance& instance;
        std::vector< std::vector<int> >& dist;
        std::vector<int> len;
        util::VecQueue queue;
        long scans;
        util::PreciseTimer& timer;

        void initialize(int src);
        bool compute_distances(int src, std::vector<int>& dist);
        bool propagate(int i, std::vector<int>& dist, int src);
    };
}

#endif //H_SDTP_KA_DIST_ALG_H
