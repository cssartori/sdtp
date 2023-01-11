#include "apsp_alg.hpp"


namespace sdtp::ult{

    ApspAlg::ApspAlg(const UltModel& model_, std::vector< std::vector<int> >& m_, util::PreciseTimer& timer_) : model(model_), graph(model.graph), matrix(m_), timer(timer_){
        matrix[0][0] = 0;
        for(size_t i=1;i<graph.num_nodes();i++){
            matrix[0][i] = model.instance.var_domains[i].back().ub;
        }

        for(size_t i=1;i<graph.num_nodes();i++){
            for(const Arc& a : graph.arcs[i]){
                matrix[i][a.target] = a.weight;
            }

            matrix[i][i] = 0;
        }
    }

    bool ApspAlg::solve(long time_limit){
        scans = 0;
        for(size_t k=0;k<graph.num_nodes();k++){
            for(size_t i=0;i<graph.num_nodes();i++){
                scans++;
                if(timer.exceeds_us(time_limit)) return false;
                for(size_t j=0;j<graph.num_nodes();j++){
                    if(matrix[i][j] > matrix[i][k] + matrix[k][j]){
                        matrix[i][j] = matrix[i][k] + matrix[k][j];
                        if(i == j){
                            return false; //negative cycle found
                        }
                    }
                }
            }
        }

        return true;
    }

    void ApspAlg::reset_matrix(){
        for(size_t i=0;i<graph.num_nodes();i++){
            for(size_t j=0;j<graph.num_nodes();j++){
                int w = def::MAX_INT;
                if(graph.has_arc(i, j)) w = graph.get_arc(i, j).weight;
                matrix[i][j] = w;
            }
            matrix[i][i] = 0;
        }
    }

    long ApspAlg::get_num_scans() const{
        return scans;
    }
}
