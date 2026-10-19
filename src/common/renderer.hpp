#pragma once
#include "math.hpp"

#include <string>
#include <vector>

namespace Common {

struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

class IRenderer {
 public:
  virtual ~IRenderer() = default;

  virtual Vec2u GetSize() const = 0;

  virtual void BeginRendering() = 0;
  virtual void EndRendering() = 0;

  virtual void RenderRect(
    const Vec2f& pos,
    const Vec2f& size,
    const Color& color
  ) = 0;
  virtual void RenderText(
    const Vec2f& pos,
    const Vec2f& size,
    const Color& color,
    const std::string& text
  ) = 0;
  virtual void RenderPoly(
    const std::vector<Vec2f>& points,
    const std::vector<int>& indices,
    const Color& color
  ) = 0;
};

}  // namespace Common