#include "math.hpp"

#include <cmath>

namespace Common {

Vec2f Vec2f::operator+(const Vec2f& rhs) const noexcept {
  return {x + rhs.x, y + rhs.y};
}

Vec2f Vec2f::operator+(float val) const noexcept {
  return {x + val, y + val};
}

Vec2f Vec2f::operator-(const Vec2f& rhs) const noexcept {
  return {x - rhs.x, y - rhs.y};
}
Vec2f Vec2f::operator-(float val) const noexcept {
  return {x - val, y - val};
}

Vec2f Vec2f::operator*(float val) const noexcept {
  return {x * val, y * val};
}

Vec2f Vec2f::operator/(float val) const noexcept {
  return {x / val, y / val};
}

float Vec2f::distance(const Vec2f& p1, const Vec2f& p2) {
  float s1 = std::powf(p1.x - p2.x, 2);
  float s2 = std::powf(p1.y - p2.y, 2);
  return std::sqrtf(s1 + s2);
}

bool Vec2u::operator==(const Vec2u& rhs) const noexcept {
  return x == rhs.x && y == rhs.y;
}

Vec2u Vec2u::operator+(const Vec2u& rhs) const noexcept {
  return {x + rhs.x, y + rhs.y};
}

Vec2u Vec2u::operator-(const Vec2u& rhs) const noexcept {
  return {x - rhs.x, y - rhs.y};
}

Vec2u Vec2u::operator*(uint32_t val) const noexcept {
  return {x * val, y * val};
}

Vec2u Vec2u::operator/(uint32_t val) const noexcept {
  return {x / val, y / val};
}

Vec2u::operator Vec2f() const noexcept {
  return Vec2f{static_cast<float>(x), static_cast<float>(y)};
}

}  // namespace Common