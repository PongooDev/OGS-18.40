#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <map>
#include "minhook/MinHook.h"
#include "SDK/SDK.hpp"

#pragma comment(lib, "minhook/minhook.lib")

using namespace SDK;

static auto ImageBase = InSDKUtils::GetImageBase();

static bool (*InitListen)(void*, void*, FURL&, bool, FString&) = decltype(InitListen)(ImageBase + 0x3B27284);
static void (*SetWorld)(void*, void*) = decltype(SetWorld)(ImageBase + 0x11B826C);
static bool (*InitHost)(UObject* Beacon) = decltype(InitHost)(ImageBase + 0x3b26ed8);
static void (*PauseBeaconRequests)(UObject* Beacon, bool bPause) = decltype(PauseBeaconRequests)(ImageBase + 0x50a55e0);

inline bool (*InternalTryActivateAbility)(UAbilitySystemComponent* AbilitySystemComp, FGameplayAbilitySpecHandle AbilityToActivate, FPredictionKey InPredictionKey, UGameplayAbility** OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData) = decltype(InternalTryActivateAbility)(ImageBase + 0x3d555ec);
static inline void (*GiveAbilityAndActivateOnce)(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle* Spec, FGameplayAbilitySpec GameplayEventData) = decltype(GiveAbilityAndActivateOnce)(ImageBase + 0x3d54000);
static inline __int64 (*CombineStruct)(void* FGameplayAbilitySpec, UGameplayAbility* InAbility, char InLevel, int InInputID, void* InSourceObject) = decltype(CombineStruct)(InSDKUtils::GetImageBase() + 0x3d47310);

static void* (*StaticFindObjectOG)(UClass*, UObject* Package, const wchar_t* OrigInName, bool ExactClass) = decltype(StaticFindObjectOG)(ImageBase + 0xd13c3c);
static void* (*StaticLoadObjectOG)(UClass* Class, UObject* InOuter, const TCHAR* Name, const TCHAR* Filename, uint32_t LoadFlags, UObject* Sandbox, bool bAllowObjectReconciliation, void*) = decltype(StaticLoadObjectOG)(ImageBase + 0x14ad4a0);

void Log(const std::string& msg)
{
	static bool firstCall = true;

	if (firstCall)
	{
		std::ofstream logFile("OGS_log.txt", std::ios::trunc);
		if (logFile.is_open())
		{
			logFile << "[OGS]: Log file initialized\n";
			logFile.close();
		}
		firstCall = false;
	}

	std::ofstream logFile("OGS_log.txt", std::ios::app);
	if (logFile.is_open())
	{
		logFile << "[OGS]: " << msg << std::endl;
		logFile.close();
	}

	std::cout << "[OGS]: " << msg << std::endl;
}

void HookVTable(void* Base, int Idx, void* Detour, void** OG)
{
	DWORD oldProtection;

	void** VTable = *(void***)Base;

	if (OG)
	{
		*OG = VTable[Idx];
	}

	VirtualProtect(&VTable[Idx], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection);

	VTable[Idx] = Detour;

	VirtualProtect(&VTable[Idx], sizeof(void*), oldProtection, NULL);
}

template <typename T>
static inline T* StaticFindObject(std::wstring ObjectName)
{
	return (T*)StaticFindObjectOG(T::StaticClass(), nullptr, ObjectName.c_str(), false);
}

template<typename T>
inline T* Cast(UObject* Object)
{
	if (!Object || !Object->IsA(T::StaticClass()))
		return nullptr;
	return (T*)Object;
}

template<typename T = UObject>
static inline T* StaticLoadObject(const std::string& Name)
{
	auto ConvName = std::wstring(Name.begin(), Name.end());

	T* Object = StaticFindObject<T>(ConvName);

	if (!Object)
	{
		Object = (T*)StaticLoadObjectOG(T::StaticClass(), nullptr, ConvName.c_str(), nullptr, 0, nullptr, false, nullptr);
	}

	return Object;
}

static inline FQuat FRotToQuat(FRotator Rotation) {
	FQuat Quat;
	const float DEG_TO_RAD = 3.14159f / 180.0f;
	const float DIVIDE_BY_2 = DEG_TO_RAD / 2.0f;

	float SP = sin(Rotation.Pitch * DIVIDE_BY_2);
	float CP = cos(Rotation.Pitch * DIVIDE_BY_2);
	float SY = sin(Rotation.Yaw * DIVIDE_BY_2);
	float CY = cos(Rotation.Yaw * DIVIDE_BY_2);
	float SR = sin(Rotation.Roll * DIVIDE_BY_2);
	float CR = cos(Rotation.Roll * DIVIDE_BY_2);

	Quat.X = CR * SP * SY - SR * CP * CY;
	Quat.Y = -CR * SP * CY - SR * CP * SY;
	Quat.Z = CR * CP * SY - SR * SP * CY;
	Quat.W = CR * CP * CY + SR * SP * SY;

	return Quat;
}

template<typename T>
inline T* SpawnActor(FVector Loc, FRotator Rot = FRotator(), AActor* Owner = nullptr, SDK::UClass* Class = T::StaticClass(), ESpawnActorCollisionHandlingMethod Handle = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn)
{
	FTransform Transform{};
	Transform.Scale3D = FVector{ 1,1,1 };
	Transform.Translation = Loc;
	Transform.Rotation = FRotToQuat(Rot);

	return (T*)UGameplayStatics::FinishSpawningActor(UGameplayStatics::BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), Class, Transform, Handle, Owner), Transform);
}