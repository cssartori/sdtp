#include "solver_perf.hpp"

namespace sdtp{

    struct perf_event_attr SolverPerf::create_pea(uint64_t type, uint64_t conf){
        struct perf_event_attr pea;
        memset(&pea, 0, sizeof(struct perf_event_attr));
        pea.type = type;
        pea.size = sizeof(struct perf_event_attr);
        pea.config = conf;
        pea.disabled = 1;
        pea.exclude_kernel = 1;
        pea.exclude_hv = 1;
        pea.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
        return pea;
    }

    long SolverPerf::create_event(uint64_t type, uint64_t conf, long leader){
        struct perf_event_attr pea = create_pea(type, conf);
        peas.push_back(pea);
        long fd = syscall(__NR_perf_event_open, &peas.back(), 0, -1, leader, 0);
        fds.push_back(fd);
        if(leader == -1) leaders.push_back(fd);
        if (fd == -1) {
            fprintf(stderr, "Error creating config %lu, errno: %d, reason: %s\n", peas.size()-1, errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
        return fd;
    }

    void SolverPerf::setup_measurements(){
        ids.push_back(std::vector<uint64_t>(2,0));
        ids.push_back(std::vector<uint64_t>(2,0));
        ids.push_back(std::vector<uint64_t>(2,0));
        ids.push_back(std::vector<uint64_t>(2,0));

        int c = 0;
        long fd;

        fd = create_event(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES, -1);
        ioctl(fd, PERF_EVENT_IOC_ID, &ids[c][0]);
        fd = create_event(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES, leaders.back());
        ioctl(fd, PERF_EVENT_IOC_ID, &ids[c][1]);

        c++;
        fd = create_event(PERF_TYPE_HW_CACHE, ((PERF_COUNT_HW_CACHE_L1D) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16)), -1);
        ioctl(fd, PERF_EVENT_IOC_ID, &ids[c][0]);
        fd = create_event(PERF_TYPE_HW_CACHE, ((PERF_COUNT_HW_CACHE_L1D) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16)), leaders.back());
        ioctl(fd, PERF_EVENT_IOC_ID, &ids[c][1]);

        c++;
        fd = create_event(PERF_TYPE_HW_CACHE, ((PERF_COUNT_HW_CACHE_LL) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16)), -1);
        ioctl(fd, PERF_EVENT_IOC_ID, &ids[c][0]);
        fd = create_event(PERF_TYPE_HW_CACHE, ((PERF_COUNT_HW_CACHE_LL) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16)), leaders.back());
        ioctl(fd, PERF_EVENT_IOC_ID, &ids[c][1]);

        c++;
        fd = create_event(PERF_TYPE_HW_CACHE, ((PERF_COUNT_HW_CACHE_LL) | (PERF_COUNT_HW_CACHE_OP_WRITE << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16)), -1);
        ioctl(fd, PERF_EVENT_IOC_ID, &ids[c][0]);
        fd = create_event(PERF_TYPE_HW_CACHE, ((PERF_COUNT_HW_CACHE_LL) | (PERF_COUNT_HW_CACHE_OP_WRITE << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16)), leaders.back());
        ioctl(fd, PERF_EVENT_IOC_ID, &ids[c][1]);

    }

    void SolverPerf::read_values(){
        char buf[655360];
        struct read_format* rf = (struct read_format*) buf;

        for(size_t c = 0; c < leaders.size(); c++){
            long leader = leaders[c];
            int err = read(leader, buf, sizeof(buf));
            if(err == -1){
                printf("Error reading!\n");
                exit(EXIT_FAILURE);
            }

            uint64_t refs = 0, miss = 0;
            for (uint64_t i = 0; i < rf->nr; i++) {
                //printf("%lu -> For event ID %lu the counter is %lu\n", c, rf->values[i].id, rf->values[i].value);
                if(rf->values[i].id == ids[c][0]){
                    refs = rf->values[i].value;
                }else if(rf->values[i].id == ids[c][1]){
                    miss = rf->values[i].value;
                }
            }
            if(c == 0){
                values.cache = PerfValue(refs, miss);
            }else if(c == 1){
                values.l1d = PerfValue(refs, miss);
            }else if(c == 2){
                values.llc_read = PerfValue(refs, miss);
            }else if(c == 3){
                values.llc_write = PerfValue(refs, miss);
            }
        }
    }

    void SolverPerf::close_all(){
        for(long fd : fds){
            close(fd);
        }
    }

    void SolverPerf::run(){
        feas = false;
        setup_measurements();

        long scans = 0;
        long count = 0;
        for(long leader : leaders){
            ioctl(leader, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
            ioctl(leader, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);

            feas = solver.run(time_limit);
            scans += solver.get_num_scans();
            count++;
            ioctl(leader, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);
            solver.reset();
        }

        read_values();
        values.scans = (1.0*scans)/(count*solver.get_instance().num_variables());
        close_all();
    }

    PerfValue SolverPerf::get_value(int conf) const{
        if(conf == 0) return values.cache;
        else if(conf == 1) return values.l1d;
        else if(conf == 2) return values.llc_read;
        else if(conf == 3) return values.llc_write;

        return PerfValue();
    }

    PerfValues SolverPerf::get_values() const{
        return values;
    }

}
