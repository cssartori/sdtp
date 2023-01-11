#ifndef H_SDTP_BFDC_ALG_H
#define H_SDTP_BFDC_ALG_H

#include <map>
#include <algorithm>
#include "../sdtp_solver.hpp"
#include "../../common/domain.hpp"
#include "../../common/instance.hpp"
#include "../../util/vec_queue.hpp"
#include "../../util/precise_timer.hpp"
#include "../../util/len_par_data.hpp"

namespace sdtp::bfdc{
    class BfdcAlg : public Solver{
    public:
        BfdcAlg(const Instance& instance_);
        bool run(long time_limit=def::MAX_LONG);
        void extract_solution(std::vector<int>& sol) const;
        const Instance& get_instance() const;
        std::string get_name() const;
        long get_runtime_microseconds() const;
        long get_runtime_milliseconds() const;
        long get_num_scans() const;
        bool timedout() const;
        void reset();
        void extract_vid(std::vector<int>& vid_){
            vid_ = vid;
        }

    private:
        std::string name;
        Instance instance;
        std::vector<int> vid; //var index domain
        std::vector<int> distance;
        std::vector<util::LenParData> path;
        util::VecQueue queue;
        util::PreciseTimer timer;
        long scans;
        bool timeout;

        void initialize();
        bool compute_distances(std::vector<int>& dist, long time_limit);
        bool valid_domain(int i, int value) const;
        void change_domain_arcs(int i, const Domain& dom);
        bool update_domain(int i, int& value);
        bool relax(int i, std::vector<int>& dist);
        bool domain_check(int i, std::vector<int>& dist);

    };
}

#endif //H_SDTP_BFDC_ALG_H
