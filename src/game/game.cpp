#include "game.hpp"

#include "common/gamestate.hpp"
#include "common/platform.hpp"
#include "common/renderer.hpp"

namespace Game {

using namespace Common;

Game::Game(IPlatform& platform, IRenderer& renderer, IGameState& gameState)
    : m_platform(platform), m_renderer(renderer), m_gameState(gameState) {
  m_platform.SetEventHandler([this](const Event& event) { OnEvent(event); });
}

Game::~Game() {
  m_platform.SetEventHandler(nullptr);
}

void Game::Run() {
  auto ticks = m_platform.GetTicks();
  while (m_isRunning) {
    m_platform.ProcessEvents();

    const auto now = m_platform.GetTicks();
    m_gameState.Update(now - ticks);
    ticks = now;

    m_renderer.BeginRendering();
    m_gameState.Render(m_renderer);
    m_renderer.EndRendering();
  }
}

void Game::OnEvent(const Event& event) {
  if (std::holds_alternative<QuitEvent>(event)) {
    m_isRunning = false;
  }
  if (std::holds_alternative<MouseClick>(event)) {
    const auto& clickEvent = std::get<MouseClick>(event);
    m_gameState.MouseClick({clickEvent.x, clickEvent.y});
  }
}

}  // namespace Game