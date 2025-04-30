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