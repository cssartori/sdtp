#ifndef H_SDTP_RESULT_IO_H
#define H_SDTP_RESULT_IO_H

#include <chrono>
#include <ctime>
#include "jfields.hpp"
#include "../util/strfmt.hpp"
#include "../util/rapidjson/reader.h"
#include "../util/rapidjson/error/en.h"
#include "../util/rapidjson/writer.h"
#include "../util/rapidjson/stringbuffer.h"
#include "../common/instance.hpp"
#include "result.hpp"
#include <random>
#include <algorithm>

namespace sdtp{
    class ResultIo{
    public:
        ResultIo(std::string filename_){
            filename = filename_;
        }

        bool write(const Instance& instance, const Result& result, const std::vector<int>& values);
        bool write(const Instance& instance, const std::vector<Result>& results, std::string message);

    private:
        std::string filename;
        std::string msg;
        bool error = false;

        void write_instance_info(rapidjson::Writer<rapidjson::StringBuffer>& writer, const Instance& instance);
        void write_result(rapidjson::Writer<rapidjson::StringBuffer>& writer, const Result& res);
        void write_date(rapidjson::Writer<rapidjson::StringBuffer>& writer);
    };
}
#endif //H_SDTP_RESULT_IO_H
