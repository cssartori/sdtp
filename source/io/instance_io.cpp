#include "instance_io.hpp"
#include "../util/strfmt.hpp"

namespace sdtp{
    bool InstanceIo::read(Instance& instance){
        std::ifstream file(filename);
        if(not file.is_open()){
            msg = "Failed to open instance file '" + filename + "'.";
            error = true;
            return false;
        }

        InHandler handler(instance);
        std::string content( (std::istreambuf_iterator<char>(file) ),
                             (std::istreambuf_iterator<char>()    ) );
        rapidjson::StringStream ss(content.c_str());

        if(not reader.Parse(ss, handler)){
            error = true;
            size_t o = reader.GetErrorOffset();
            msg = "Error at offset " + std::to_string(o) + " near '" + std::string(content).substr(o, 15) + "'.";
            msg += "\n" + handler.msg;
            return false;
        }
        return true;
    }

    bool InstanceIo::write(const Instance& instance, bool status, std::string name, std::string comment, bool shuff, std::mt19937* rng){
        rapidjson::StringBuffer s;
        rapidjson::Writer<rapidjson::StringBuffer> writer(s);

        FILE* pfile = fopen(filename.c_str(), "w");
        if(pfile == NULL){
            msg = "Failed to open file '" + filename + "'";
            error = true;
            return false;
        }

        writer.StartObject();
        writer.Key(jfields::instance::NAME.c_str());
        writer.String(name.c_str());
        writer.Key(jfields::instance::NUM_VARIABLES.c_str());
        writer.Uint(instance.num_variables()-1);
        writer.Key(jfields::instance::NUM_CONSTRAINTS.c_str());
        writer.Uint(instance.num_constraints());
        writer.Key(jfields::instance::STATUS.c_str());
        writer.String(status ? "feasible" : "infeasible");
        writer.Key(jfields::instance::COMMENT.c_str());
        writer.String(comment.c_str());

        writer.Key(jfields::instance::VARIABLES.c_str());
        writer.StartArray();
        for(size_t i=1;i<instance.num_variables();i++){
            writer.StartObject();
            writer.Key(jfields::instance::VAR_ID.c_str());
            writer.Uint(i);
            writer.Key(jfields::instance::VAR_DOMAIN_SIZE.c_str());
            writer.Uint(instance.variable_domain_size(i));
            writer.Key(jfields::instance::VAR_DOMAINS.c_str());
            writer.StartArray();
            for(const Domain& dom : instance.var_domains[i]){
                writer.StartObject();
                writer.Key(jfields::instance::VAR_DOM_LOW.c_str());
                writer.Int(dom.lb);
                writer.Key(jfields::instance::VAR_DOM_UP.c_str());
                writer.Int(dom.ub);
                writer.EndObject();
            }
            writer.EndArray();
            writer.EndObject();
        }
        writer.EndArray();

        writer.Key(jfields::instance::CONSTRAINTS.c_str());

        std::vector<std::string> jcons;
        for(size_t i=1;i<instance.num_variables();i++){
            for(const Arc& a : instance.rgraph.arcs[i]){
                if(a.target == instance.node_zero) continue;
                std::string con = util::str::format("%d %d %d", a.source, a.target, a.weight);
                jcons.push_back(con);
            }
        }

        if(shuff){
            std::shuffle(jcons.begin(), jcons.end(), *rng);
        }

        writer.StartArray();
        for(const std::string& sc : jcons){
            writer.String(sc.c_str());
        }
        writer.EndArray();
        writer.EndObject();

        fprintf(pfile, "%s", s.GetString());
        fclose(pfile);

        return true;
    }

    std::string InstanceIo::get_message() const{
        return msg;
    }

    bool InstanceIo::has_error() const{
        return error;
    }


    bool InstanceIo::InHandler::Null() {
        if(state == States::IGNORE){
            state = States::START;
            return true;
        }
        return false;
    }


    bool InstanceIo::InHandler::Bool(bool b) {
        if(state == States::IGNORE){
            state = States::START;
            return true;
        }
        return false;
    }

