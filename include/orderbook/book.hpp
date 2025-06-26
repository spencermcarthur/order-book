#pragma once

#include "orderbook/aliases.hpp"
#include "orderbook/macros.hpp"
#include "orderbook/message.hpp"

class Book {
public:
    OB_NO_COPY(Book);

    Price bid_price(int level = 0);
    Quantity bid_quantity(int level = 0);

    Price ask_price(int level = 0);
    Quantity ask_quantity(int level = 0);

    void handle_message(const Message&) noexcept;

private:
    void handle_add(const Add&) noexcept;
    void handle_modify(const Modify&) noexcept;
    void handle_delete(const Delete&) noexcept;
    void handle_trade(const Trade&) noexcept;

private:
    OrdersById m_orders;

    Bids m_bids;
    Prices m_bid_prices;
    LevelQuantities m_bid_qtys;

    Asks m_asks;
    Prices m_ask_prices;
    LevelQuantities m_ask_qtys;
};
