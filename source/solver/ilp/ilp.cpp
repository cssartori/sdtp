#include "ilp.hpp"
#include <cmath>

namespace sdtp::ilp{
    //needed by CPLEX
    ILOSTLBEGIN

    Ilp::Ilp(const Instance& instance_) : instance(instance_), h(env){
        name = def::ALG_ILP_NAME;
        this->model = new IloModel(env);
        build_model();
        timeout = false;
    }

    Ilp::~Ilp(){
        if(cplex != nullptr) cplex->end();
        delete cplex;
        delete model;
    }

    void Ilp::build_model(){
        for(size_t i=1;i<instance.num_variables();i++){
            this->h.add(IloNumVar(this->env, instance.var_domains[i].front().lb, instance.var_domains[i].back().ub));
            this->y.push_back(IloIntVarArray(this->env, instance.var_domains[i].size(), 0, 1));
        }

        for(size_t i=1;i<instance.num_variables();i++){
            for(const Arc& a : instance.rgraph.arcs[i]){
                if(a.target == instance.node_zero) continue;
                this->model->add(h[a.source-1] - h[a.target-1] <= a.weight);
            }
        }

        for(size_t i=1;i<instance.num_variables();i++){
            IloExpr sumy_i(env);
            int min_lb = instance.var_domains[i].front().lb;
            int max_lb = instance.var_domains[i].back().lb;
            int MLB = (max_lb - min_lb);
            int min_ub = instance.var_domains[i].front().ub;
            int max_ub = instance.var_domains[i].back().ub;
            int MUB = (max_ub - min_ub);
            size_t k = 0;
            for(const Domain& dom : instance.var_domains[i]){
                this->model->add(dom.lb <= h[i-1] + MLB*(1-y[i-1][k]));
                this->model->add(h[i-1] <= dom.ub + MUB*(1-y[i-1][k]));
                sumy_i += y[i-1][k];
                k++;
            }

            this->model->add(sumy_i == 1);
        }
    }

    const Instance& Ilp::get_instance() const{
        return instance;
    }

    std::string Ilp::get_name() const{
        return name;
    }

    long Ilp::get_runtime_microseconds() const{
        return timer.elapsed_microseconds();
    }

    long Ilp::get_runtime_milliseconds() const{
        return timer.elapsed_milliseconds();
    }

    bool Ilp::timedout() const{
        return timeout;
    }

    bool Ilp::run(long time_limit){
        cplex = new IloCplex(*model);

        timer.start();
        bool result;
        cplex->setParam(IloCplex::Param::Threads, 1);
        cplex->setOut(env.getNullStream());
        cplex->setWarning(env.getNullStream());
        if(time_limit < def::MAX_LONG){
            double tl_sec = time_limit/1e6;
            cplex->setParam(IloCplex::Param::TimeLimit, tl_sec);
        }

        cplex->solve();

        if(cplex->getStatus() == IloAlgorithm::Feasible
           || cplex->getStatus() == IloAlgorithm::Optimal){
            result = true;
            timeout = false;
        }else if (cplex->getStatus() == IloAlgorithm::Infeasible){
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

    void Ilp::extract_solution(std::vector<int>& sol) const{
        sol.push_back(0);
        for(size_t i=1;i<instance.num_variables();i++){
            sol.push_back(static_cast<int>(std::round(cplex->getValue(h[i-1]))));
        }
    }

    void Ilp::reset(){
        cplex->end();
        delete cplex;
        cplex = nullptr;
        timeout = false;
    }
}
