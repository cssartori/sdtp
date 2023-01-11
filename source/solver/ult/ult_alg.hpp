#ifndef H_SDTP_ULT_ALG_H
#define H_SDTP_ULT_ALG_H

#include "../solver.hpp"
#include "../../common/instance.hpp"
#include "apsp_alg.hpp"
#include "ult_model.hpp"
#include "../../util/precise_timer.hpp"


// Implementation of Upper-Lower Tightening algorithm from https://doi.org/10.1016/S0004-3702(97)00009-X
// Ref.: Schwalb, E., & Dechter, R. (1997). Processing disjunctions in temporal constraint networks. Artificial Intelligence, 93(1-2), 29-61.

namespace sdtp::ult{

    class UltAlg : public Solver{
    public:
        UltAlg(const Instance& instance_);
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
        UltModel model;
        std::vector< std::vector<int> > matrix;
        ApspAlg apsp;
        util::PreciseTimer timer;
        long scans;
        bool timeout;

        bool compute_n2(long time_limit);
        int compute_n3();

    };
}
#endif //H_SDTP_ULT_ALG_H
