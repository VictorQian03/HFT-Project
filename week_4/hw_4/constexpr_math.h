//
// Created by Amy Wang on 4/22/25.
//

#ifndef CONSTEXPR_MATH_H
#define CONSTEXPR_MATH_H
constexpr int factorialConstexpr(int n) {
    return (n <= 1) ? 1 : n * factorialConstexpr(n - 1);
}


constexpr int fibonacciConstexpr(int n) {
    if (n==0) {
        return 0;
    }
    else if (n==1) {
        return 1;
    }
    else {
        return fibonacciConstexpr(n-1)+fibonacciConstexpr(n-2);
    }

};

//Create a constexpr function price_bucket(double price) that rounds a price down to the nearest 0.05 increment at compile time.
constexpr double price_bucket(double price) {
    return static_cast<int>(price / 0.05) * 0.05;
}

constexpr int square(int n) {
    return n*n;
}

#endif //CONSTEXPR_MATH_H
