#include "bfdc_alg.hpp"

namespace sdtp::bfdc{

    BfdcAlg::BfdcAlg(const Instance& instance_) : instance(instance_), queue(instance.num_variables()+1)
    {
        name = def::ALG_BFDC_NAME;
        instance.add_max_bound_constraints();
        distance.assign(instance.rgraph.num_nodes(), def::MAX_INT);
        distance[instance.rgraph.node_zero()] = 0;
        vid.assign(instance.rgraph.num_nodes(), 0);
        path.assign(instance.rgraph.num_nodes(), util::LenParData());
        scans = 0;
        timeout = false;
    }

    bool BfdcAlg::run(long time_limit){
        timer.start();
        initialize();
        bool a = compute_distances(distance, time_limit);
        timer.stop();

        return a;
    }

    void BfdcAlg::reset(){
        scans = 0;
        timeout = false;
        distance.assign(instance.rgraph.num_nodes(), def::MAX_INT);
        distance[instance.rgraph.node_zero()] = 0;
        vid.assign(instance.rgraph.num_nodes(), 0);
        path.assign(instance.rgraph.num_nodes(), util::LenParData());
        for(int i=1;i<static_cast<int>(instance.rgraph.num_nodes());i++){
            const auto& vdom_i = instance.var_domains.at(i);
            instance.rgraph.update_arc_weight(instance.node_zero, i, -vdom_i[0].lb);
        }
    }

    void BfdcAlg::extract_solution(std::vector<int>& sol) const{
        sol = distance;
        std::transform(sol.begin(), sol.end(), sol.begin(), std::negate<int>());
    }

    const Instance& BfdcAlg::get_instance() const{
        return instance;
    }

    std::string BfdcAlg::get_name() const{
        return name;
    }

    long BfdcAlg::get_runtime_microseconds() const{
        return timer.elapsed_microseconds();
    }

    long BfdcAlg::get_runtime_milliseconds() const{
        return timer.elapsed_milliseconds();
    }

    void BfdcAlg::initialize(){
        queue.reset();
        queue.push(instance.rgraph.node_zero());
        path[instance.rgraph.node_zero()].par = instance.rgraph.node_zero();
        distance[instance.rgraph.node_zero()] = 0;
    }

    bool BfdcAlg::compute_distances(std::vector<int>& dist, long time_limit){
        bool valid = true;
        uint32_t iter = 0;

        while(not queue.is_empty() && valid && not timer.exceeds_us(time_limit)){
            int n = queue.pop();
            if(not queue.has_element(path[n].par)){
                valid = domain_check(n, dist);
                if(not valid) break;
                valid = relax(n, dist);
            }
            iter++;
        }

        if(not queue.is_empty() && valid){
            timeout = true;
            valid = false;
        }

        return valid;
    }

    bool BfdcAlg::valid_domain(int i, int value) const{
        int dix = vid[i];
        return instance.var_domains.at(i)[dix].contains(-value);
    }

    void BfdcAlg::change_domain_arcs(int i, const Domain& dom){
        instance.rgraph.update_arc_weight(instance.node_zero, i, -dom.lb);
    }

    bool BfdcAlg::update_domain(int i, int& value){
        const auto& vdom_i = instance.var_domains.at(i);
        vid[i]++;
        scans++;
        while(vid[i] < static_cast<int>(vdom_i.size())){
            if(-value <= vdom_i[vid[i]].ub){
                value = std::min(value, -vdom_i[vid[i]].lb);
                return true;
            }
            vid[i]++;
        }
        return false;
    }

    bool BfdcAlg::domain_check(int i, std::vector<int>& dist){
        if(i == instance.node_zero) return true;
        bool valid = true;
        if(not valid_domain(i, dist[i])){
            int di = dist[i];
            valid = update_domain(i, di);
            if(di != dist[i]){
                dist[i] = di;
                path[i].len = 1; //now it is leaving node 0 directly
                path[i].par = instance.node_zero;
            }
        }
        return valid;
    }

    bool BfdcAlg::relax(int i, std::vector<int>& dist){
        static long count = 0;
        count++;
        scans++;

        for(const Arc& a : instance.rgraph.arcs[i]){
            int j = a.target;
            int w = a.weight;

            int dj = dist[i] + w;
            if(dj < dist[j]){
                dist[j] = dj;

                path[j].len = path[i].len + 1;
                path[j].par = i;
                if(path[j].len >= static_cast<int>(instance.num_variables())) return false;

                if(not queue.has_element(j)){
                    queue.push(j);
                }
            }
        }

        return true;
    }

    long BfdcAlg::get_num_scans() const{
        return scans;
    }

    bool BfdcAlg::timedout() const{
        return timeout;
    }

}
