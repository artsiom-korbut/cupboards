#pragma once
#include <variant>

namespace Common {

struct QuitEvent {};
struct MouseClick {
  float x;
  float y;
};

using Event = std::variant<QuitEvent, MouseClick>;

}  // namespace Common