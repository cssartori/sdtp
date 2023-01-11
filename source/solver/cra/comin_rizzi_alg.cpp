#include "comin_rizzi_alg.hpp"

namespace sdtp::cra{
    CominRizziAlg::CominRizziAlg(const Instance& instance_) : instance(instance_), bfa(instance), dijkstra(instance.rgraph, t0, DijkstraAlg::Type::SANDERS){
        name = def::ALG_CRA_NAME;
        t0.assign(instance.num_variables(), -def::MAX_INT);

        dist.assign(instance.num_variables(), std::vector<int>());
        scans = 0;
        timeout = false;
        vdi.assign(instance.num_variables(), 0);
        compd.resize(instance.num_variables(), false);
        linf_pointers.assign(instance.num_variables(), linf.end()); //initially all pointers are invalid
        reachable_from.assign(instance.num_variables(), std::vector<int>());
    }

    void CominRizziAlg::reset(){
        t0.assign(instance.num_variables(), -def::MAX_INT);

        dist.assign(instance.num_variables(), std::vector<int>());
        vdi.assign(instance.num_variables(), 0);
        compd.reset();
        linf.clear();
        linf_pointers.assign(instance.num_variables(), linf.end()); //initially all pointers are invalid
        reachable_from.assign(instance.num_variables(), std::vector<int>());
        scans = 0;
        timeout = false;
        dijkstra.reset();
    }

    const Instance& CominRizziAlg::get_instance() const{
        return instance;
    }

    std::string CominRizziAlg::get_name() const{
        return name;
    }

    void CominRizziAlg::extract_solution(std::vector<int>& sol) const{
        sol = t;
    }

    long CominRizziAlg::get_runtime_microseconds() const{
        return timer.elapsed_microseconds();
    }

    long CominRizziAlg::get_runtime_milliseconds() const{
        return timer.elapsed_milliseconds();
    }

    bool CominRizziAlg::run(long time_limit){
        timer.start();
        bool feas = solve(time_limit);
        timer.stop();

        return feas;
    }

    bool CominRizziAlg::solve(long time_limit){
        bool feas = bfa.solve(t0);
        scans += bfa.get_num_scans();

        if(not feas) return false;
        long dk_calls = 0;
        t = t0;
        for(size_t i=1;i<instance.num_variables();i++){
            if(not instance.var_domains[i][0].contains(t0[i])){
                int r = update_domain_index(i);
                if(r == 0) return false;
                else if(r == 2){
                    linf.push_back(i);
                    linf_pointers[i] = std::prev(linf.end());
                }
            }
        }

        if(linf.size() == 0) return true;

        int iter = 0;
        while(feas && linf.size() > 0 && not timer.exceeds_us(time_limit)){
            iter++;
            int x = linf.front();
            linf.pop_front();
            linf_pointers[x] = linf.end();

            if(not compd[x]){
                dk_calls++;
                //lazy evaluation of dijkstra to improve empirical performance
                dist[x] = std::vector<int>(instance.num_variables(), def::MAX_INT); //allocate as needed
                dijkstra.run(x, dist[x], reachable_from[x]);
                compd[x] = true;
            }
            feas = update_rules(x);
        }

        scans += dijkstra.get_num_scans();

        if(feas && linf.size()){
            //stopped because of timeout
            timeout = true;
            feas = false;
        }

        return feas;
    }


    int CominRizziAlg::update_domain_index(int i){
        scans++;
        for(int d=vdi[i];d<static_cast<int>(instance.var_domains[i].size());d++){
            if(instance.var_domains[i][d].contains(t[i])){
                vdi[i] = d;
                return 1;
            }else if(t[i] < instance.var_domains[i][d].lb){
                vdi[i] = d;
                return 2;
            }
        }
        return 0;
    }


    bool CominRizziAlg::update_rules(int x){
        int t_x = t[x];
        int li = instance.var_domains[x][vdi[x]].lb;
        scans++;

        for(int i : reachable_from[x]){
            int dx_i = dist[x][i] + (t[i] - t0[i]) - (t_x - t0[x]);
            t[i] = t[i] + std::max(0, li - t_x - dx_i);

            if(linf_pointers[i] == linf.end() && not instance.var_domains[i][vdi[i]].contains(t[i])){
                int r = update_domain_index(i);
                if(r == 0) return false;
                else if(r == 2){
                    linf.push_back(i);
                    linf_pointers[i] = std::prev(linf.end());
                }
            }else if(linf_pointers[i] != linf.end() && instance.var_domains[i][vdi[i]].contains(t[i])){
                linf.erase(linf_pointers[i]);
                linf_pointers[i] = linf.end();
            }else if(linf_pointers[i] != linf.end()){
                int r = update_domain_index(i);
                if(r == 1){
                    linf.erase(linf_pointers[i]);
                    linf_pointers[i] = linf.end();
                }else if(r == 0) return false;
            }
        }
        return true;
    }

    long CominRizziAlg::get_num_scans() const{
        return scans;
    }

    bool CominRizziAlg::timedout() const{
        return timeout;
    }
}
