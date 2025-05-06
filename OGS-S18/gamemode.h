#pragma once
#include "framework.h"

namespace GameMode {
	inline bool (*ReadyToStartMatchOG)(AFortGameModeAthena* GameMode);
	inline bool ReadyToStartMatch(AFortGameModeAthena* GameMode) {
		AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;

		static bool SetupPlaylist = false;
		if (!SetupPlaylist) {
			SetupPlaylist = true;
			//UFortPlaylistAthena* Playlist = UObject::FindObject<UFortPlaylistAthena>("Playlist_DefaultSolo.Playlist_DefaultSolo"); Doesent work for some reason idk
			UFortPlaylistAthena* Playlist = StaticLoadObject<UFortPlaylistAthena>("/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo");
			if (!Playlist) {
				Log("Could not find playlist!");
				return false;
			}
			else {
				Log("Found Playlist!");
			}

			GameState->CurrentPlaylistInfo.BasePlaylist = Playlist;
			GameState->CurrentPlaylistInfo.OverridePlaylist = Playlist;
			GameState->CurrentPlaylistInfo.PlaylistReplicationKey++;
			GameState->CurrentPlaylistInfo.MarkArrayDirty();
			GameState->OnRep_CurrentPlaylistInfo();

			GameMode->CurrentPlaylistName = Playlist->PlaylistName;
			//GameMode->bAlwaysDBNO = Playlist->MaxSquadSize > 1; might be our issue mr tech
			GameMode->WarmupRequiredPlayerCount = 1;

			GameMode->bDBNOEnabled = Playlist->MaxTeamSize > 1;
			GameMode->bAlwaysDBNO = Playlist->MaxTeamSize > 1;
			GameState->bDBNODeathEnabled = Playlist->MaxTeamSize > 1;
			GameState->SetIsDBNODeathEnabled(Playlist->MaxTeamSize > 1);

			GameMode->GameSession->MaxPlayers = Playlist->MaxPlayers;
			GameMode->GameSession->MaxSpectators = 0;
			GameMode->GameSession->MaxPartySize = Playlist->MaxSquadSize;
			GameMode->GameSession->MaxSplitscreensPerConnection = 2;
			GameMode->GameSession->bRequiresPushToTalk = false;
			GameMode->GameSession->SessionName = UKismetStringLibrary::Conv_StringToName(FString(L"GameSession"));

			auto TS = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
			auto DR = 60.f;

			GameState->WarmupCountdownEndTime = TS + DR;
			GameMode->WarmupCountdownDuration = DR;
			GameState->WarmupCountdownStartTime = TS;
			GameMode->WarmupEarlyCountdownDuration = DR;

			GameState->CurrentPlaylistId = Playlist->PlaylistId;
			GameState->OnRep_CurrentPlaylistId();

			GameState->bGameModeWillSkipAircraft = Playlist->bSkipAircraft;
			GameState->CachedSafeZoneStartUp = Playlist->SafeZoneStartUp;
			GameState->AirCraftBehavior = Playlist->AirCraftBehavior;
			GameState->OnRep_Aircraft();

			Log("Setup Playlist!");
		}

		if (!GameState->MapInfo) { // cant listen without map being fully loaded
			return false;
		}

		static bool Listening = false;
		if (!Listening) {
			Listening = true;

			auto Beacon = SpawnActor<AFortOnlineBeaconHost>({});
			Beacon->ListenPort = 7777;
			InitHost(Beacon);
			PauseBeaconRequests(Beacon, false);

			UWorld::GetWorld()->NetDriver = Beacon->NetDriver;
			UWorld::GetWorld()->NetDriver->World = UWorld::GetWorld();
			UWorld::GetWorld()->NetDriver->NetDriverName = UKismetStringLibrary::GetDefaultObj()->Conv_StringToName(L"GameNetDriver");

			FString Error;
			FURL url = FURL();
			url.Port = 7777;

			if (!InitListen(UWorld::GetWorld()->NetDriver, UWorld::GetWorld(), url, true, Error))
			{
				Log("InitListen Failed!");
			}
			else {
				Log("InitListen successful!");
			}

			SetWorld(UWorld::GetWorld()->NetDriver, UWorld::GetWorld());

			for (size_t i = 0; i < UWorld::GetWorld()->LevelCollections.Num(); i++) {
				UWorld::GetWorld()->LevelCollections[i].NetDriver = UWorld::GetWorld()->NetDriver;
			}

			SetWorld(UWorld::GetWorld()->NetDriver, UWorld::GetWorld());

			for (int i = 0; i < GameState->CurrentPlaylistInfo.BasePlaylist->AdditionalLevels.Num(); i++)
			{
				FVector Loc{};
				FRotator Rot{};
				bool bSuccess = false;
				((ULevelStreamingDynamic*)ULevelStreamingDynamic::StaticClass()->DefaultObject)->LoadLevelInstanceBySoftObjectPtr(UWorld::GetWorld(), GameState->CurrentPlaylistInfo.BasePlaylist->AdditionalLevels[i], Loc, Rot, &bSuccess, FString());
				FAdditionalLevelStreamed NewLevel{};
				NewLevel.LevelName = GameState->CurrentPlaylistInfo.BasePlaylist->AdditionalLevels[i].ObjectID.AssetPathName;
				NewLevel.bIsServerOnly = false;
				GameState->AdditionalPlaylistLevelsStreamed.Add(NewLevel);
			}

			for (int i = 0; i < GameState->CurrentPlaylistInfo.BasePlaylist->AdditionalLevelsServerOnly.Num(); i++)
			{
				FVector Loc{};
				FRotator Rot{};
				bool bSuccess = false;
				((ULevelStreamingDynamic*)ULevelStreamingDynamic::StaticClass()->DefaultObject)->LoadLevelInstanceBySoftObjectPtr(UWorld::GetWorld(), GameState->CurrentPlaylistInfo.BasePlaylist->AdditionalLevelsServerOnly[i], Loc, Rot, &bSuccess, FString());
				FAdditionalLevelStreamed NewLevel{};
				NewLevel.LevelName = GameState->CurrentPlaylistInfo.BasePlaylist->AdditionalLevelsServerOnly[i].ObjectID.AssetPathName;
				NewLevel.bIsServerOnly = true;
				GameState->AdditionalPlaylistLevelsStreamed.Add(NewLevel);
			}
			GameState->OnRep_AdditionalPlaylistLevelsStreamed();
			GameState->OnFinishedStreamingAdditionalPlaylistLevel();

			GameMode->bWorldIsReady = true;

			Log("Listening: 7777");
			SetConsoleTitleA("OGS 18.40 | Listening: 7777");
		}

		if (GameState->PlayersLeft > 0)
		{
			return true;
		}
		else
		{
			auto TS = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
			auto DR = 60.f;

			GameState->WarmupCountdownEndTime = TS + DR;
			GameMode->WarmupCountdownDuration = DR;
			GameState->WarmupCountdownStartTime = TS;
			GameMode->WarmupEarlyCountdownDuration = DR;
		}

		return false;
	}

	inline APawn* SpawnDefaultPawnFor(AFortGameMode* GameMode, AFortPlayerController* Player, AActor* StartingLoc)
	{
		FTransform Transform = StartingLoc->GetTransform();
		return (AFortPlayerPawnAthena*)GameMode->SpawnDefaultPawnAtTransform(Player, Transform);
	}

	inline __int64 PickTeam(__int64 a1, unsigned __int8 a2, __int64 a3)
	{
		return 3;
	};

	void Hook() {
		MH_CreateHook((LPVOID)(ImageBase + 0x4A80D0C), ReadyToStartMatch, (LPVOID*)&ReadyToStartMatchOG);
		MH_CreateHook((LPVOID)(ImageBase + 0x4a8abf8), SpawnDefaultPawnFor, nullptr);
		MH_CreateHook((LPVOID)(ImageBase + 0x4a7b6d0), PickTeam, nullptr);

		Log("Gamemode Hooked!");
	}
}