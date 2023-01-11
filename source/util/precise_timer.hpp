#ifndef H_SDTP_UTIL_PRECISE_TIMER_H
#define H_SDTP_UTIL_PRECISE_TIMER_H

#include <chrono>

namespace sdtp::util{

    struct PreciseTimer{
        typedef std::chrono::steady_clock ClockType;
        std::chrono::time_point<ClockType> start_, end_;
        bool stopped = false;

        void start(){
            this->stopped = false;
            this->start_ = ClockType::now();
        }

        void stop(){
            this->end_ = ClockType::now();
            this->stopped = true;
        }

        bool exceeds_ms(long tms) const{
            std::chrono::time_point<ClockType> end = ClockType::now();
            auto elt = end - start_;
            return elt > std::chrono::milliseconds{tms};
        }

        bool exceeds_us(long tus) const{
            std::chrono::time_point<ClockType> end = ClockType::now();
            auto elt = end - start_;
            return elt > std::chrono::microseconds{tus};
        }

        long int elapsed_microseconds() const{
            std::chrono::time_point<ClockType> end;
            auto elsec = end_ - start_;
            if (not stopped){
                end = ClockType::now();
                elsec = end - start_;
            }
            return std::chrono::duration_cast<std::chrono::microseconds>(elsec).count();
        }

        long int elapsed_milliseconds() const{
            std::chrono::time_point<ClockType> end;
            auto elsec = end_ - start_;
            if (not stopped){
                end = ClockType::now();
                elsec = end - start_;
            }
            return std::chrono::duration_cast<std::chrono::milliseconds>(elsec).count();
        }
    };
}

#endif //H_SDTP_UTIL_PRECISE_TIMER_H
