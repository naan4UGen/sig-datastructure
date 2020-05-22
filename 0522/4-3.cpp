#include<vector>
#include<memory>
#include<random>
#include<cstdio>
#include<iostream>

class height_picker{
    std::mt19937_64 mt;
public:
    height_picker() : mt(std::random_device{}()) {}
    std::size_t operator()(){
        std::size_t r{1};
        while(mt() & 1)++r;
        return r;
    }
};

template<typename T>
class skiplist{
    height_picker gen_height;
    std::size_t _size;

    struct skiplist_node{
        T val;
        std::size_t height;
        std::vector<std::size_t> length;
        std::vector<std::shared_ptr<skiplist_node>> next;
        skiplist_node(const T& _v, std::size_t _h, std::size_t sz) : val(_v), height(_h), length(height, sz), next(height, nullptr) {}
        skiplist_node(const skiplist_node&) = default;
        skiplist_node(skiplist_node&&) noexcept = default;
        void update_height(std::size_t h, std::size_t sz){
            if(height < h){
                this -> height = h;
                length.resize(h, sz);
                next.resize(h, nullptr);
            }
        }
        void pop(){
            --height;
            length.pop_back();
            next.pop_back();
        }
    };

    skiplist_node top;

public:
    skiplist() : _size(0), top(0, 0, 0), gen_height() {}
    skiplist(std::size_t sz, const T& v) : _size(0), top(0, 0, 0), gen_height() {
        for(std::size_t i{0}; i < sz; ++i)insert(0, v);
    }

    skiplist_node& find_pred(std::size_t i){
        auto u = std::ref(top);
        ++i;
        std::size_t now{0};
        for(std::size_t r{u.get().height + 1}; r--; ){
            while(u.get().next[r] && now + u.get().length[r] < i){
                now += u.get().length[r];
                u = *u.get().next[r];
            }
        }
        return u.get();
    }

    const T& operator[](std::size_t i) const& {
        return find_pred(i).next[0] -> val;
    }
    T& operator[](std::size_t i) & {
        return find_pred(i).next[0] -> val;
    }
    T operator[](std::size_t i) const&& {
        return std::move(find_pred(i).next[0] -> val);
    }

    size_t size() const noexcept {
        return this -> _size;
    }

    void insert(std::size_t i, const T& v){
        auto val{std::make_shared<skiplist_node>(v, gen_height(), ++_size)};
        top.update_height(val -> height, _size);

        auto u = std::ref(top);
        ++i;
        std::size_t now{0};
        for(std::size_t r{u.get().height}; r--; ){
            while(u.get().next[r] && now + u.get().length[r] < i){
                now += u.get().length[r];
                u = *u.get().next[r];
            }
            ++u.get().length[r];
            if(r < val -> height){
                val -> next[r].swap(u.get().next[r]);
                u.get().next[r] = val;
                val -> length[r] = u.get().length[r] - (i - now);
                u.get().length[r] = (i - now);
            }
        }
    }

    void erase(std::size_t i){
        auto u = std::ref(top);
        ++i;
        std::size_t now{0}, moved{0};
        for(std::size_t r{u.get().height}; r--; ){
            while(u.get().next[r] && now + u.get().length[r] < i){
                now += u.get().length[r];
                u = *u.get().next[r];
                moved = 1;
            }
            --u.get().length[r];
            if(now + u.get().length[r] + 1 == i && u.get().next[r]){
                u.get().length[r] += u.get().next[r] -> length[r];
                u.get().next[r] = u.get().next[r] -> next[r];
                if(!moved && !u.get().next[r])u.get().pop();
            }
        }
        --_size;
    }

    void debug_print(){
        for(std::size_t r{top.height}; r--; ){
            auto u = std::cref(top);
            printf("|%s", std::string(6 * (u.get().length[r] - 1), ' ').data());
            while(u.get().next[r]){
                u = std::cref(*u.get().next[r]);
                printf(" [%3lu]%s", u.get().val, std::string(6 * (u.get().length[r] - 1), ' ').data());
            }
            puts(" |");
        }puts("");
    }

    void listed_print(){
        auto u = std::cref(top);
        while(u.get().next[0]){
            u = std::cref(*u.get().next[0]);
            std::cout << u.get().val << " ";
        }
        std::cout << std::endl;
    }
};

int main(){
    using namespace std;
    height_picker hoge{};
    skiplist<unsigned long> skl;
    auto mt{mt19937_64{random_device{}()}};
    for(unsigned long i{0}; i < 24; ++i){
        skl.insert(uniform_int_distribution<unsigned long>{0, skl.size()}(mt), i);
        skl.debug_print();
    }
    for(unsigned long i{0}; i < 24; ++i){
        skl.erase(uniform_int_distribution<unsigned long>{0, skl.size() - 1}(mt));
        skl.debug_print();
    }

    for(unsigned long i{0}; i < 100000; ++i)skl.insert(i, i);
    unsigned long ans{0};
    for(unsigned long i{0}; i < 100000; ++i)ans += skl[i];
    cout << ans << endl;
}
