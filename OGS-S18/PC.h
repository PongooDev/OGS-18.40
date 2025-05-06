#pragma once
#include "framework.h"
#include "Abilities.h"

namespace PC {
	inline void (*ServerLoadingScreenDroppedOG)(AFortPlayerControllerAthena* PC);
	inline void ServerLoadingScreenDropped(AFortPlayerControllerAthena* PC) {
		AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;
		AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
		AFortGameModeAthena* GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
		auto Pawn = (AFortPlayerPawn*)PC->Pawn;

		Abilities::InitAbilitiesForPlayer(PC);

		PlayerState->SquadId = PlayerState->TeamIndex - 2;
		PlayerState->OnRep_SquadId();
		PlayerState->OnRep_TeamIndex(0);
		PlayerState->OnRep_PlayerTeam();
		PlayerState->OnRep_PlayerTeamPrivate();

		FGameMemberInfo Info{ -1, -1, -1 };
		Info.MemberUniqueId = PlayerState->UniqueId;
		Info.SquadId = PlayerState->SquadId;
		Info.TeamIndex = PlayerState->TeamIndex;

		GameState->GameMemberInfoArray.Members.Add(Info);

		GameState->GameMemberInfoArray.MarkItemDirty(Info);

		return ServerLoadingScreenDroppedOG(PC);
	}

	void Hook() {
		//static auto FortPlayerControllerAthena = StaticFindObject<AFortPlayerControllerAthena>(L"/Script/FortniteGame.Default__FortPlayerControllerAthena");

		//MH_CreateHook((LPVOID)(ImageBase + 0xc57d70), ServerAcknowledgePossession, (LPVOID*)&ServerAcknowledgePossessionOG); I dont seem to need it
		//HookVTable(FortPlayerControllerAthena, 0x114, ServerAcknowledgePossession, nullptr);

		MH_CreateHook((LPVOID)(ImageBase + 0x149c554), ServerLoadingScreenDropped, (LPVOID*)&ServerLoadingScreenDroppedOG);

		Log("PC Hooked!");
	}
}