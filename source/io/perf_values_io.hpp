#ifndef H_SDTP_PERF_VALUES_IO_H
#define H_SDTP_PERF_VALUES_IO_H

#include <chrono>
#include <ctime>
#include "jfields.hpp"
#include "../util/strfmt.hpp"
#include "../util/rapidjson/reader.h"
#include "../util/rapidjson/error/en.h"
#include "../util/rapidjson/writer.h"
#include "../util/rapidjson/stringbuffer.h"
#include "../common/instance.hpp"
#include "perf_values.hpp"
#include <random>
#include <algorithm>

namespace sdtp{
    class PerfValuesIo{
    public:
        PerfValuesIo(std::string filename_){
            filename = filename_;
        }

        bool write(const Instance& instance, const std::vector<PerfValues>& values);

    private:
        std::string filename;
        std::string msg;
        bool error = false;

        void write_instance_info(rapidjson::Writer<rapidjson::StringBuffer>& writer, const Instance& instance);
        void write_values(rapidjson::Writer<rapidjson::StringBuffer>& writer, const PerfValues& val);
        void write_date(rapidjson::Writer<rapidjson::StringBuffer>& writer);
    };
}
#endif //H_SDTP_PERF_VALUES_IO_H
