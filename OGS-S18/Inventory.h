#pragma once
#include "framework.h"

namespace Inventory {
	inline void GiveItem(AFortPlayerController* PC, UFortItemDefinition* Def, int Count, int LoadedAmmo)
	{
		if (!PC || !Def)
			return;

		UFortWorldItem* Item = Cast<UFortWorldItem>(Def->CreateTemporaryItemInstanceBP(Count, 0));
		Item->SetOwningControllerForTemporaryItem(PC);
		Item->OwnerInventory = PC->WorldInventory;
		Item->ItemEntry.LoadedAmmo = LoadedAmmo;

		PC->WorldInventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
		PC->WorldInventory->Inventory.ItemInstances.Add(Item);
		PC->WorldInventory->Inventory.MarkItemDirty(Item->ItemEntry);
		PC->WorldInventory->HandleInventoryLocalUpdate();
	}
}