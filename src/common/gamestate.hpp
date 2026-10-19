#pragma once

#include "math.hpp"

#include <cstdint>

namespace Common {

class IRenderer;

class IGameState {
 public:
  virtual ~IGameState() = default;

  virtual void Update(uint64_t ticks) = 0;
  virtual void Render(IRenderer& renderer) const = 0;
  virtual void MouseClick(Vec2f pos) = 0;
};

}  // namespace Common