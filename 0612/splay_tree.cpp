#include<memory>
#include<vector>
#include<iostream>
#include<string>

template<typename T, typename Compare = std::less<>>
class splay_tree_set{
    using key_type = T;
    using size_type = unsigned long;
    using key_compare = Compare;
    struct splay_tree_set_node{
        key_type val;
        size_type _size;
        std::unique_ptr<splay_tree_set_node> left, right;
        explicit splay_tree_set_node(const key_type& v) : val(v), _size(1), left(nullptr), right(nullptr) {}
        void debug_print(std::ostream& os, size_type depth = 0){
            std::string s;
            s += "[" + std::to_string(val) + ", " + std::to_string(_size) + "]";
            if(left)left -> debug_print(os, depth + s.size() + 1);
            for(size_type i{0}; i < depth; ++i)os << " ";
            os << s << std::endl;
            if(right)right -> debug_print(os, depth + s.size() + 1);
        }
        void debug_insert(const key_type& v, const key_compare& cmp){
            if(cmp(val, v)){
                if(right)right -> debug_insert(v, cmp);
                else right = std::make_unique<splay_tree_set_node>(v);
            }else{
                if(left)left -> debug_insert(v, cmp);
                else left = std::make_unique<splay_tree_set_node>(v);
            }
            recalc();
        }
        size_t recalc(){
            _size = 1;
            if(left)_size += left -> _size;
            if(right)_size += right -> _size;
            return _size;
        }
    };
    using pointer_type = std::unique_ptr<splay_tree_set_node>;

    pointer_type top;
    key_compare cmp;
public:
    size_type size() const noexcept {
        if(top)return top -> _size;
        return 0;
    }
    void splay(const key_type& x){
        if(!top)return;
        pointer_type ltop{nullptr}, rtop{nullptr};
        std::vector<pointer_type*> lr{&ltop}, rl{&rtop};
        while(top && (cmp(top -> val, x) || cmp(x, top -> val))){
            if(cmp(top -> val, x)){
                if(!top -> right)break;
                if(cmp(top -> right -> val, x) && top -> right -> right){
                    auto& tr = top -> right;
                    auto& trl = tr -> left;
                    auto& trr = tr -> right;
                    top.swap(trr);
                    trr.swap(*lr.back());
                    lr.back()->swap(tr);
                    tr.swap(trl);
                    lr.push_back(&((*lr.back()) -> left));
                    lr.push_back(&((*lr[lr.size() - 2]) -> right));
                }else if(cmp(x, top -> right -> val) && top -> right -> left){
                    auto& tr = top -> right;
                    auto& trl = tr -> left;
                    auto& trr = tr -> right;
                    top.swap(trl);
                    trl.swap(*rl.back());
                    rl.back()->swap(tr);
                    tr.swap(*lr.back());
                    rl.push_back(&((*rl.back()) -> left));
                    lr.push_back(&((*lr.back()) -> right));
                }else{
                    auto& tr = top -> right;
                    top.swap(tr);
                    tr.swap(*lr.back());
                    lr.push_back(&((*lr.back()) -> right));
                }
            }else{
                if(!top -> left)break;
                if(cmp(x, top -> left -> val) && top -> left -> left){
                    auto& tl = top -> left;
                    auto& tlr = tl -> right;
                    auto& tll = tl -> left;
                    top.swap(tll);
                    tll.swap(*rl.back());
                    rl.back()->swap(tl);
                    tl.swap(tlr);
                    rl.push_back(&((*rl.back()) -> right));
                    rl.push_back(&((*rl[rl.size() - 2]) -> left));
                }else if(cmp(top -> left -> val, x) && top -> left -> right){
                    auto& tl = top -> left;
                    auto& tlr = tl -> right;
                    auto& tll = tl -> left;
                    top.swap(tlr);
                    tlr.swap(*lr.back());
                    lr.back()->swap(tl);
                    tl.swap(*rl.back());
                    lr.push_back(&((*lr.back()) -> right));
                    rl.push_back(&((*rl.back()) -> left));
                }else{
                    auto& tl = top -> left;
                    top.swap(tl);
                    tl.swap(*rl.back());
                    rl.push_back(&((*rl.back()) -> left));
                }
            }
        }
        if(ltop){
            auto &tl = top -> left;
            tl.swap(ltop);
            ltop.swap(*lr.back());
        }
        if(rtop){
            auto& tr = top -> right;
            tr.swap(rtop);
            rtop.swap(*rl.back());
        }
        while(!lr.empty()){
            if((*lr.back()))(*lr.back()) -> recalc();
            lr.pop_back();
        }
        while(!rl.empty()){
            if((*rl.back()))(*rl.back()) -> recalc();
            rl.pop_back();
        }
        if(top -> left)top -> left -> recalc();
        if(top -> right)top -> right -> recalc();
        top -> recalc();
    }
    splay_tree_set() : top(nullptr), cmp() {}
    explicit splay_tree_set(pointer_type&& _top) : top(std::move(_top)), cmp() {}
    void erase(const key_type& x){
        if(!top)return;
        splay(x);
        if(cmp(top -> val, x) || cmp(x, top -> val))return;
        if(top -> left && top -> right){
            splay_tree_set tmp_left{std::move(top -> left)}, tmp_right{std::move(top -> right)};
            tmp_left.splay(x);
            tmp_left.top -> right.swap(tmp_right.top);
            top.swap(tmp_left.top);
        }else if(top -> left){
            splay_tree_set tmp_left{std::move(top -> left)};
            top.swap(tmp_left.top);
        }else{
            splay_tree_set tmp_right{std::move(top -> right)};
            top.swap(tmp_right.top);
        }
    }
    void debug_print(std::ostream& os){
        os << std::string(50, '-') << std::endl;
        if(top)top -> debug_print(os, 0);
        os << std::string(50, '-') << std::endl;
        os << std::endl;
    }
    void insert(const key_type& t){
        splay(t);
        if(top && top -> val == t)return;
        if(top)top -> debug_insert(t, cmp);
        else top = std::make_unique<splay_tree_set_node>(t);
        splay(t);
    }
};

int main(){
    using namespace std;
    splay_tree_set<unsigned long> s;
    unsigned long N;
    cin >> N;
    for(unsigned long i{0}, A; i < N; ++i){
        cin >> A;
        s.insert(A);
    }
    puts(s.size() == N ? "YES": "NO");
}
