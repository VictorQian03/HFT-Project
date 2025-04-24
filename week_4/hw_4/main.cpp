

#include <iostream>
#include <iterator>
#include "metaprogramming.h"
#include "constexpr_math.h"
#include "StaticVector.h"
#include "generic_algorithms.h"
#include "OrderBookBuffer.h"

struct Order {
    int    id;
    double price;
    int    qty;
    Order(int i = 0, double p = 0.0, int q = 0) : id(i), price(p), qty(q) {}
    friend std::ostream& operator<<(std::ostream& os, const Order& o) {
        return os << "Order{id=" << o.id << ", price=" << o.price << ", qty=" << o.qty << "}";
    }
};

int main() {
    std::cout << "🚀 HFT Template Homework Starter\n";

    // TODO: Fill with examples from each header

    static_assert(Factorial<5>::value == 120, "Correct factorial");
    static_assert(Fibonacci<7>::value == 13, "Correct fibonacci");

    print_if_odd<3>;
    //print_if_odd<4>;

    print_all("hey",3.1453,42555,true,"no");

    static_assert(factorialConstexpr(5)==120,"constexpr factorial good implementation");
    static_assert(fibonacciConstexpr(7)==13,"constexpr fibonacci good implementation");

    static_assert(price_bucket(101.73) == 101.70,"price bucket works");
    static_assert(square(5)==25,"square function is compiled correctly");
    constexpr int Size = square(5);
    int arr[Size];
    std::cout << "Array of size " << Size << " created.\n";


    StaticVector<Order, 10> orders;
    orders.push_back({1,  95.50,  7});
    orders.push_back({2,102,10});
    orders.push_back({3,96.2,40});
    orders.push_back({4,120,4});



    auto price_gt_100 = [](const Order& o){ return o.price > 100.0; };

    std::cout << "Orders with price > 100:\n";
    for (auto it = find_if(orders.begin(), orders.end(), price_gt_100);
         it != orders.end();
         it  = find_if(std::next(it), orders.end(), price_gt_100))
    {
        std::cout << "Order id " << (*it).id << " with prices "<<(*it).price<< '\n';  
    }

    auto qtydivisibleby10 = [](const Order& o){ return o.qty % 10==0; };
    auto it2 = find_if(orders.begin(), orders.end(), qtydivisibleby10);
    if (it2 != orders.end()) {
        std::cout << "First order with qty divisible by 10:\n"
                  << "Order id " << it2->id
                  << " with qty "  << it2->qty << "\n";
    }

    std::cout << "=== Stack Allocated, No Lock ===\n";
    OrderBookBuffer<Order, StackAllocator<Order>, NoLock> stackBook(5);
    stackBook.add_order({1, 101.5, 10});
    stackBook.add_order({2, 102.2, 5});
    stackBook.print_orders();

    std::cout << "\n=== Heap Allocated, Thread Safe ===\n";
    OrderBookBuffer<Order, HeapAllocator<Order>, MutexLock> heapBook(3);
    heapBook.add_order({10, 99.8, 8});
    heapBook.add_order({11, 100.0, 12});
    heapBook.print_orders();

    return 0;
}
