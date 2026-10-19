#include "window.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <cassert>

namespace {

const char* getFontPath() {
#if defined(_WIN32)
  return "C:\\Windows\\Fonts\\arial.ttf";
#elif defined(__APPLE__)
  return "/System/Library/Fonts/Supplemental/Arial.ttf";
#else
#error "No font found"
#endif
}

SDL_FColor colorToFColor(const Common::Color& color) {
  const auto convert = [](uint8_t v) { return static_cast<float>(v) / 255.0f; };

  return {
    convert(color.r),
    convert(color.g),
    convert(color.b),
    convert(color.a),
  };
}

}  // namespace

namespace SDL {

Window::Window(uint32_t width, uint32_t height, const std::string& title) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(
    title.c_str(), static_cast<int>(width), static_cast<int>(height),
    SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY, &m_window, &m_renderer
  );
  TTF_Init();
  m_font = TTF_OpenFont(getFontPath(), 96);
  assert(m_font && "Couldn't find font, set other path");

  SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
}

Window::~Window() {
  TTF_CloseFont(m_font);
  TTF_Quit();
  SDL_DestroyRenderer(m_renderer);
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

void Window::SetEventHandler(EventHandler&& handler) {
  m_eventHandler = std::move(handler);
}

void Window::ProcessEvents() {
  SDL_Event event;
  if (!m_eventHandler) {
    return;
  }
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        m_eventHandler(Common::QuitEvent{});
        break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        m_eventHandler(Common::MouseClick{event.button.x, event.button.y});
        break;

      default:
        break;
    }
  }
}

void Window::Show() {
  SDL_ShowWindow(m_window);
}

void Window::Hide() {
  SDL_HideWindow(m_window);
}

uint64_t Window::GetTicks() const {
  return SDL_GetTicks();
}

void Window::BeginRendering() {
  SDL_SetRenderDrawColor(m_renderer, 24, 24, 24, 255);
  SDL_RenderClear(m_renderer);
}

void Window::EndRendering() {
  SDL_RenderPresent(m_renderer);
}

Vec2u Window::GetSize() const {
  int w = 0, h = 0;
  SDL_GetWindowSize(m_window, &w, &h);
  return {static_cast<uint32_t>(w), static_cast<uint32_t>(h)};
}

void Window::RenderRect(
  const Vec2f& pos,
  const Vec2f& size,
  const Color& color
) {
  const float scaleFactor = SDL_GetWindowDisplayScale(m_window);
  SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
  const SDL_FRect rect{
    pos.x * scaleFactor,
    pos.y * scaleFactor,
    size.x * scaleFactor,
    size.y * scaleFactor,
  };
  SDL_RenderFillRect(m_renderer, &rect);
}

void Window::RenderText(
  const Vec2f& pos,
  const Vec2f& size,
  const Color& color,
  const std::string& text
) {
  const float scaleFactor = SDL_GetWindowDisplayScale(m_window);
  const SDL_FRect rect{
    pos.x * scaleFactor,
    pos.y * scaleFactor,
    size.x * scaleFactor,
    size.y * scaleFactor,
  };

  auto* surface = TTF_RenderText_Blended(
    m_font, text.c_str(), text.size(), {color.r, color.g, color.b, color.a}
  );
  auto* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
  SDL_RenderTexture(m_renderer, texture, nullptr, &rect);
  SDL_DestroyTexture(texture);
  SDL_DestroySurface(surface);
}

void Window::RenderPoly(
  const std::vector<Vec2f>& poly,
  const std::vector<int>& indices,
  const Color& color
) {
  const auto fcolor = colorToFColor(color);
  const float scaleFactor = SDL_GetWindowDisplayScale(m_window);

  std::vector<SDL_Vertex> vertices;
  vertices.reserve(poly.size());

  for (const auto& point : poly) {
    const SDL_FPoint pos{
      point.x * scaleFactor,
      point.y * scaleFactor,
    };
    vertices.push_back(SDL_Vertex{pos, fcolor});
  }

  SDL_RenderGeometry(
    m_renderer, nullptr, vertices.data(), vertices.size(), indices.data(),
    indices.size()
  );
}

}  // namespace SDL