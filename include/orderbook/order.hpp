#pragma once

#include "orderbook/aliases.hpp"
#include "orderbook/message.hpp"

struct Order {
    OrderId id;
    Side side;
    Price price;
    Quantity quantity;

    LevelIter level_iter;
    OrderIter order_iter;
};
