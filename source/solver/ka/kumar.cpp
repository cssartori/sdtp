#include "kumar.hpp"
#include "dinic_alg.hpp"
#include "dist_alg_ja.hpp"
#include <algorithm>

namespace sdtp::ka{

    KumarAlg::KumarAlg(Instance instance_, TypeDa type_) : instance((instance_.add_max_bound_constraints(), instance_)), kbf_alg(instance, vdi), type(type_){
        if(type == TypeDa::BELLMAN_FORD) name = def::ALG_KAB_NAME;
        else name = def::ALG_KAJ_NAME;

        dist.assign(instance.num_variables(), std::vector<int>(instance.num_variables(), def::MAX_INT));
        vdi.assign(instance.num_variables(), -1);
        sol.assign(instance.num_variables(), def::MAX_INT);
        intervals.reserve(instance.num_domains());
        timeout = false;
    }

    bool KumarAlg::run(long time_limit){
        timer.start();
        bool feas = solve(time_limit);
        timer.stop();
        return feas;
    }

    const Instance& KumarAlg::get_instance() const{
        return instance;
    }

    std::string KumarAlg::get_name() const{
        return name;
    }

    long KumarAlg::get_num_scans() const{
        return 0; //no scans here
    }

    void KumarAlg::extract_solution(std::vector<int>& sol_) const{
        sol_ = sol;
    }

    long KumarAlg::get_runtime_microseconds() const{
        return timer.elapsed_microseconds();
    }

    long KumarAlg::get_runtime_milliseconds() const{
        return timer.elapsed_milliseconds();
    }

    bool KumarAlg::timedout() const{
        return timeout;
    }

    void KumarAlg::reset(){
        intervals.clear();
        dist.assign(instance.num_variables(), std::vector<int>(instance.num_variables(), def::MAX_INT));
        vdi.assign(instance.num_variables(), -1);
        sol.assign(instance.num_variables(), def::MAX_INT);
        intervals.reserve(instance.num_domains());
        timeout = false;
        mfg.clear();
    }

    bool KumarAlg::has_time(long time_limit){
        if(timer.exceeds_us(time_limit)){
            timeout = true;
            return false;
        }
        return true;
    }

    bool KumarAlg::solve(long time_limit){
        util::PreciseTimer timer;
        timer.start();

        bool feas = true;

        if(type == TypeDa::BELLMAN_FORD){
            DistAlgBf dbf(instance, timer);
            feas = dbf.run_apsp(time_limit, dist);
        }else{
            DistAlgJa dja(instance, dist, timer);
            feas = dja.run(time_limit);
        }

        if(not has_time(time_limit)) return false;
        else if(not feas) return false;

        feas = prepare_intervals(time_limit);
        if(not feas) return false;

        std::vector< std::pair<int,int> > conf2;
        feas = define_size2_conflicts(conf2, time_limit);
        if(not feas) return false;

        if(not has_time(time_limit)) return false;

        DinicAlg dinic(mfg);
        dinic.run();

        if(not has_time(time_limit)) return false;

        //at this point mfg is the residual graph Gf
        std::vector<bool> reach(mfg.nodes.size(), false);
        bfs_reacheable_from(mfg.source, reach);

        std::vector<int> q;
        feas = create_sets(reach, q);
        if(not feas) return false;

        //solve the resulting STP
        feas = kbf_alg.solve(sol);

        return feas;
    }

    bool KumarAlg::prepare_intervals(long time_limit){
        for(int i=1;i<(int)instance.num_variables();i++){
            int k=0;
            int added = 0;

            for(const Domain& dom : instance.var_domains[i]){
                k++;
                //the two if's below remove size-1 conflicts
                if(dist[i][instance.node_zero] + dom.ub < 0) continue;
                if(dist[instance.node_zero][i] - dom.lb < 0) continue;

                intervals.push_back(Interval(i, k-1, dom, 1)); //NOTE: intervals with weight = 0 will never be used in the max-flow
                mfg.add_node((int)intervals.size()-1, 1);
                added++;
            }

            if(added == 0){
                //trivially infeasible, a variable has no valid interval
                return false;
            }
        }

        if(not has_time(time_limit)) return false;

        //create complete max-flow graph
        mfg.duplicate_nodes();
        mfg.add_source_and_sink();

        return true;
    }

    bool KumarAlg::define_size2_conflicts(std::vector< std::pair<int,int> >& conf2, long time_limit){
        for(int i=0;i<(int)intervals.size();i++){
            if(not has_time(time_limit)) return false;
            for(int j=i+1;j<(int)intervals.size();j++){
                int vi = intervals[i].v;
                int vj = intervals[j].v;
                if(vi == vj){
                    mfg.add_conflict_arc(std::pair<int,int>(i,j));
                }else{
                    int a = 0;
                    if(intervals[i].ub + dist[vi][vj] - intervals[j].lb < 0){
                        mfg.add_conflict_arc(std::pair<int,int>(i,j));
                        a++;
                    }
                    if(intervals[j].ub + dist[vj][vi] - intervals[i].lb < 0){
                        mfg.add_conflict_arc(std::pair<int,int>(j,i));
                        a++;
                    }
                    if(a == 2){
                        //NOTE: this should never happen!
                        exit(-1);
                    }
                }
            }
        }
        return true;
    }

    void KumarAlg::bfs_reacheable_from(int source, std::vector<bool>& reach){
        std::queue<int> q;
        q.push(source);

        int c =0;
        while(q.size() > 0){
            int i = q.front();
            q.pop();
            if(reach[i]) continue;
            reach[i] = true;
            for(const auto& arc : mfg.arcs[i]){
                if(arc.residual_capacity() > 0 && not reach[arc.to]){
                    if(i == source || arc.to == mfg.sink){
                        c++;
                    }
                    q.push(arc.to);
                }
            }
        }
    }

    void KumarAlg::dfs_reacheable_from(int s, std::vector<bool>& reach)
    {
        reach[s] = true;
        for(const MfArc& arc : mfg.arcs[s]){
            int i = arc.to;
            if (arc.residual_capacity() > 0 && !reach[i])
                dfs_reacheable_from(i, reach);
        }
    }

    bool KumarAlg::create_sets(const std::vector<bool>& reach, std::vector<int>& q){
        std::vector<bool> s(intervals.size(), false);

        for(const auto& arc : mfg.arcs[mfg.source]){
            int i = arc.to;
            if(not reach[i]){
                int n = i;
                if(mfg.nodes[i].is_prime()) n = mfg.nodes[i].pair;
                s[n] = true;
            }
        }

        for(const auto& arc : mfg.arcs[mfg.sink]){
            int i = arc.to;
            if(reach[i]){
                int n = i;
                if(mfg.nodes[i].is_prime()) n = mfg.nodes[i].pair;
                s[n] = true;
            }
        }

        for(int n=0;n<(int)intervals.size();n++){
            if(not s[n]){
                q.push_back(n);
                int v = intervals[n].v;
                if(vdi[v] != -1) return false; //trying to associate two intervals to the same variable
                vdi[v] = intervals[n].d;
            }
        }

        return (q.size() == instance.num_variables()-1);
    }

};
