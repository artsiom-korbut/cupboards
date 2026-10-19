#include "game/game.hpp"
#include "game/gamestate.hpp"
#include "sdl/window.hpp"

int main(int argc, char** argv) {
  auto state = Game::GameState::LoadFromFile(argc == 1 ? "game.txt" : argv[1]);
  if (!state.has_value()) {
    return 1;
  }

  auto window = SDL::Window{800, 600, "Cupboards"};
  auto game = Game::Game{window, window, *state};

  window.Show();
  game.Run();
  window.Hide();

  return 0;
}
