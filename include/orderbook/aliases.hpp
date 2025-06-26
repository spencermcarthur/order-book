#pragma once

#include <list>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

using OrderId = unsigned;
using Price = int;
using Quantity = unsigned;

struct Order;
using OrderPtr = std::shared_ptr<Order>;
using OrdersById = std::unordered_map<OrderId, OrderPtr>;

using Orders = std::list<OrderPtr>;
using OrderIter = Orders::iterator;

using Bids = std::map<Price, Orders, std::greater<>>;
using Asks = std::map<Price, Orders, std::less<>>;
using LevelIter = std::map<Price, Orders>::iterator;

using Prices = std::vector<Price>;
using LevelQuantities = std::unordered_map<Price, Quantity>;
