#include "perf_values_io.hpp"
namespace sdtp{
    bool PerfValuesIo::write(const Instance& instance, const std::vector<PerfValues>& values){
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

        writer.Key(jfields::perf::VALUES.c_str());
        writer.StartArray();
        for(const PerfValues& val : values){
            writer.StartObject();
            write_values(writer, val);
            writer.EndObject();
        }
        writer.EndArray();

        //write_date(writer);

        writer.EndObject();

        fprintf(pfile, "%s", sb.GetString());
        fclose(pfile);

        return true;
    }

    void PerfValuesIo::write_instance_info(rapidjson::Writer<rapidjson::StringBuffer>& writer, const Instance& instance){
        writer.Key(jfields::perf::INSTANCE.c_str());
        writer.String(instance.name.c_str());
        writer.Key(jfields::perf::INST_NUM_VARS.c_str());
        writer.Uint(instance.num_variables()-1);
        writer.Key(jfields::perf::INST_NUM_CONS.c_str());
        writer.Uint(instance.num_constraints());
        writer.Key(jfields::perf::INST_VAR_DISJ.c_str());
        writer.Uint(instance.num_vars_multi_domains());
        writer.Key(jfields::perf::INST_MAX_DISJ.c_str());
        writer.Uint(instance.max_multi_dom());
        writer.Key(jfields::perf::INST_SUM_DISJ.c_str());
        writer.Uint(instance.sum_multi_domains());
    }

    void PerfValuesIo::write_values(rapidjson::Writer<rapidjson::StringBuffer>& writer, const PerfValues& val){
        writer.Key(jfields::perf::PV_METHOD.c_str());
        writer.String(val.name.c_str());
        writer.Key(jfields::perf::PV_SCANS.c_str());
        writer.Double(val.scans);
        writer.Key(jfields::perf::PV_CACHE_HITS.c_str());
        writer.Uint64(val.cache.hits);
        writer.Key(jfields::perf::PV_CACHE_MISSES.c_str());
        writer.Uint64(val.cache.misses);
        writer.Key(jfields::perf::PV_L1D_HITS.c_str());
        writer.Uint64(val.l1d.hits);
        writer.Key(jfields::perf::PV_L1D_MISSES.c_str());
        writer.Uint64(val.l1d.misses);
        writer.Key(jfields::perf::PV_LL_READ_HITS.c_str());
        writer.Uint64(val.llc_read.hits);
        writer.Key(jfields::perf::PV_LL_READ_MISSES.c_str());
        writer.Uint64(val.llc_read.misses);
        writer.Key(jfields::perf::PV_LL_WRITE_HITS.c_str());
        writer.Uint64(val.llc_read.hits);
        writer.Key(jfields::perf::PV_LL_WRITE_MISSES.c_str());
        writer.Uint64(val.llc_read.misses);
    }

    void PerfValuesIo::write_date(rapidjson::Writer<rapidjson::StringBuffer>& writer){
        auto cur_time = std::chrono::system_clock::now();
        std::time_t ct = std::chrono::system_clock::to_time_t(cur_time);
        std::string str_date = std::string(std::ctime(&ct));
        str_date.pop_back();
        writer.Key(jfields::perf::DATE.c_str());
        writer.String(str_date.c_str());
    }
}
