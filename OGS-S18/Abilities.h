#pragma once
#include "framework.h"

namespace Abilities {
	static void(*GiveAbility)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle*, FGameplayAbilitySpec) = decltype(GiveAbility)(ImageBase + 0x3d53ee0);
	static void (*AbilitySpecCtor)(FGameplayAbilitySpec*, UGameplayAbility*, int, int, UObject*) = decltype(AbilitySpecCtor)(ImageBase + 0x3d47310);
	static bool (*InternalTryActivateAbility)(UAbilitySystemComponent* AbilitySystemComp, FGameplayAbilitySpecHandle AbilityToActivate, FPredictionKey InPredictionKey, UGameplayAbility** OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData) = decltype(InternalTryActivateAbility)(ImageBase + 0x3d555ec);
	static FGameplayAbilitySpecHandle(*GiveAbilityAndActivateOnce)(UAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle*, FGameplayAbilitySpec) = decltype(GiveAbilityAndActivateOnce)(ImageBase + 0x3d54000);

	inline void InitAbilitiesForPlayer(AFortPlayerController* PC)
	{
		auto PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;
		static auto AbilitySet = StaticLoadObject<UFortAbilitySet>("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_AthenaPlayer.GAS_AthenaPlayer");

		if (PlayerState && AbilitySet)
		{
			for (size_t i = 0; i < AbilitySet->GameplayAbilities.Num(); i++)
			{
				FGameplayAbilitySpec Spec{};
				AbilitySpecCtor(&Spec, (UGameplayAbility*)AbilitySet->GameplayAbilities[i].Get()->DefaultObject, 1, -1, nullptr);
				GiveAbility(PlayerState->AbilitySystemComponent, &Spec.Handle, Spec);
			}
		}
	}

	inline FGameplayAbilitySpec* FindAbilitySpecFromHandle(UFortAbilitySystemComponentAthena* ASC, FGameplayAbilitySpecHandle& Handle)
	{
		for (size_t i = 0; i < ASC->ActivatableAbilities.Items.Num(); i++)
		{
			if (ASC->ActivatableAbilities.Items[i].Handle.Handle == Handle.Handle)
				return &ASC->ActivatableAbilities.Items[i];
		}
		return nullptr;
	}


	inline void InternalServerTryActivateAbility(UFortAbilitySystemComponentAthena* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle, bool InputPressed, FPredictionKey& PredictionKey, FGameplayEventData* TriggerEventData) // Broo what is this thing
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilitySystemComponent, Handle);
		if (!Spec)
			return AbilitySystemComponent->ClientActivateAbilityFailed(Handle, PredictionKey.Current);


		UGameplayAbility* AbilityToActivate = Spec->Ability;

		UGameplayAbility* InstancedAbility = nullptr;
		Spec->InputPressed = true;

		if (!InternalTryActivateAbility(AbilitySystemComponent, Handle, PredictionKey, &InstancedAbility, nullptr, TriggerEventData))
		{
			AbilitySystemComponent->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
			Spec->InputPressed = false;
		}
		AbilitySystemComponent->ActivatableAbilities.MarkItemDirty(*Spec);
	}

	void Hook()
	{
		HookVTable(UAbilitySystemComponent::GetDefaultObj(), 0xfe, InternalServerTryActivateAbility, nullptr);
		HookVTable(UFortAbilitySystemComponent::GetDefaultObj(), 0xfe, InternalServerTryActivateAbility, nullptr);
		HookVTable(UFortAbilitySystemComponentAthena::GetDefaultObj(), 0xfe, InternalServerTryActivateAbility, nullptr);

		Log("Abilities Hooked!");
	}
}