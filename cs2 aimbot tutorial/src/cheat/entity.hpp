#include "../mem/memify.h"
#include "../math/vector.hpp"

#include <vector>
#include <thread>

inline memify mem("cs2.exe");

namespace offset
{
	constexpr std::ptrdiff_t dwEntityList = 0x18C7F98;
	constexpr std::ptrdiff_t dwViewMatrix = 0x1929430;
	constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x173B568;

	constexpr std::ptrdiff_t m_hPawn = 0x604; // CHandle<C_BasePlayerPawn>

	constexpr std::ptrdiff_t m_iHealth = 0x334;
	constexpr std::ptrdiff_t m_iTeamNum = 0x3CB;

	constexpr std::ptrdiff_t m_vOldOrigin = 0x127C;

	constexpr std::ptrdiff_t m_entitySpottedState = 0x2278; // 0x8 m_bSpotted
}

// create an entity class for our vector, since we need to be able to push_back().
class C_CSPlayerPawn
{
public:
	int health, team;

	Vector Position;

	uintptr_t pCSPlayerPawn;
};

inline C_CSPlayerPawn CCSPlayerPawn;

// create a class for filtering players, and our new thread.
class Reader
{
public:
	uintptr_t client = 0;

	std::vector<C_CSPlayerPawn> playerList;

	void ThreadLoop();
private:
	void FilterPlayers();
};

inline Reader reader;

// aimbot functions
class Aimbot
{
public:
	void doAimbot();
private:
	Vector findClosest(const std::vector<Vector> playerPositions);
	void MoveMouseToPlayer(Vector position);
};

inline Aimbot aimbot;