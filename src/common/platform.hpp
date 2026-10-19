#pragma once
#include "common/event.hpp"

#include <cstdint>
#include <functional>

namespace Common {

class IPlatform {
 public:
  using EventHandler = std::function<void(const Common::Event&)>;

  virtual ~IPlatform() = default;

  virtual uint64_t GetTicks() const = 0;
  virtual void ProcessEvents() = 0;
  virtual void SetEventHandler(EventHandler&& handler) = 0;
};

}  // namespace Common