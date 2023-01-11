#include "red_ult_alg.hpp"
#include <algorithm>

namespace sdtp::rult{

    RedUltAlg::RedUltAlg(Instance instance_) :
        instance((instance_.add_max_bound_constraints(), instance_)),
        twbf(instance.rgraph, instance.dgraph)
    {
        scans = 0;
        timeout = false;
        fd.assign(instance.num_variables(), def::MAX_INT);
        bd.assign(instance.num_variables(), def::MAX_INT);
        di.assign(instance.num_variables(), std::pair<int,int>(0,0));
        for(int i=1;i<(int)instance.num_variables();i++){
            di[i].second = (int)instance.var_domains[i].size()-1;
        }
        var_domains = instance.var_domains;
        name = def::ALG_RULT_NAME;
    }

    const Instance& RedUltAlg::get_instance() const{
        return instance;
    }

    std::string RedUltAlg::get_name() const{
        return name;
    }

    void RedUltAlg::extract_solution(std::vector<int>& sol) const{
        sol = fd;
        std::transform(sol.begin(), sol.end(), sol.begin(), std::negate<int>());
    }

    void RedUltAlg::get_bounds(std::vector<int>& low, std::vector<int>& upp){
        low = fd;
        std::transform(low.begin(), low.end(), low.begin(), std::negate<int>());
        upp = bd;
    }

    long RedUltAlg::get_runtime_microseconds() const{
        return timer.elapsed_microseconds();
    }

    long RedUltAlg::get_runtime_milliseconds() const{
        return timer.elapsed_milliseconds();
    }


    bool RedUltAlg::compute_bounds(){
        bool v = twbf.solve(fd, bd);
        scans += twbf.get_num_scans();
        return v;
    }

    bool RedUltAlg::has_domain_changed(const std::vector<Domain>& dv1, const std::vector<Domain>& dv2){
        if(dv1.size() != dv2.size()) return true;
        bool diff = false;
        for(size_t k=0;k<dv1.size();k++){
            if(dv1[k] != dv2[k]){
                diff = true;
                break;
            }
        }
        return diff;
    }

    bool RedUltAlg::intersect_var_domains(size_t i, Domain dom){
        Domain de;
        Domain dl;

        scans++;

        //early
        for(int k=di[i].first;k<(int)instance.var_domains[i].size();k++){
            bool inter;
            Domain in_d;
            std::tie(inter, in_d) = dom.intersection(instance.var_domains[i][k]);
            if(inter){
                de = in_d;
                di[i].first = k;
                break;
            }
        }

        //late
        for(int k=di[i].second;k>=0;k--){
            bool inter;
            Domain in_d;
            std::tie(inter, in_d) = dom.intersection(instance.var_domains[i][k]);
            if(inter){
                dl = in_d;
                di[i].second = k;
                break;
            }
        }

        bool change = false;
        int prev_e = -instance.rgraph.get_arc(instance.node_zero, i).weight;
        int prev_l = instance.rgraph.get_arc(i, instance.node_zero).weight;

        if(de.lb != prev_e || dl.ub != prev_l){
            instance.dgraph.update_arc_weight(instance.node_zero, i, dl.ub);
            instance.rgraph.update_arc_weight(instance.node_zero, i, -de.lb);

            instance.rgraph.update_arc_weight(i, instance.node_zero, dl.ub);
            instance.dgraph.update_arc_weight(i, instance.node_zero, -de.lb);
            change = true;
        }

        return change;
    }

    int RedUltAlg::tighten_bounds(){
        int count = 0;

        for(size_t i=1;i<instance.num_variables();i++){
            int lb = -fd[i];
            int ub = bd[i];

            bool ch = intersect_var_domains(i, Domain(lb, ub));

            if(ch && di[i].first > di[i].second) return -1; //empty domain means no solution
            else if(ch) count++;
        }
        //printf("=====\n");
        return count;
    }

    bool RedUltAlg::run(long time_limit){
        timer.start();
        bool feas = true;
        bool done = false;
        size_t iter = 0;
        while(feas && not done && not timer.exceeds_us(time_limit)){
            iter++;
            feas = compute_bounds();
            if(not feas) break;
            int r = tighten_bounds();
            if(r < 0) feas = false;
            else if(r == 0) done = true;
        }

        if(feas && not done){
            //timedout
            timeout = true;
            feas = false;
        }

        timer.stop();
        if(feas && done) return true;
        return false;
    }

    void RedUltAlg::reset(){
        fd.assign(instance.num_variables(), def::MAX_INT);
        bd.assign(instance.num_variables(), def::MAX_INT);
        instance.var_domains = var_domains;

        for(int i=1;i<static_cast<int>(instance.rgraph.num_nodes());i++){
            di[i].first = 0;
            di[i].second = (int)instance.var_domains[i].size()-1;

            const auto& vdom_i = instance.var_domains.at(i);
            instance.dgraph.update_arc_weight(instance.node_zero, i, vdom_i.back().ub);
            instance.rgraph.update_arc_weight(instance.node_zero, i, -vdom_i.front().lb);
            instance.rgraph.update_arc_weight(i, instance.node_zero, vdom_i.back().ub);
            instance.dgraph.update_arc_weight(i, instance.node_zero, -vdom_i.front().lb);
        }
        scans = 0;
        timeout = false;
    }

    long RedUltAlg::get_num_scans() const{
        return scans;
    }

    bool RedUltAlg::timedout() const{
        return timeout;
    }

}
