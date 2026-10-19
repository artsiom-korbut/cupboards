#pragma once

#include "common/event.hpp"
#include "common/platform.hpp"
#include "common/renderer.hpp"

#include <functional>
#include <string>

union SDL_Event;
struct SDL_Window;
struct SDL_Renderer;
struct TTF_Font;

namespace SDL {

using Common::Color;
using Common::Vec2f;
using Common::Vec2u;

class Window final : public Common::IPlatform, public Common::IRenderer {
  using EventHandler = std::function<void(const Common::Event&)>;

 public:
  Window(uint32_t width, uint32_t height, const std::string& title);
  ~Window();

  void Show();
  void Hide();

  uint64_t GetTicks() const override;
  void ProcessEvents() override;
  void SetEventHandler(EventHandler&& handler) override;

  Vec2u GetSize() const override;

  void BeginRendering() override;
  void EndRendering() override;

  void RenderRect(
    const Vec2f& pos,
    const Vec2f& size,
    const Color& color
  ) override;

  void RenderText(
    const Vec2f& pos,
    const Vec2f& size,
    const Color& color,
    const std::string& text
  ) override;

  void RenderPoly(
    const std::vector<Vec2f>& poly,
    const std::vector<int>& indices,
    const Color& color
  ) override;

 private:
  SDL_Window* m_window;
  SDL_Renderer* m_renderer;
  TTF_Font* m_font;
  EventHandler m_eventHandler;
};

}  // namespace SDL