#include "result_io.hpp"

namespace sdtp{
    bool ResultIo::write(const Instance& instance, const Result& result, const std::vector<int>& values){
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        FILE* pfile = fopen(filename.c_str(), "w");
        if(pfile == NULL){
            msg = "Failed to open file '" + filename + "'";
            error = true;
            return false;
        }

        writer.StartObject();

        write_instance_info(writer, instance);

        writer.Key(jfields::log::MESSAGES.c_str());
        writer.StartArray();
        for(const std::string& s : result.msgs){
            writer.String(s.c_str());
        }
        writer.EndArray();

        write_result(writer, result);

        if(result.is_feasible() && result.is_solution_valid()){
            writer.Key(jfields::log::RES_VALUES.c_str());
            writer.StartArray();
            for(size_t i=0;i<values.size();i++){
                writer.Int(values[i]);
            }
            writer.EndArray();
        }

        //write_date(writer);

        writer.EndObject();

        fprintf(pfile, "%s", sb.GetString());
        fclose(pfile);

        return true;
    }

    bool ResultIo::write(const Instance& instance, const std::vector<Result>& results, std::string message){
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        FILE* pfile = fopen(filename.c_str(), "w");
        if(pfile == NULL){
            msg = "Failed to open file '" + filename + "'";
            error = true;
            return false;
        }

        writer.StartObject();

        write_instance_info(writer, instance);

        writer.Key(jfields::log::MESSAGES.c_str());
        writer.String(message.c_str());

        writer.Key(jfields::log::RESULTS.c_str());
        writer.StartArray();
        for(const Result& res : results){
            writer.StartObject();
            write_result(writer, res);
            writer.EndObject();
        }
        writer.EndArray();

        //write_date(writer);

        writer.EndObject();

        fprintf(pfile, "%s", sb.GetString());
        fclose(pfile);

        return true;
    }


    void ResultIo::write_instance_info(rapidjson::Writer<rapidjson::StringBuffer>& writer, const Instance& instance){
        writer.Key(jfields::log::INSTANCE.c_str());
        writer.String(instance.name.c_str());
        writer.Key(jfields::log::INST_NUM_VARS.c_str());
        writer.Uint(instance.num_variables()-1);
        writer.Key(jfields::log::INST_NUM_CONS.c_str());
        writer.Uint(instance.num_constraints());
        writer.Key(jfields::log::INST_VAR_DISJ.c_str());
        writer.Uint(instance.num_vars_multi_domains());
        writer.Key(jfields::log::INST_MAX_DISJ.c_str());
        writer.Uint(instance.max_multi_dom());
        writer.Key(jfields::log::INST_SUM_DISJ.c_str());
        writer.Uint(instance.sum_multi_domains());
    }

    void ResultIo::write_result(rapidjson::Writer<rapidjson::StringBuffer>& writer, const Result& res){
        writer.Key(jfields::log::RES_METHOD.c_str());
        writer.String(res.name.c_str());
        writer.Key(jfields::log::RES_STATUS.c_str());
        std::string status = res.has_error() ? "error" : "ok";
        writer.String(status.c_str());
        writer.Key(jfields::log::RES_FEASIBILITY.c_str());
        writer.StartArray();
        for(bool b : res.feas_flags){
            writer.Bool(b);
        }
        writer.EndArray();

        writer.Key(jfields::log::RES_SOLUTIONS.c_str());
        writer.StartArray();
        for(bool b : res.sol_flags){
            writer.Bool(b);
        }
        writer.EndArray();

        writer.Key(jfields::log::RES_RUNTIMES.c_str());
        writer.StartArray();
        for(long t : res.runtimes){
            writer.Int64(t);
        }
        writer.EndArray();

        writer.Key(jfields::log::RES_TIMEOUTS.c_str());
        writer.StartArray();
        for(bool b : res.timeouts){
            writer.Bool(b);
        }
        writer.EndArray();

        writer.Key(jfields::log::RES_SCANS.c_str());
        writer.StartArray();
        for(double a : res.avg_scans){
            writer.Double(a);
        }
        writer.EndArray();
    }

    void ResultIo::write_date(rapidjson::Writer<rapidjson::StringBuffer>& writer){
        auto cur_time = std::chrono::system_clock::now();
        std::time_t ct = std::chrono::system_clock::to_time_t(cur_time);
        std::string str_date = std::string(std::ctime(&ct));
        str_date.pop_back();
        writer.Key(jfields::log::DATE.c_str());
        writer.String(str_date.c_str());
    }
}
