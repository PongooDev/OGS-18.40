#pragma once
#include "framework.h"

namespace Tick {
	inline void (*TickFlushOG)(UNetDriver* Driver, float DeltaSeconds);
	inline void TickFlush(UNetDriver* Driver, float DeltaSeconds)
	{
		static void (*ServerReplicateActors)(void*) = decltype(ServerReplicateActors)(ImageBase + 0x3fd2b1c);
		if (ServerReplicateActors && Driver->ReplicationDriver && Driver->ClientConnections.Num() > 0)
			ServerReplicateActors(Driver->ReplicationDriver);

		auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
		auto Statics = (UGameplayStatics*)UGameplayStatics::StaticClass()->DefaultObject;
		auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;

		return TickFlushOG(Driver, DeltaSeconds);
	}


	inline float GetMaxTickRate()
	{
		return 30.f;
	}

	void Hook() {
		MH_CreateHook((LPVOID)(ImageBase + 0xd122e0), TickFlush, (LPVOID*)&TickFlushOG);
		MH_CreateHook((LPVOID)(ImageBase + 0xe976cc), GetMaxTickRate, nullptr);

		Log("Tick Hooked!");
	}
}