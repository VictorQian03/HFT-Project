//
// Created by Amy Wang on 4/22/25.
//

#ifndef METAPROGRAMMING_H
#define METAPROGRAMMING_H

template<int N> struct Factorial {
    static constexpr int value = N* Factorial<N-1>::value;
};
template<> struct Factorial<0> { static constexpr int value = 1; };


template<int N> struct Fibonacci {
    static constexpr int value = Fibonacci<N-2>::value+ Fibonacci<N-1>::value;
};
template<> struct Fibonacci<1> { static constexpr int value = 1; };
template<> struct Fibonacci<0> { static constexpr int value = 0; };


//Implement a TMP-based IsOdd<T> trait, and use SFINAE to enable a print_if_odd(T) function only if the value is odd.
//Use std::enable_if, std::is_integral, etc.
template<int N>
struct IsOdd {
    static constexpr bool value = (N % 2 != 0);
};

template<int N, typename std::enable_if<
    IsOdd<N>::value && std::is_integral<decltype(N)>::value, int>::type = 0>
void print_if_odd() {
    std::cout << N << " is odd.\n";
}

template<typename... Args>
void print_all(const Args&... args) {
    ((std::cout << args << " "), ...) << std::endl; // Fold expression version
}

#endif //METAPROGRAMMING_H



