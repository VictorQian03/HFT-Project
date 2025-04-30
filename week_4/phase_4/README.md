## Build Instructions

1.  **Build Steps**:
    # Build the project
    ```bash
    make
    ```
    # Clean up the project
    ```bash
    make clean
    ```
    This will create an executable file named `hft_app`. 

## ▶️ How to Run

1.  Execute the application:
    ```bash
    ./hft_app
    ```
    The application will start, and the Market Data Feed Simulator will begin generating ticks, printing them to the console. It will run for a while and then stop.

## to run the test latency cpp
  ```bash
    make clean
    make test
    ```


=== Testing with 1000 ticks ===

Running latency test with aligned market data 1000 ticks...

Aligned marketdata test Statistics (nanoseconds):
Min: 83
Max: 368292
Mean: 1016.34
StdDev: 12836.8
P50: 208
P95: 458
P99: 9167

Running unaligned test with 1000 ticks...

Unaligned MarketData Test Statistics (nanoseconds):
Min: 83
Max: 2625
Mean: 221.359
StdDev: 177.999
P50: 208
P95: 292
P99: 1084

=== Testing with 10000 ticks ===

Running latency test with aligned market data 10000 ticks...

Aligned marketdata test Statistics (nanoseconds):
Min: 83
Max: 138208
Mean: 253.476
StdDev: 1522.91
P50: 208
P95: 333
P99: 1125

Running unaligned test with 10000 ticks...

Unaligned MarketData Test Statistics (nanoseconds):
Min: 83
Max: 66292
Mean: 270.782
StdDev: 761.109
P50: 209
P95: 458
P99: 1250

=== Testing with 100000 ticks ===

Running latency test with aligned market data 100000 ticks...

Aligned marketdata test Statistics (nanoseconds):
Min: 83
Max: 1166042
Mean: 373.413
StdDev: 4323.35
P50: 209
P95: 875
P99: 1458

Running unaligned test with 100000 ticks...

Unaligned MarketData Test Statistics (nanoseconds):
Min: 83
Max: 2303875
Mean: 471.543
StdDev: 7316.21
P50: 250
P95: 1042
P99: 1667