    bool InstanceIo::InHandler::Int(int i) {
        if(state == States::IGNORE){
            state = States::START;
            return true;
        }
        if(state == States::VAR_ID){
            var_id = i;
            state = States::VAR;
        }else if(state == States::DOM_L){
            dom.lb = i;
            state = States::DOM;
        }else if(state == States::DOM_U){
            dom.ub = i;
            state = States::DOM;
        }else if(state == States::DOM_SIZE){
            domsz = i;
            state = States::VAR;
        }else if(state == States::NUM_VARS){
            num_vars = i;
            state = States::START;
        }else if(state == States::NUM_CONS){
            num_cons = i;
            state = States::START;
        }
        return true;
    }

    bool InstanceIo::InHandler::Uint(unsigned u) {
        if(state == States::IGNORE){
            state = States::START;
            return true;
        }
        if(state == States::VAR_ID){
            var_id = u;
            state = States::VAR;
        }else if(state == States::DOM_L){
            dom.lb = u;
            state = States::DOM;
        }else if(state == States::DOM_U){
            dom.ub = u;
            state = States::DOM;
        }else if(state == States::DOM_SIZE){
            domsz = u;
            state = States::VAR;
        }else if(state == States::NUM_VARS){
            num_vars = u;
            state = States::START;
        }else if(state == States::NUM_CONS){
            num_cons = u;
            state = States::START;
        }
        return true;
    }

    bool InstanceIo::InHandler::Int64(int64_t i) {
        if(state == States::IGNORE){
            state = States::START;
            return true;
        }
        if(state == States::VAR_ID){
            var_id = i;
            state = States::VAR;
        }else if(state == States::DOM_L){
            dom.lb = i;
            state = States::DOM;
        }else if(state == States::DOM_U){
            dom.ub = i;
            state = States::DOM;
        }else if(state == States::DOM_SIZE){
            domsz = i;
            state = States::VAR;
        }else if(state == States::NUM_VARS){
            num_vars = i;
            state = States::START;
        }else if(state == States::NUM_CONS){
            num_cons = i;
            state = States::START;
        }
        return true;
    }

    bool InstanceIo::InHandler::Uint64(uint64_t u) {
        if(state == States::IGNORE){
            state = States::START;
            return true;
        }
        if(state == States::VAR_ID){
            var_id = u;
            state = States::VAR;
        }else if(state == States::DOM_L){
            dom.lb = u;
            state = States::DOM;
        }else if(state == States::DOM_U){
            dom.ub = u;
            state = States::DOM;
        }else if(state == States::DOM_SIZE){
            domsz = u;
            state = States::VAR;
        }else if(state == States::NUM_VARS){
            num_vars = u;
            state = States::START;
        }else if(state == States::NUM_CONS){
            num_cons = u;
            state = States::START;
        }
        return true;
    }

    bool InstanceIo::InHandler::Double(double d) {
        if(state == States::IGNORE){
            state = States::START;
            return true;
        }
        return false;
    }

    bool InstanceIo::InHandler::RawNumber(const char* str, rapidjson::SizeType length, bool copy) {
        if(state == States::IGNORE){
            state = States::START;
            return true;
        }
        return false;
    }

    bool InstanceIo::InHandler::String(const char* str, rapidjson::SizeType length, bool copy) {
        if(state == States::IGNORE){
            state = States::START;
            return true;
        }else if(state == States::CONSTRAINTS){
            rconstraints = true;
            std::string con(str);
            //std::cout << "Matching con " << con << std::endl;
            boost::regex rgx("(\\d+) (\\d+) (-?\\d+)");
            boost::smatch match;
            bool m = boost::regex_match(con, match, rgx);
            if(not m) return false;

            int v1 = std::atoi(match.str(1).c_str());
            int v2 = std::atoi(match.str(2).c_str());
            int w = std::atoi(match.str(3).c_str());

            constrs.push_back(Arc(v1, v2, w));
        }else if (state == States::NAME){
            std::string name(str);
            instance.name = name;
            state = States::START;
        }else if (state == States::STATUS){
            std::string status(str);
            if(status == "feasible") instance.feasible = true;
            else instance.feasible = false;
            state = States::START;
        }
        return true;
    }

    bool InstanceIo::InHandler::StartObject() {
        if(state == States::VARIABLES){
            //start reading one var
            state = States::VAR;
        }else if(state == States::VAR_DOM){
            state = States::DOM;
        }
        return true;
    }

