#pragma once
#include "framework.h"
#include "Abilities.h"

namespace PC {
	inline void (*ServerLoadingScreenDroppedOG)(AFortPlayerControllerAthena* PC);
	inline void ServerLoadingScreenDropped(AFortPlayerControllerAthena* PC) {
		if (!PC) {
			return ServerLoadingScreenDroppedOG(PC);
		}

		AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;
		AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
		AFortGameModeAthena* GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
		auto Pawn = (AFortPlayerPawn*)PC->Pawn;

		Abilities::InitAbilitiesForPlayer(PC);

		PC->XPComponent->bRegisteredWithQuestManager = true;
		//PC->XPComponent->OnRep_bRegisteredWithQuestManager(); OnRep functions are cooking the gs for some reason?!

		PlayerState->SeasonLevelUIDisplay = PC->XPComponent->CurrentLevel;
		//PlayerState->OnRep_SeasonLevelUIDisplay();

		return ServerLoadingScreenDroppedOG(PC);
	}

	inline void ServerAttemptAircraftJump(UFortControllerComponent_Aircraft* Comp, FRotator Rotation)
	{
		auto PC = (AFortPlayerControllerAthena*)Comp->GetOwner();
		UWorld::GetWorld()->AuthorityGameMode->RestartPlayer(PC);

		if (PC->MyFortPawn)
		{
			PC->ClientSetRotation(Rotation, true);
			PC->MyFortPawn->BeginSkydiving(true);
			PC->MyFortPawn->SetHealth(100);
			PC->MyFortPawn->SetShield(0);
		}
	}

	void Hook() {
		//static auto FortPlayerControllerAthena = StaticFindObject<AFortPlayerControllerAthena>(L"/Script/FortniteGame.Default__FortPlayerControllerAthena");

		//MH_CreateHook((LPVOID)(ImageBase + 0xc57d70), ServerAcknowledgePossession, (LPVOID*)&ServerAcknowledgePossessionOG); I dont seem to need it
		//HookVTable(FortPlayerControllerAthena, 0x114, ServerAcknowledgePossession, nullptr);

		MH_CreateHook((LPVOID)(ImageBase + 0x149c554), ServerLoadingScreenDropped, (LPVOID*)&ServerLoadingScreenDroppedOG);

		//MH_CreateHook((LPVOID)(ImageBase + 0xc57d70), ServerAttemptAircraftJump, nullptr);
		HookVTable(UFortControllerComponent_Aircraft::GetDefaultObj(), 0x94, ServerAttemptAircraftJump, nullptr);

		Log("PC Hooked!");
	}
}