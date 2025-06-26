#pragma once

#include "orderbook/aliases.hpp"

enum class Side : char {
    Buy = 'B',
    Sell = 'S',
};

enum class MsgType {
    Add,
    Delete,
    Modify,
    Trade,
};

struct Add {
    OrderId id;
    Side side;
    Price price;
    Quantity quantity;
};

struct Delete {
    OrderId id;
};

struct Modify {
    OrderId id;
    Quantity quantity;
};

struct Trade {
    Side side;
    Price price;
    Quantity shares;
};

struct Message {
    MsgType msg_type;
    union {
        Add add;
        Modify mod;
        Delete del;
        Trade trd;
    };
};
