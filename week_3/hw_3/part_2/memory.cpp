#include <iostream>
#include <string>

struct Trade {
    std::string symbol;
    double price;

    Trade(const std::string& s, double p) : symbol(s), price(p) {
        std::cout << "Trade created: " << symbol << "\n";
    }

    ~Trade() {
        std::cout << "Trade destroyed: " << symbol << "\n";
    }
};

int main() {
    Trade* t1 = new Trade("AAPL", 150.0);
    Trade* t2 = new Trade("GOOG", 2800.0);
    delete t1;
    // delete t1; ❌ we should delete this line because calling delete twice on the same pointer
    // will cause a segmentation fault.

    Trade* t3 = new Trade("MSFT", 300.0);
    // t3 = new Trade("TSLA", 750.0); ❌ The memory allocated for the "MSFT" trade needs to be freed first, 
    // otherwise it will result in a memory leak.
    delete t3;
    t3 = new Trade("TSLA", 750.0); 

    Trade* trades = new Trade[3] {
        {"NVDA", 900.0},
        {"AMZN", 3200.0},
        {"META", 250.0}
    };
    // delete trades; ❌ Using delete directly will typically call the destructor for only one element rather than the entire array, 
    // thus leaving the remaining objects without proper cleanup and creating a memory leak.
    delete[] trades;

    return 0;
}

// g++ memory.cpp -o memory -std=c++20