#pragma once
#include "common/event.hpp"

namespace Common {

class IGameState;
class IPlatform;
class IRenderer;

}  // namespace Common

namespace Game {

using Common::IGameState;
using Common::IPlatform;
using Common::IRenderer;

class Game {
 public:
  Game(IPlatform& platform, IRenderer& renderer, IGameState& gameState);
  ~Game();

  void Run();

 private:
  void OnEvent(const Common::Event& event);

 private:
  IPlatform& m_platform;
  IGameState& m_gameState;
  IRenderer& m_renderer;

  bool m_isRunning = true;
};

}  // namespace Game