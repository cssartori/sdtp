#include "ult_alg.hpp"


namespace sdtp::ult{

    UltAlg::UltAlg(const Instance& instance_) :
        instance(instance_),
        model(instance),
        matrix(std::vector< std::vector<int> >(model.graph.num_nodes(), std::vector<int>(model.graph.num_nodes(), def::MAX_INT))),
        apsp(model, matrix, timer)
    {
        name = def::ALG_ULT_NAME;
        timeout = false;
        scans = 0;
    }

    const Instance& UltAlg::get_instance() const{
        return instance;
    }

    std::string UltAlg::get_name() const{
        return name;
    }

    void UltAlg::extract_solution(std::vector<int>& sol) const{
        sol = matrix[instance.node_zero]; //currently returning the latest solution

        //to return the earliest solution:
        // sol.assign(instance.num_variables(), 0);
        // for(int i=1;i<(int)instance.num_variables();i++){
        //     sol[i] = -matrix[i][instance.node_zero];
        // }
    }

    long UltAlg::get_runtime_microseconds() const{
        return timer.elapsed_microseconds();
    }

    long UltAlg::get_runtime_milliseconds() const{
        return timer.elapsed_milliseconds();
    }


    bool UltAlg::compute_n2(long time_limit){
        bool v = apsp.solve(time_limit);
        scans += apsp.get_num_scans();
        return v;
    }

    int UltAlg::compute_n3(){
        int count = 0;
        for(const std::pair<int,int>& con : model.con_list){
            int i = con.first;
            int j = con.second;
            int lb = -matrix[j][i];
            int ub = matrix[i][j];

            scans++;
            bool ch = model.intersect_con_domain(con.first, con.second, Domain(lb,ub));

            if(ch && model.con_domains[i][j].size() == 0){
                return -1; //empty domain means no solution
            }else if(ch){
                //to avoid having to reset matrix when computing APSP (re-use computations)
                matrix[i][j] = std::min(matrix[i][j], model.con_domains[i][j].back().ub);
                matrix[j][i] = std::min(matrix[j][i], -model.con_domains[i][j].front().lb);
                count++;
            }
        }
        return count;
    }

    bool UltAlg::run(long time_limit){
        timer.start();
        bool feas = true;
        bool done = false;
        size_t iter = 0;
        while(feas && not done && not timer.exceeds_us(time_limit)){
            iter++;
            feas = compute_n2(time_limit);
            if(not feas) break;
            int r = compute_n3();
            if(r < 0) feas = false;
            else if(r == 0) done = true;
        }

        if(timer.exceeds_us(time_limit)){
            timeout = true;
            feas = false;
        }

        timer.stop();
        if(feas && done) return true;

        return false;
    }

    void UltAlg::reset(){
        model.reset();
        matrix.assign(model.graph.num_nodes(), std::vector<int>(model.graph.num_nodes(), def::MAX_INT));

		matrix[0][0] = 0;
        for(size_t i=1;i<model.graph.num_nodes();i++){
            matrix[0][i] = instance.var_domains[i].back().ub;
        }

        for(size_t i=1;i<model.graph.num_nodes();i++){
            for(const Arc& a : model.graph.arcs[i]){
                matrix[i][a.target] = a.weight;
            }
            matrix[i][i] = 0;
        }
        scans = 0;
    }

    long UltAlg::get_num_scans() const{
        return scans;
    }

    bool UltAlg::timedout() const{
        return timeout;
    }
}
