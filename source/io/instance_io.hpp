#ifndef H_SDTP_INSTANCE_IO_H
#define H_SDTP_INSTANCE_IO_H

#include "../common/instance.hpp"
#include "../util/rapidjson/reader.h"
#include "../util/rapidjson/error/en.h"
#include "../util/rapidjson/writer.h"
#include "../util/rapidjson/stringbuffer.h"
#include "jfields.hpp"
#include <fstream>
#include <boost/regex.hpp>
#include <iostream>
#include <random>

namespace sdtp{

    class InstanceIo{
    public:
        InstanceIo(std::string filename_){
            filename = filename_;
        }
        bool read(Instance& instance);
        bool write(const Instance& instance, bool status, std::string name, std::string comment, bool shuff, std::mt19937* rng=nullptr);
        std::string get_message() const;
        bool has_error() const;

    private:
        std::string filename;
        std::string msg;
        bool error = false;
        rapidjson::Reader reader;
        struct InHandler;
        std::vector< Arc > cons;

        struct InHandler {
            enum class States{ START=0, IGNORE,
                               VARIABLES,
                               VAR,
                               VAR_ID,
                               VAR_DOM,
                               DOM,
                               DOM_U,
                               DOM_L,
                               DOM_SIZE,
                               NAME,
                               CONSTRAINTS,
                               STATUS,
                               NUM_VARS,
                               NUM_CONS
            };
            std::string msg;
            Instance& instance;
            States state = States::START;
            std::vector<Domain> domains;
            Domain dom;
            size_t domsz;
            int var_id;
            long num_vars;
            long num_cons;
            bool rvariables;
            bool rconstraints;
            bool rname;
            bool rstatus;
            std::vector< Arc > constrs;

            InHandler(Instance& instance_) : instance(instance_) {
                rvariables = false;
                rconstraints = false;
                rname = false;
                rstatus = false;
                num_vars = 0;
                num_cons = 0;
            }

            bool Null();
            bool Bool(bool b);
            bool Int(int i);
            bool Uint(unsigned u);
            bool Int64(int64_t i);
            bool Uint64(uint64_t u);
            bool Double(double d);
            bool RawNumber(const char* str, rapidjson::SizeType length, bool copy);
            bool String(const char* str, rapidjson::SizeType length, bool copy);
            bool StartObject();
            bool Key(const char* str, rapidjson::SizeType length, bool copy);
            bool EndObject(rapidjson::SizeType memberCount);
            bool StartArray();
            bool EndArray(rapidjson::SizeType elementCount);
        };
    };
}

#endif //H_SDTP_INSTANCE_IO_H
