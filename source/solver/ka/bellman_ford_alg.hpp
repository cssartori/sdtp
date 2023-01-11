#ifndef H_SDTP_KA_BELLMAN_FORD_ALG_H
#define H_SDTP_KA_BELLMAN_FORD_ALG_H

#include <map>
#include "../../common/instance.hpp"
#include "../../common/domain.hpp"
#include "../../util/vec_queue.hpp"
#include "../../util/len_par_data.hpp"

namespace sdtp::ka{

    class BellmanFordAlg{
    public:
        BellmanFordAlg(const Instance& instance_, std::vector<int>& vdi_);
        bool solve(std::vector<int>& dist);
        long get_num_scans() const;

    private:
        const Instance& instance;
        std::vector<int>& vdi;
        util::VecQueue queue;
        std::vector<util::LenParData> path;
        long scans;

        void initialize(std::vector<int>& dist);
        bool compute_distances(std::vector<int>& dist);
        bool relax(int i, std::vector<int>& dist);
    };
}
#endif //H_SDTP_KA_BELLMAN_FORD_ALG_H