    bool InstanceIo::InHandler::Key(const char* str, rapidjson::SizeType length, bool copy) {
        std::string key(str);
        if(key == jfields::instance::VARIABLES){
            if(state != States::START) return false;
            state = States::VARIABLES;
            rvariables = true;
        }else if(key == jfields::instance::VAR_ID){
            if(state != States::VAR) return false;
            state = States::VAR_ID;
        }else if(key == jfields::instance::VAR_DOMAINS){
            if(state != States::VAR) return false;
            state = States::VAR_DOM;
            domains.clear();
        }else if(key == jfields::instance::VAR_DOM_LOW){
            if(state != States::DOM) return false;
            state = States::DOM_L;
        }else if(key == jfields::instance::VAR_DOM_UP){
            if(state != States::DOM) return false;
            state = States::DOM_U;
        }else if(key == jfields::instance::VAR_DOMAIN_SIZE){
            if(state != States::VAR) return false;
            state = States::DOM_SIZE;
        }else if(key == jfields::instance::NAME){
            if(state != States::START) return false;
            state = States::NAME;
            rname = true;
        }else if(key == jfields::instance::CONSTRAINTS){
            if(state != States::START) return false;
            state = States::CONSTRAINTS;
        }else if(key == jfields::instance::STATUS){
            if(state != States::START) return false;
            state = States::STATUS;
            rstatus = true;
        }else if(key == jfields::instance::NUM_VARIABLES){
            if(state != States::START) return false;
            state = States::NUM_VARS;
        }else if(key == jfields::instance::NUM_CONSTRAINTS){
            if(state != States::START) return false;
            state = States::NUM_CONS;
        }else{
            if(state != States::START) return false;
            state = States::IGNORE;
        }
        return true;
    }

    bool InstanceIo::InHandler::EndObject(rapidjson::SizeType memberCount) {
        if(state == States::DOM){
            state = States::VAR_DOM;
            domains.push_back(dom);
        }else if(state == States::VAR){
            //ending a variable
            if(domains.size() != domsz){
                msg = "Error. Domain size for variable " + std::to_string(var_id) + " does not match informed size in instance " + std::to_string(domsz) + " != " + std::to_string(domains.size());
                return false;
            }
            int x = instance.declare_variable(domains);
            if(x == def::NO_NODE) return false;
            else if(x != var_id){
                //printf("Incorrect variable declaration!\n");
                msg = "Variable " + std::to_string(var_id) + " could not be declared.";
                return false;
            }
            state = States::VARIABLES;
        }else if(state == States::START){
            //ending all reading
            if(not rvariables){
                msg = "JSON does not contain a 'variables' field which is required.";
                return false;
            }else if(not rconstraints){
                msg = "JSON does not contain a 'constraints' field which is required.";
                return false;
            }else if(not rstatus){
                msg = "JSON does not contain a 'status' field which is required.";
                return false;
            }else if(not rname){
                msg = "JSON does not contain a 'name' field which is required.";
                return false;
            }

            //declare constraints
            for(const Arc& a : constrs){
                int v1 = a.source;
                int v2 = a.target;
                int w = a.weight;
                if(instance.has_constraint(v1, v2)){
                    printf("ERROR. Double constraint between %d and %d\n", v1, v2);
                    exit(-1);
                }

                instance.declare_constraint(v1, v2, w);
            }
            constrs.clear();

            if((long)instance.num_variables()-1 != num_vars){
                printf("Number of variables declared in instance differ from those in defined in the file: %ld != %ld\n", (long)instance.num_variables()-1, num_vars);
                msg = "Number of variables declared in instance differ from those in defined in the file: " + std::to_string((long)instance.num_variables()-1) + " != " + std::to_string(num_vars);
                return false;
            }else if((long)instance.num_constraints() != num_cons){
                msg = "Number of constraints declared in instance differ from those in defined in the file: " + std::to_string((long)instance.num_constraints()) + " != " + std::to_string(num_cons);
                return false;
            }
        }
        return true;
    }

    bool InstanceIo::InHandler::StartArray() {
        return true;
    }

    bool InstanceIo::InHandler::EndArray(rapidjson::SizeType elementCount) {
        if(state == States::CONSTRAINTS){
            state = States::START;
        }else if(state == States::VAR_DOM){
            state = States::VAR;
        }else if(state == States::VARIABLES){
            state = States::START;
        }
        return true;
    }
}
