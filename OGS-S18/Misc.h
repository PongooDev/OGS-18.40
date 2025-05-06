#pragma once
#include "framework.h"

namespace Misc {
    void nullFunc() {}

    int True() {
        return 1;
    }

    int False() {
        return 0;
    }

    static inline void (*KickPlayerOG)(AGameSession*, AController*);
    static void KickPlayer(AGameSession*, AController*) {
        Log("KickPlayer Called!");
        return;
    }

    void (*DispatchRequestOG)(__int64 a1, unsigned __int64* a2, int a3);
    void DispatchRequest(__int64 a1, unsigned __int64* a2, int a3)
    {
        return DispatchRequestOG(a1, a2, 3);
    }

    void Hook() {
        MH_CreateHook((LPVOID)(ImageBase + 0x11AE108), True, nullptr); // Should be collect garbage if the offset is correct!!!
        MH_CreateHook((LPVOID)(ImageBase + 0x509bf28), KickPlayer, (LPVOID*)&KickPlayerOG); // Kickplayer
        MH_CreateHook((LPVOID)(ImageBase + 0x1023d3c), DispatchRequest, (LPVOID*)&DispatchRequestOG);

        MH_CreateHook((LPVOID)(ImageBase + 0x50af34c), True, nullptr); // reservation kick

        MH_CreateHook((LPVOID)(ImageBase + 0x3079b00), nullFunc, nullptr);
        MH_CreateHook((LPVOID)(ImageBase + 0x5237b2c), nullFunc, nullptr);

        Log("Misc Hooked!");
    }
}