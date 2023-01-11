#include "cp.hpp"

namespace sdtp::cp{
    //needed by CPLEX
    ILOSTLBEGIN

    Cp::Cp(const Instance& instance_, bool simp_) : instance(instance_), h(env), simp(simp_){
        this->name = simp ? def::ALG_SCP_NAME : def::ALG_CP_NAME;
        this->model = new IloModel(env);
        build_model();
    }

    Cp::~Cp(){
        if(cp != nullptr) cp->end();
        delete cp;
        delete model;
    }

    const Instance& Cp::get_instance() const{
        return instance;
    }

    std::string Cp::get_name() const{
        return name;
    }

    long Cp::get_runtime_microseconds() const{
        return timer.elapsed_microseconds();
    }

    long Cp::get_runtime_milliseconds() const{
        return timer.elapsed_milliseconds();
    }

    long Cp::get_num_scans() const{ return 0; }

    bool Cp::timedout() const{
        return timeout;
    }

    bool Cp::run(long time_limit){
        bool result;
        timeout = false;
        cp = new IloCP(*model);

        timer.start();
        cp->setParameter(IloCP::Workers, 1);
        if(time_limit < def::MAX_LONG){
            double tl_sec = time_limit/1e6;
            cp->setParameter(IloCP::TimeLimit, tl_sec);
        }

        cp->setOut(env.getNullStream());
        cp->setWarning(env.getNullStream());
        result = cp->solve();
        if(cp->getStatus() == IloAlgorithm::Feasible || cp->getStatus() == IloAlgorithm::Optimal){
            result = true;
            timeout = false;
        }else if(cp->getStatus() == IloAlgorithm::Infeasible){
            result = false;
            timeout = false;
        }else{
            //timeout?
            result = false;
            timeout = true;
        }

        timer.stop();
        return result;
    }

    void Cp::extract_solution(std::vector<int>& sol) const{
        sol.assign(instance.num_variables(), 0);
        for(int i=1;i<(int)instance.num_variables();i++){
            sol[i] = static_cast<int>(std::round(cp->getValue(h[i-1])));
        }
    }

    void Cp::reset(){
        timeout = false;
        cp->end();
        delete cp;
        cp = nullptr;
    }

    void Cp::build_model(){
        for(int i=1;i<(int)instance.num_variables();i++){
            int lb = instance.var_domains[i].front().lb;
            int ub = instance.var_domains[i].back().ub;
            h.add(IloIntVar(env, lb, ub));

            if(simp){
                IloIntArray iva(env); //invalid assignments to h[i]
                for(int k=1;k<(int)instance.var_domains[i].size();k++){
                    int a = instance.var_domains[i][k-1].ub+1;
                    while(a < instance.var_domains[i][k].lb){
                        iva.add(a);
                        a++;
                    }
                }
                this->model->add(IloForbiddenAssignments(env, h[i-1], iva));
            }else{
                IloConstraint ci = (instance.var_domains[i].front().lb <= h[i-1] && h[i-1] <= instance.var_domains[i].front().ub); //valid assignments to h[i]
                for(int k=1;k<(int)instance.var_domains[i].size();k++){
                    const Domain& dom = instance.var_domains[i][k];
                    ci = ci || (dom.lb <= h[i-1] && h[i-1] <= dom.ub);
                }
                this->model->add(ci);
            }
        }

        for(size_t i=1;i<instance.num_variables();i++){
            for(const Arc& a : instance.rgraph.arcs[i]){
                if(a.target == instance.node_zero) continue;
                this->model->add(h[a.source-1] - h[a.target-1] <= a.weight);
            }
        }
    }
}
