#ifndef H_SDTP_UTIL_VEC_QUEUE_H
#define H_SDTP_UTIL_VEC_QUEUE_H

//Implementation of a vector-based queue to be used within the Bellman-Ford algorithm
//this is typically more efficient than the STL version of queue

#include <vector>
#include <cstdint>
#include <limits>

namespace sdtp::util{

    struct VecQueue{
        std::vector<int> elements;
        std::vector<bool> on_queue;
        size_t f,t; //front and tail

        VecQueue(size_t num_e){
            elements.assign(num_e, -1);
            on_queue.resize(num_e, false);
            f=0;
            t=0;
        }

        void reset(){
            f = t = 0;
            on_queue.assign(on_queue.size(), false);
        }

        bool is_empty() const{
            return (f == t);
        }

        int peek() const{
            return elements[f];
        }

        bool has_element(int el) const{
            return on_queue[el];
        }

        void push(int el){
            elements[t++] = el;
            on_queue[el] = true;
            if(t == elements.size()) t = 0;
        }

        int pop(){
            int el = elements[f++];
            on_queue[el] = false;
            if(f == elements.size()) f = 0;
            return el;
        }

        void push_front(int el){
            if(f == 0) f = elements.size()-1;
            else f--;
            elements[f] = el;
            on_queue[el] = true;
        }

        size_t size() const{
            if(t > f) return t-f;
            else return f - t;
        }

        void enlarge_by(size_t num){
            while(num > 0){
                num--;
                elements.push_back(-1);
                on_queue.push_back(false);
            }
        }
    };
}

#endif //H_SDTP_UTIL_VEC_QUEUE_H
