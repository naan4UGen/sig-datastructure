#include<vector>
#include<iostream>

template<typename T, typename Comp = std::less<T>>
class binary_min_heap{
    std::vector<T> v;
    Comp comp;
    std::size_t _size;

    std::size_t left(std::size_t i){
        return 2 * i + 1;
    }
    std::size_t right(std::size_t i){
        return 2 * i + 2;
    }
    std::size_t parent(std::size_t i){
        return (i - 1) / 2;
    }

public:
    binary_min_heap(Comp _cmp = Comp{}) : v(), comp(_cmp), _size(){}

    std::size_t size() const {
        return _size;
    }

    bool empty() const {
        return !_size;
    }

    void up_heapify(std::size_t idx){
        std::size_t par = parent(idx);
        while(idx && comp(v[idx], v[par])){
            std::swap(v[idx], v[par]);
            std::swap(idx, par);
            par = parent(idx);
        }
    }

    void push(const T& _val){
        v.emplace_back(_val);
        up_heapify(_size++);
    }

    void down_heapify(std::size_t idx){
        std::size_t j = left(idx);
        while(j < _size){
            if(auto i = right(idx); i < _size){
                std::size_t tmp;
                tmp = comp(v[i], v[j]) ? i : j;
                if(comp(v[tmp], v[idx])){
                    std::swap(v[tmp], v[idx]);
                    std::swap(tmp, idx);
                }else break;
            }else{
                if(comp(v[j], v[idx])){
                    std::swap(v[j], v[idx]);
                    std::swap(j, idx);
                }else break;
            }
            j = left(idx);
        }
    }

    T pop(){
        std::swap(v[0], v[--_size]);
        down_heapify(0);
        auto ret = v.back();
        v.pop_back();
        return ret;
    }

    void debug_print(){
        unsigned long t{1}, c{0};
        while(c < _size){
            for(unsigned long i{c}; i < std::min(_size, c + t); ++i)printf("%3lu%s", v[i], std::string(64 / t - 3, ' ').data());
            std::cout << std::endl;
            c += t;
            t *= 2;
        }
        std::cout << std::endl;
    }

};

#include<numeric>
#include<random>
#include<algorithm>

int main(){
    binary_min_heap<unsigned long> pq;
    std::vector<unsigned long> hoge(20);
    std::iota(begin(hoge), end(hoge), 0UL);
    std::shuffle(begin(hoge), end(hoge), std::mt19937_64{std::random_device{}()});
    for(const auto& i : hoge){
        pq.push(i);
        pq.debug_print();
    }
    while(!pq.empty())std::cout << pq.pop() << std::endl;
    return 0;
}
