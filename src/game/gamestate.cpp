#include "game/gamestate.hpp"

#include <cassert>
#include <charconv>
#include <fstream>
#include <random>
#include <string>
#include <unordered_set>

namespace {

using namespace Common;
using namespace Game;

Color RandomColor() {
  static std::mt19937 rng;
  static std::uniform_int_distribution<uint16_t> dist;
  return {
    static_cast<uint8_t>(dist(rng) % UINT8_MAX),
    static_cast<uint8_t>(dist(rng) % UINT8_MAX),
    static_cast<uint8_t>(dist(rng) % UINT8_MAX),
    255,
  };
}

void RenderConnection(
  IRenderer& renderer,
  const Vec2f& p1,
  const Vec2f& p2,
  uint32_t width
) {
  const auto diff = p2 - p1;
  const auto angle = std::atan2f(diff.y, diff.x) - static_cast<float>(M_PI_2);
  const Vec2f offset{
    std::cosf(angle) * width / 2,
    std::sinf(angle) * width / 2,
  };

  renderer.RenderPoly(
    {
      p1 + offset,
      p1 - offset,
      p2 + offset,
      p2 - offset,
    },
    {0, 1, 2, 2, 3, 1},
    Color{
      CONNECTION_COLOR[0],
      CONNECTION_COLOR[1],
      CONNECTION_COLOR[2],
      CONNECTION_COLOR[3],
    }
  );
}

bool readUint(const std::string& str, uint32_t& val) {
  if (str.empty()) {
    return false;
  }
  auto [ptr, ec] = std::from_chars(&*str.cbegin(), &*str.cend(), val);
  return ec == std::errc{};
}

std::vector<uint32_t> readVec(std::istream& stream, uint32_t count) {
  assert(count > 0);
  if (count == 0) {
    return {};
  }
  std::string str;
  std::vector<uint32_t> result(count);
  for (uint32_t i = 0; i < count - 1; i++) {
    std::getline(stream, str, ',');
    if (!readUint(str, result[i])) {
      return {};
    }
  }
  std::getline(stream, str);
  if (!readUint(str, result[count - 1])) {
    return {};
  }
  return result;
}

bool validateArray(
  const std::vector<uint32_t>& vec,
  std::function<bool(int)>&& pred
) {
  const auto end = vec.cend();
  const auto it = std::find_if_not(vec.cbegin(), end, pred);
  return it == end;
}

}  // namespace

