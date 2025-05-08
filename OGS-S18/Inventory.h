#pragma once
#include "framework.h"

namespace Inventory {
	inline void GiveItem(AFortPlayerController* PC, UFortItemDefinition* Def, int Count, int LoadedAmmo)
	{
		UFortWorldItem* Item = Cast<UFortWorldItem>(Def->CreateTemporaryItemInstanceBP(Count, 0));
		Item->SetOwningControllerForTemporaryItem(PC);
		Item->OwnerInventory = PC->WorldInventory;
		Item->ItemEntry.LoadedAmmo = LoadedAmmo;

		PC->WorldInventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
		PC->WorldInventory->Inventory.ItemInstances.Add(Item);
		PC->WorldInventory->Inventory.MarkItemDirty(Item->ItemEntry);
		PC->WorldInventory->HandleInventoryLocalUpdate();
	}

	bool CompareGuids(FGuid Guid1, FGuid Guid2) {
		if (Guid1.A == Guid2.A
			&& Guid1.B == Guid2.B
			&& Guid1.C == Guid2.C
			&& Guid1.D == Guid2.D) {
			return true;
		}
		else {
			return false;
		}
	}

	static void ServerExecuteInventoryItem(AFortPlayerControllerAthena* PC, FGuid Guid)
	{
		Log("ServerExecuteInventoryItem Called!");
		if (!PC || !PC->MyFortPawn)
			return;

		for (size_t i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
		{
			if (!PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition)
				continue;
			
			FGuid ItemGuid = PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid;
			if (CompareGuids(Guid, ItemGuid)) {
				UFortWeaponItemDefinition* ItemDef = (UFortWeaponItemDefinition*)PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition;
				FGuid TrackerGuid = PC->WorldInventory->Inventory.ReplicatedEntries[i].TrackerGuid;

				PC->MyFortPawn->EquipWeaponDefinition(ItemDef, Guid, TrackerGuid, false);
			}
		}

		return;
	}

	void Hook() {
		//MH_CreateHook((LPVOID)(ImageBase + 0xc57d70), ServerExecuteInventoryItem, nullptr);
		HookVTable(AFortPlayerControllerAthena::GetDefaultObj(), 0x21d, ServerExecuteInventoryItem, nullptr);

		Log("Inventory Hooked!");
	}
}