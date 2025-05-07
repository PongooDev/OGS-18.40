#pragma once
#include "framework.h"

namespace Tick {
	void (*ServerReplicateActors)(void*) = decltype(ServerReplicateActors)(ImageBase + 0x3fd2b1c);

	inline void (*TickFlushOG)(UNetDriver*, float);
	void TickFlush(UNetDriver* Driver, float DeltaTime)
	{
		if (!Driver)
			return;

		if (!Driver->ReplicationDriver)
		{
			Log("ReplicationDriver Doesent Exist!");
		}

		/*if (Driver && Driver->ClientConnections.Num() > 0 && Driver->ReplicationDriver)
			ServerReplicateActors(Driver->ReplicationDriver);*/

		ServerReplicateActors(Driver->ReplicationDriver);
		
		return TickFlushOG(Driver, DeltaTime);
	}


	inline float GetMaxTickRate()
	{
		return 30.f;
	}

	void Hook() {
		MH_CreateHook((LPVOID)(ImageBase + 0xe976cc), GetMaxTickRate, nullptr);
		MH_CreateHook((LPVOID)(ImageBase + 0xd122e0), TickFlush, (LPVOID*)&TickFlushOG);

		Log("Tick Hooked!");
	}
}