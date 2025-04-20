# Phase 3: Build a Local Order Book and Core Trading Infrastructure

#### Team members: Amy Wang, Victor Qian, Jonathan Ran, Robert Xing

### Architecture Overview
Our system works by first reading in a new market update and creating a snapshot that holds key information such as price, quantity, and side (buy or sell). This is managed by our ```MarketSnapshot``` class, which also keeps track of the current best bid and ask on the market by maintaining two ordered maps that map prices to `PriceLevel` objects that contain the price and quantity of the level, in addition to being able to update existing bids/asks through the ```update_bid``` and `update_ask` methods.

After processing the event, our strategy loop responds to changes in the current price levels on the market and places or cancels orders accordingly. If an existing bid/ask was updated in the most recent event, old orders for that price level are cancelled, and a new one is placed through the `OrderManager::resolved_conflicts` method. Otherwise, we check if the most recent event resulted in a new best bid/ask, and we place an order for a sell/buy accordingly.

The `OrderManager` class maintains what outstanding orders we currently have and the degree to which they have been filled through the `MyOrder` struct, which contains information about the order, including the Order `id` and a status (New, Filled, PartiallyFilled, Cancelled), as well as a map from each `id` to the `MyOrder` object associated with it.

In the event one of our outstanding orders is filled, through the `OrderManager::handle_fill` method, we update that order's status and only remove it from our map of outstanding orders if it is completely filled.

### Memory Management
Our only uses for dynamically-allocated memory are in our maps for bids and asks in `MarketSnapshot` and the map of orders in `OrderManager`. In both cases, we use `unique_ptr` to create smart pointers to a specific, distinct `PriceLevel` or `MyOrder` object. Shared ownership is not allowed among these objects, as each one represents a unique entity on the market or in our order book. When no longer needed, such as when a bid/ask is removed from the market or an order is completely filled/cancelled, we use `std::map:erase()` to remove it from  our maps, and the space is automatically freed from the heap, as we no longer have any outstanding pointers to the object. All objects are created/destroyed through these interfaces, and no manual `new` or `delete` calls are used.

### How to run
Navigate to the `hft-project/week_3/phase_3` directory and run `make`.

Use `make clean` to clean the build files.

### Verifying Correctness + Safety
Our sample data that we used to feed market input to our system can be viewed in the `sample_feed.txt` file in this directory, which also contains some comments about the expected behavior of our system for every event. The actual output of our system is logged in `trading.log`, which includes both the market events as well as the actions of our system, such as order placements/cancellations, as well as the updated list of all active orders after every event.

For checking memory safety, we use the `-fsanitize=address` flag for our debug build, which can be compiled with `make debug`. Under various simulated scenarios, we have not recorded any memory leaks while using this tool.



