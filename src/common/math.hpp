#pragma once
#include <cstdint>

namespace Common {

struct Vec2f {
  float x;
  float y;

  Vec2f operator+(const Vec2f& rhs) const noexcept;
  Vec2f operator+(float val) const noexcept;
  Vec2f operator-(const Vec2f& rhs) const noexcept;
  Vec2f operator-(float val) const noexcept;
  Vec2f operator*(float val) const noexcept;
  Vec2f operator/(float val) const noexcept;

  static float distance(const Vec2f& p1, const Vec2f& p2);
};

struct Vec2u {
  uint32_t x;
  uint32_t y;

  bool operator==(const Vec2u& rhs) const noexcept;
  Vec2u operator+(const Vec2u& rhs) const noexcept;
  Vec2u operator-(const Vec2u& rhs) const noexcept;
  Vec2u operator*(uint32_t val) const noexcept;
  Vec2u operator/(uint32_t val) const noexcept;
  operator Vec2f() const noexcept;
};

}  // namespace Common