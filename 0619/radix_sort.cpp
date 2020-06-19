#include<type_traits>
#include<vector>
#include<array>
#include<iterator>
#include<numeric>
#include<algorithm>

template<typename RandomAccessIterator>
void radix_sort(RandomAccessIterator begin, RandomAccessIterator end){
    if constexpr (std::is_integral_v<typename RandomAccessIterator::value_type>){

        using integer_type = typename RandomAccessIterator::value_type;
        unsigned long now_shift = 0, byte_size = sizeof(integer_type);

        std::vector<integer_type> tmp(end - begin);

        for(unsigned long i{0}; i < byte_size; ++i){
            std::array<unsigned long, 256> count{{}};

            for(auto it = begin; it != end; ++it)++count[(*it >> now_shift) & 255];

            std::partial_sum(std::begin(count), std::end(count), std::begin(count));

            for(auto it = end; it-- != begin; )tmp[--count[(*it >> now_shift) & 255]] = *it;

            std::copy(std::begin(tmp), std::end(tmp), begin);

            now_shift += 8;
        }

        if constexpr (std::is_signed_v<integer_type>)std::rotate(begin, std::partition_point(begin, end, [](auto i){return i >= 0;}), end);

    } else if constexpr (std::is_integral_v<typename RandomAccessIterator::value_type::first_type>){

        using pair_type = typename RandomAccessIterator::value_type;
        using key_type = typename pair_type::second_type;

        unsigned long now_shift = 0, byte_size = sizeof(key_type);

        std::vector<pair_type> tmp(end - begin);

        for(unsigned long i{0}; i < byte_size; ++i){
            std::array<unsigned long, 256> count{{}};

            for(auto it = begin; it != end; ++it)++count[(it -> first >> now_shift) & 255];

            std::partial_sum(std::begin(count), std::end(count), std::begin(count));

            for(auto it = end; it-- != begin; )tmp[--count[(it -> first >> now_shift) & 255]] = *it;

            std::copy(std::begin(tmp), std::end(tmp), begin);

            now_shift += 8;
        }

        if constexpr (std::is_signed_v<key_type>)std::rotate(begin, std::partition_point(begin, end, [](auto i){return i.first >= 0;}), end);

    } else puts("guee radix sort muzukashi.");
}

#include<random>
#include<iostream>
#include<utility>
#include<string>

int main(){
    std::uniform_int_distribution<unsigned long> uld(0, 1000), uid(5, 10);
    std::uniform_int_distribution<char> ucd('a', 'z');
    std::mt19937_64 mt{std::random_device{}()};

    std::vector<long> tap(25);
    std::iota(std::begin(tap), std::end(tap), -12L);
    std::shuffle(std::begin(tap), std::end(tap), mt);

    std::copy(std::begin(tap), std::end(tap), std::ostream_iterator<long>(std::cout, " "));
    std::cout << std::endl;

    radix_sort(std::begin(tap), std::end(tap));

    std::copy(std::begin(tap), std::end(tap), std::ostream_iterator<long>(std::cout, " "));
    std::cout << std::endl;


    std::vector<std::pair<unsigned long, std::string>> nichi(25);
    std::generate(std::begin(nichi), std::end(nichi), [&uld, &uid, &ucd, &mt]() -> std::pair<unsigned long, std::string> {return {uld(mt), std::string(uid(mt), ucd(mt))};});

    for(const auto& [id, str] : nichi)std::cout << "[" << id << ", " << str << "] ";
    std::cout << std::endl;

    radix_sort(std::begin(nichi), std::end(nichi));

    for(const auto& [id, str] : nichi)std::cout << "[" << id << ", " << str << "] ";
    std::cout << std::endl;

}