namespace Game {

struct GameState::AvailableMovesCache {
  std::vector<uint32_t> paths;  // paths[i] -- stores step towards target point
                                // from point with ID i
  uint32_t builtFor;            // id of point for which cache is built
};

std::istream& operator>>(std::istream& stream, Vec2u& vec) {
  std::string str;
  std::getline(stream, str, ',');
  if (!readUint(str, vec.x)) {
    stream.setstate(std::ios_base::failbit);
    return stream;
  }
  std::getline(stream, str);
  if (!readUint(str, vec.y)) {
    stream.setstate(std::ios_base::failbit);
    return stream;
  }
  return stream;
}

GameState::GameState() = default;

GameState::GameState(GameState&&) noexcept = default;

GameState::~GameState() = default;

std::optional<GameState> GameState::LoadFromFile(std::filesystem::path path) {
  GameState result;

  uint32_t numOfChips = 0;
  uint32_t numOfPoints = 0;
  uint32_t numOfConnections = 0;

  std::ifstream file{path.string()};

  if (!file.is_open()) {
    return std::nullopt;
  }

  file >> numOfChips >> numOfPoints;
  if (numOfChips == 0 || numOfPoints == 0 || file.fail()) {
    return std::nullopt;
  }

  result.m_points.resize(numOfPoints);
  for (uint32_t i = 0; i < numOfPoints; i++) {
    file >> result.m_points[i];
    if (file.fail()) {
      return std::nullopt;
    }
  }

  const auto vecPredicate = [numOfPoints](uint32_t i) {
    return i > 0 && i <= numOfPoints;
  };
  const auto currentPositions = readVec(file, numOfChips);
  if (
    currentPositions.empty() || !validateArray(currentPositions, vecPredicate)
  ) {
    return std::nullopt;
  }

  const auto winningPositions = readVec(file, numOfChips);
  if (
    winningPositions.empty() || !validateArray(winningPositions, vecPredicate)
  ) {
    return std::nullopt;
  }

  for (uint32_t i = 0; i < numOfChips; i++) {
    result.m_chips.push_back(
      Chip{
        result.m_points[currentPositions[i] - 1], RandomColor(),
        currentPositions[i] - 1, winningPositions[i] - 1
      }
    );
  }

  file >> numOfConnections;
  if (numOfConnections == 0 || file.fail()) {
    return std::nullopt;
  }

  result.m_connections.reserve(numOfConnections);
  for (uint32_t i = 0; i < numOfConnections; i++) {
    Vec2u conn;
    file >> conn;
    if (file.fail() || conn.x == 0 || conn.x > numOfPoints || conn.y == 0 || conn.y > numOfPoints) {
      return std::nullopt;
    }
    result.m_connections.push_back({conn.x - 1, conn.y - 1});
  }
  return result;
}

bool GameState::IsMoving() const {
  return !m_movementPath.empty();
}

bool GameState::CheckWinCondition() const {
  for (const auto& chip : m_chips) {
    if (chip.currentPosId != chip.winningPosId) {
      return false;
    }
  }
  return !IsMoving();  // Win happens after last movement is done
}

std::vector<uint32_t> GameState::GetAvailableMoves(uint32_t chipId) const {
  uint32_t posId = m_chips[chipId].currentPosId;
  BuildMovesCache(posId);
  std::vector<uint32_t> result;
  for (uint32_t i = 0; i < m_movesCache->paths.size(); i++) {
    uint32_t move = m_movesCache->paths[i];
    if (move != UINT32_MAX && i != posId) {
      result.push_back(i);
    }
  }
  return result;
}

void GameState::UnpickChip() {
  m_pickedChip = UINT32_MAX;
  InvalidateMovesCache();
}

void GameState::MovePickedChipTo(uint32_t pointId) {
  assert(m_pickedChip != UINT32_MAX);
  BuildMovesCache(m_chips[m_pickedChip].currentPosId);
  m_chips[m_pickedChip].currentPosId = pointId;
  while (true) {
    assert(m_movesCache->paths[pointId] != UINT32_MAX);
    m_movementPath.push_front(pointId);
    pointId = m_movesCache->paths[pointId];
    if (pointId == m_movesCache->paths[pointId]) {
      return;
    }
  }
}

Vec2f GameState::GetPickedChipTarget() const {
  if (!m_movementPath.empty()) {
    return m_points[m_movementPath.front()];
  }
  return m_chips[m_pickedChip].pos;
}

void GameState::SetPickedChipPos(Vec2f pos) {
  m_chips[m_pickedChip].pos = pos;
  if (
    !m_movementPath.empty() &&
    Vec2f::distance(m_points[m_movementPath.front()], pos) < 0.1f
  ) {
    m_movementPath.pop_front();
    if (m_movementPath.empty()) {
      InvalidateMovesCache();
    }
  }
}

void GameState::Update(uint64_t ticks) {
  m_ticks += ticks;
  if (IsMoving() && m_pickedChip != UINT32_MAX) {
    const auto target = GetPickedChipTarget();
    const auto pos = m_chips[m_pickedChip].pos;
    const auto dir = pos - target;
    const auto dist = Vec2f::distance(dir, Vec2f{0.0f, 0.0f});

    if (dist < 1.0f) {
      SetPickedChipPos(target);
    } else {
      const auto norm = dir / dist;
      const auto moveDist = std::min(dist, static_cast<float>(ticks));
      SetPickedChipPos(pos - norm * moveDist);
    }
  }
}

void GameState::Render(IRenderer& renderer) const {
  const Vec2f winCondShift{500, 50};

  renderer.RenderText({520, 50}, {80, 20}, {255, 255, 255, 255}, "Target:");

  for (const auto& [p1, p2] : m_connections) {
    const auto pos1 = m_points[p1];
    const auto pos2 = m_points[p2];
    RenderConnection(renderer, pos1, pos2, CONNECTION_WIDTH);
    RenderConnection(
      renderer, winCondShift + pos1 / 2, winCondShift + pos2 / 2,
      CONNECTION_WIDTH / 2
    );
  }

  for (auto i = 0u; i < m_chips.size(); i++) {
    const auto& chip = m_chips[i];
    const auto size = CHIP_RADIUS * (i == m_pickedChip ? 2 : 1);
    renderer.RenderRect(chip.pos - size / 2, {size, size}, chip.color);

    const auto pos = m_points[chip.winningPosId];
    renderer.RenderRect(
      winCondShift + pos / 2 - CHIP_RADIUS / 4,
      {CHIP_RADIUS / 2, CHIP_RADIUS / 2}, chip.color
    );
  }

  if (m_pickedChip != UINT32_MAX) {
    const auto moves = GetAvailableMoves(m_pickedChip);
    for (const auto posId : moves) {
      const Color color{
        200,
        200,
        50,
        static_cast<uint8_t>(205 + 50 * sinf(m_ticks / 100.0f)),
      };
      const Vec2f pos = m_points[posId];
      renderer.RenderRect(
        pos - CHIP_RADIUS / 2, {CHIP_RADIUS, CHIP_RADIUS}, color
      );
    }
  }

  if (CheckWinCondition()) {
    const Vec2f size = renderer.GetSize();
    renderer.RenderRect({0, 0}, size, {0, 0, 0, 100});
    renderer.RenderText(
      {100, 200}, size - Vec2f{200, 400}, {255, 255, 255, 255}, "Well done!"
    );
  }
}

void GameState::MouseClick(Vec2f pos) {
  if (CheckWinCondition()) {
    UnpickChip();
    return;
  }

  if (IsMoving()) {
    return;
  }

  if (m_pickedChip != UINT32_MAX) {
    const auto moves = GetAvailableMoves(m_pickedChip);
    for (const auto pointId : moves) {
      if (Vec2f::distance(pos, m_points[pointId]) < CHIP_RADIUS * 2) {
        MovePickedChipTo(pointId);
        return;
      }
    }
  }

  for (auto i = 0u; i < m_chips.size(); i++) {
    if (Vec2f::distance(pos, m_chips[i].pos) < CHIP_RADIUS * 2) {
      m_pickedChip = i;
      return;
    }
  }
}

void GameState::BuildMovesCache(uint32_t pointId) const {
  if (m_movesCache && m_movesCache->builtFor == pointId) {
    return;
  }

  m_movesCache = std::make_unique<AvailableMovesCache>();
  m_movesCache->paths.resize(m_points.size(), UINT32_MAX);
  m_movesCache->paths[pointId] = pointId;
  m_movesCache->builtFor = pointId;
  // we can't go through points taken by other chips
  std::unordered_set<uint32_t> skipPoints;

  for (const auto& chip : m_chips) {
    skipPoints.insert(chip.currentPosId);
  }

  std::deque<uint32_t> bfsNodes;
  bfsNodes.push_back(pointId);
  while (!bfsNodes.empty()) {
    uint32_t node = bfsNodes.front();
    bfsNodes.pop_front();
    for (const auto [n1, n2] : m_connections) {
      if (n1 == node && skipPoints.count(n2) == 0) {
        m_movesCache->paths[n2] = n1;
        bfsNodes.push_back(n2);
        skipPoints.insert(n2);
      }
      if (n2 == node && skipPoints.count(n1) == 0) {
        m_movesCache->paths[n1] = n2;
        bfsNodes.push_back(n1);
        skipPoints.insert(n1);
      }
    }
  }
}

void GameState::InvalidateMovesCache() const {
  m_movesCache.reset();
}

}  // namespace Game