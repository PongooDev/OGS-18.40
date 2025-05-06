#pragma once
#include "framework.h"

namespace PE {
    static void* (*ProcessEventOG)(UObject*, UFunction*, void*);
    void* ProcessEvent(UObject* Obj, UFunction* Function, void* Params)
    {
        //if (Function)
            //Log("PE: " + Function->GetFullName());

        return ProcessEventOG(Obj, Function, Params);
    }

    void Hook() {
        MH_CreateHook((LPVOID)(ImageBase + 0xd6dd3c), ProcessEvent, (LPVOID*)&ProcessEventOG);

        Log("PE Hooked!");
    }
}