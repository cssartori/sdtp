#ifndef H_SDTP_UTIL_NARY_HEAP_H
#define H_SDTP_UTIL_NARY_HEAP_H

#include <vector>
#include <cstdlib>
#include <limits>

namespace sdtp::util{
    class NaryHeap{
    protected:
        struct HeapElem{
            int value;
            int key;

            HeapElem(int v=-1, int k=-1){
                value = v;
                key = k;
            }
        };

        struct CompareHeapElem{
            bool operator()(const HeapElem& he1, const HeapElem& he2) const{
                return he1.key > he2.key; //min-heap
            }
        };

    private:
        inline static const size_t NO_POSITION = std::numeric_limits<size_t>::max();
        std::vector<HeapElem> elements;
        std::vector<size_t> position;
        size_t size_;
        size_t n;

        void heapify_up_it(size_t i){
            while(i != 0){
                size_t pi = (i-1)/n;
                int k_pi = elements[pi].key;
                int k_i = elements[i].key;

                if(k_i < k_pi){
                    position[elements[i].value] = pi;
                    position[elements[pi].value] = i;
                    std::swap(elements[i], elements[pi]);

                    i = pi;
                }else i = 0;
            }
        }

        void heapify_down_it(size_t i){
            while(true){
                size_t min_i = i;
                size_t first_child = n*i+1;
                for(size_t j=first_child;j<size() && j-first_child < n;j++){
                    if(elements[j].key < elements[min_i].key) min_i = j;
                }

                if(min_i == i) break;

                position[elements[min_i].value] = i;
                position[elements[i].value] = min_i;
                std::swap(elements[min_i], elements[i]);

                i = min_i;
            }
        }


    public:
        NaryHeap(size_t dimension, int arity=2){
            elements.assign(dimension, HeapElem());
            position.assign(dimension, NO_POSITION);
            size_ = 0;
            n = static_cast<size_t>(arity);
        }

        void insert(int elem, int key){
            elements[size_].value = elem;
            elements[size_].key = key;
            position[elem] = size_;
            size_++;
            heapify_up_it(size_-1);
        }

        void update_key(int elem, int new_key){
            size_t i = position[elem];
            int old_key = elements[i].key;
            elements[i].key = new_key;
            if(new_key < old_key){
                heapify_up_it(i);
            }else{
                heapify_down_it(i);
            }
        }

        void pop(){
            if(size() >= 1){
                position[elements[0].value] = NO_POSITION;
                std::swap(elements[0],elements[size_-1]);
                position[elements[0].value] = 0;
                size_--;

                if(size() > 1){
                    heapify_down_it(0);
                }
            }
        }

        void reset(){
            size_ = 0;
        }

        bool empty() const{
            return (size_ == 0);
        }

        int top() const{
            return elements[0].value;
        }

        int top_key() const{
            return elements[0].key;
        }

        size_t size() const{
            return size_;
        }

        bool has_element(int elem) const{
            return (position[elem] != NO_POSITION);
        }

    };
}

#endif // H_SDTP_UTIL_NARY_HEAP_H
