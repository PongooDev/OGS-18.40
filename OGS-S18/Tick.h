#pragma once
#include "framework.h"

namespace Tick {
	inline void (*TickFlushOG)(UNetDriver* Driver, float DeltaSeconds);
	inline void TickFlush(UNetDriver* Driver, float DeltaSeconds)
	{
		if (!Driver)
			return;

		if (!Driver->ReplicationDriver)
		{
			Log("ReplicationDriver Doesent Exist!");
		}

		static void (*ServerReplicateActors)(void*) = decltype(ServerReplicateActors)(ImageBase + 0x3fd2b1c);
		if (Driver->ReplicationDriver && Driver->ClientConnections.Num() > 0)
			ServerReplicateActors(Driver->ReplicationDriver);

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