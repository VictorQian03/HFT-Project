#include "feed_parser.h"
#include "market_snapshot.h"
#include "order_manager.h"
#include "logger.h"
int main() {
    Logger::get_instance()->init("trading.log");

    auto feed = load_feed("sample_feed.txt");
    auto snapshot = MarketSnapshot();
    auto order_manager = OrderManager();

    double prev_best_bid_price = 100.10;  // original bid
    double prev_best_ask_price = 100.20;  // original ask

    for (const auto& event : feed) {
        event.print();

        // Integrate with your components:
        if (event.type == FeedType::BID) {
            snapshot.update_bid(event.price, event.quantity);
            // check if quantity was updated and old orders needed to be placed
            if (order_manager.resolved_conflicts(Side::Sell, event.price, event.quantity))
                continue;
            const PriceLevel* best_bid = snapshot.get_best_bid();
            if (best_bid->price > prev_best_bid_price) {
                order_manager.place_order(Side::Sell, best_bid->price, best_bid->quantity);
                prev_best_bid_price = best_bid->price;
            }
        } else if (event.type == FeedType::ASK) {
            snapshot.update_ask(event.price, event.quantity);
            // check if quantity was updated and old orders needed to be replaced
            if (order_manager.resolved_conflicts(Side::Buy, event.price, event.quantity))
                continue;
            const PriceLevel* best_ask = snapshot.get_best_ask();
            if (best_ask->price < prev_best_ask_price) {
                order_manager.place_order(Side::Buy, best_ask->price, best_ask->quantity);
                prev_best_ask_price = best_ask->price;
            }
        } else if (event.type == FeedType::EXECUTION) {
            order_manager.handle_fill(event.order_id, event.quantity);
        }

        order_manager.print_active_orders();

    }

    return 0;
}