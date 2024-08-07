#include "entity.hpp"

void Reader::ThreadLoop()
{
	// get client
	while (!client) {
		std::this_thread::sleep_for(std::chrono::milliseconds(15));

		client = mem.GetBase("client.dll");

		std::cout << "client.dll " << std::hex << client << std::endl;
	}

	while (true)
	{
		// The reason I used a loop counter is actually valid, but my old implementation was flawed due to the fact we use multiple threads now.
		// I'm used to working in a single thread environment, so I needed to count loops to manage other tasks within the loop.
		// Therefore, I couldn't just sleep for 250ms.
		//
		// However, since this is a new thread, we can simply sleep the thread to save performance.

		std::this_thread::sleep_for(std::chrono::milliseconds(250));

		FilterPlayers();
	}
}

void Reader::FilterPlayers()
{
	// clear playerList
	playerList.clear();

	auto entityList = mem.Read<uintptr_t>(client + offset::dwEntityList);

	if (!entityList)
		return;

	auto localPawn = mem.Read<uintptr_t>(client + offset::dwLocalPlayerPawn);

	// check swedz video for an explanation of this, i do not have the patience to write out all those comments :(
	for (int i = 0; i <= 64; ++i)
	{
		uintptr_t list_entry1 = mem.Read<uintptr_t>(entityList + (8 * (i & 0x7FFF) >> 9) + 16);

		uintptr_t playerController = mem.Read<uintptr_t>(list_entry1 + 120 * (i & 0x1FF));

		uint32_t playerPawn = mem.Read<uint32_t>(playerController + offset::m_hPlayerPawn);

		uintptr_t list_entry2 = mem.Read<uintptr_t>(entityList + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);

		uintptr_t pCSPlayerPawnPtr = mem.Read<uintptr_t>(list_entry2 + 120 * (playerPawn & 0x1FF));

		int health = mem.Read<int>(pCSPlayerPawnPtr + offset::m_iHealth);

		if (health <= 0 || health > 100)
			continue;

		int team = mem.Read<int>(pCSPlayerPawnPtr + offset::m_iTeamNum);

		if (team == mem.Read<int>(localPawn + offset::m_iTeamNum))
			continue;

		// save the address of the pawn we're on for later use, possibly reading positions.
		CCSPlayerPawn.pCSPlayerPawn = pCSPlayerPawnPtr;

		// push back the entity we're on now, as in save them for later so we can loop through them.
		playerList.push_back(CCSPlayerPawn);
	}
}