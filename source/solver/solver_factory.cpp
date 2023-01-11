#include "solver_factory.hpp"
#include "bfdc/bfdc_alg.hpp"
#include "cra/comin_rizzi_alg.hpp"
#include "ilp/ilp.hpp"
#include "cp/cp.hpp"
#include "ult/ult_alg.hpp"
#include "rult/red_ult_alg.hpp"
#include "ka/kumar.hpp"

namespace sdtp{

    Solver* SolverFactory::create(std::string solver_name) const{
        Solver* solver = nullptr;
        if(solver_name == def::ALG_CRA_NAME){
            solver = new cra::CominRizziAlg(instance);
        }else if(solver_name == def::ALG_KAB_NAME){
            solver = new ka::KumarAlg(instance, ka::KumarAlg::TypeDa::BELLMAN_FORD);
        }else if(solver_name == def::ALG_KAJ_NAME){
            solver = new ka::KumarAlg(instance, ka::KumarAlg::TypeDa::JOHNSON_ALG);
        }else if(solver_name == def::ALG_ULT_NAME){
            solver = new ult::UltAlg(instance);
        }else if(solver_name == def::ALG_RULT_NAME){
            solver = new rult::RedUltAlg(instance);
        }else if(solver_name == def::ALG_BFDC_NAME){
            solver = new bfdc::BfdcAlg(instance);
        }else if(solver_name == def::ALG_ILP_NAME){
            solver = new ilp::Ilp(instance);
        }else if(solver_name == def::ALG_CP_NAME){
            solver = new cp::Cp(instance, false);
        }else if(solver_name == def::ALG_SCP_NAME){
            solver = new cp::Cp(instance, true);
        }

        return solver;
    }

    bool SolverFactory::solver_exists(std::string solver_name){
        bool exists = false;
        if(solver_name == def::ALG_CRA_NAME){
            exists = true;
        }else if(solver_name == def::ALG_KAB_NAME){
            exists = true;
        }else if(solver_name == def::ALG_KAJ_NAME){
            exists = true;
        }else if(solver_name == def::ALG_ULT_NAME){
            exists = true;
        }else if(solver_name == def::ALG_RULT_NAME){
            exists = true;
        }else if(solver_name == def::ALG_BFDC_NAME){
            exists = true;
        }else if(solver_name == def::ALG_ILP_NAME){
            exists = true;
        }else if(solver_name == def::ALG_CP_NAME){
            exists = true;
        }else if(solver_name == def::ALG_SCP_NAME){
            exists = true;
        }

        return exists;
    }

}
