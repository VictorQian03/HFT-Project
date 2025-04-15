#include "feed_parser.h"
#include "market_snapshot.h"
#include "order_manager.h"
#include "logger.h"
int main() {
    Logger::get_instance()->init("trading.log");

    auto feed = load_feed("sample_feed.txt");
    auto snapshot = MarketSnapshot();
    auto order_manager = OrderManager();
    for (const auto& event : feed) {
        event.print();

        // Integrate with your components:
        if (event.type == FeedType::BID) {
            snapshot.update_bid(event.price, event.quantity);
        } else if (event.type == FeedType::ASK) {
            snapshot.update_ask(event.price, event.quantity);
        } else if (event.type == FeedType::EXECUTION) {
            order_manager.handle_fill(event.order_id, event.quantity);
        }
    }

    return 0;
}