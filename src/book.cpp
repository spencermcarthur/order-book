#include "orderbook/book.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "orderbook/aliases.hpp"
#include "orderbook/message.hpp"
#include "orderbook/order.hpp"

#define VALIDATE(prices, level)                         \
    if (level >= prices.size()) {                       \
        throw std::range_error("Level out of range: " + \
                               std::to_string(level));  \
    }

Price Book::bid_price(int level) {
    VALIDATE(m_bid_prices, level);

    const int reverse_level = m_bid_prices.size() - 1 - level;
    const Price price = m_bid_prices[reverse_level];
    return price;
}

Quantity Book::bid_quantity(int level) {
    VALIDATE(m_bid_prices, level);

    // Get price
    const int reverse_level = m_bid_prices.size() - 1 - level;
    const Price price = m_bid_prices[reverse_level];

    // Get level qty
    Quantity qty{};
    const auto qty_iter = m_bid_qtys.find(price);
    if (qty_iter != m_bid_qtys.end()) {
        qty = qty_iter->second;
    }

    return qty;
}

Price Book::ask_price(int level) {
    VALIDATE(m_ask_prices, level);

    const Price price = m_ask_prices[level];
    return price;
}

Quantity Book::ask_quantity(int level) {
    VALIDATE(m_ask_prices, level);

    // Get price
    const Price price = m_ask_prices[level];

    // Get level qty
    Quantity qty{};
    const auto qty_iter = m_ask_qtys.find(price);
    if (qty_iter != m_ask_qtys.end()) {
        qty = qty_iter->second;
    }

    return qty;
}

void Book::handle_message(const Message& msg) noexcept {
    switch (msg.msg_type) {
        case MsgType::Add:
            handle_add(msg.add);
            break;

        case MsgType::Modify:
            handle_modify(msg.mod);
            break;

        case MsgType::Delete:
            handle_delete(msg.del);
            break;

        case MsgType::Trade:
            handle_trade(msg.trd);
            break;
    }
}

void Book::handle_add(const Add& add) noexcept {
    // Make order
    OrderPtr optr =
        std::make_shared<Order>(add.id, add.side, add.price, add.quantity);

    const bool is_buy = (add.side == Side::Buy);

    // Insert into orders container
    m_orders.insert({add.id, optr});

    // Try to insert level
    std::pair<LevelIter, bool> insert_res;
    if (is_buy) {
        insert_res = m_bids.insert({add.price, {}});
        m_bid_qtys[add.price] += add.quantity;  // increment level qty
    } else {
        insert_res = m_asks.insert({add.price, {}});
        m_ask_qtys[add.price] += add.quantity;  // increment level qty
    }

    // New level inserted
    if (insert_res.second) {
        Prices& prices = is_buy ? m_bid_prices : m_ask_prices;
        const auto price_iter =
            std::lower_bound(prices.begin(), prices.end(), add.price);
        prices.insert(price_iter, add.price);
    }

    // Insert order into level
    Orders& orders = insert_res.first->second;
    optr->order_iter = orders.insert(orders.end(), optr);
    optr->level_iter = insert_res.first;
}

void Book::handle_modify(const Modify& mod) noexcept {
    OrderPtr optr = m_orders[mod.id];
    if (optr != nullptr) [[likely]] {
        // Modify quantity
        const Quantity orig_qty = optr->quantity;
        optr->quantity = mod.quantity;

        // Modified up
        if (mod.quantity > orig_qty) {
            // Order loses queue position
            Orders& orders = optr->level_iter->second;
            orders.erase(optr->order_iter);
            optr->order_iter = orders.insert(orders.end(), optr);
        }

        // Modify level quantity
        LevelQuantities& level_qtys =
            (optr->side == Side::Buy) ? m_bid_qtys : m_ask_qtys;
        Quantity& qty = level_qtys[optr->price];
        qty -= orig_qty;
        qty += mod.quantity;
    }
}

void Book::handle_delete(const Delete& del) noexcept {
    OrderPtr optr = m_orders[del.id];
    if (optr != nullptr) [[likely]] {
        // Erase order
        Orders& orders = optr->level_iter->second;
        orders.erase(optr->order_iter);

        // Modify level quantity
        LevelQuantities& level_qtys =
            (optr->side == Side::Buy) ? m_bid_qtys : m_ask_qtys;
        level_qtys[optr->price] -= optr->quantity;

        m_orders.erase(optr->id);
    }
}

void Book::handle_trade(const Trade& trd) noexcept {}
