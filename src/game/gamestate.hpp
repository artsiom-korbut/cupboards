#pragma once
#include "common/config.hpp"
#include "common/gamestate.hpp"
#include "common/math.hpp"
#include "common/renderer.hpp"

#include <deque>
#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

namespace Game {

using Common::Color;
using Common::Vec2f;
using Common::Vec2u;

struct Chip {
  Vec2f pos;
  Color color;
  uint32_t currentPosId;
  uint32_t winningPosId;
};

class GameState final : public Common::IGameState {
 public:
  GameState();
  GameState(GameState&&) noexcept;
  ~GameState();

  static std::optional<GameState> LoadFromFile(std::filesystem::path path);

  void Update(uint64_t ticks) override;
  void Render(Common::IRenderer& renderer) const override;
  void MouseClick(Vec2f pos) override;

 private:
  bool IsMoving() const;
  bool CheckWinCondition() const;
  std::vector<uint32_t> GetAvailableMoves(uint32_t chipId) const;

  void UnpickChip();
  void MovePickedChipTo(uint32_t pointId);
  Vec2f GetPickedChipTarget() const;
  void SetPickedChipPos(Vec2f pos);

  // marking cache methods as const because
  // 1) we want to call it from const GetAvailableMoves
  // 2) they change mutable cache only
  void BuildMovesCache(uint32_t pointId) const;
  void InvalidateMovesCache() const;

 private:
  std::vector<Vec2u> m_points;
  std::vector<Chip> m_chips;
  std::vector<Vec2u> m_connections;

  uint32_t m_pickedChip = UINT32_MAX;
  std::deque<uint32_t> m_movementPath;
  uint64_t m_ticks = 0u;

  struct AvailableMovesCache;
  mutable std::unique_ptr<AvailableMovesCache> m_movesCache;
};

}  // namespace Game