#include "global.h"
#define SessionMenu_SetState Registry1745_SetState
#define SessionMenu_ResetWithCallback Registry1745_BeginShutdown
#define gSessionMenu                  gUnk_020004B4
#include "area_effect_link.h"
#define SessionMenu_WaitForSelection sub_0810A3E0
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void Registry1745_SetState(void *, u32, void (*)(void));
void sub_08109C80(void);
void sub_0810A448(void);
void sub_0810A540(void);
void RegistryF246_Close(void);
s32 sub_0810A3E0(struct AreaEffectLinkState *p)
{
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, sub_08109C80);
        return -1;
    }
    if (p->menuElapsed > 120) {
        if (gUnk_020004B4 && gUnk_020004B4->localPlayer == 0) {
            Registry1745_SetState(p, 7, sub_0810A448);
            RegistryF246_Close();
        } else {
            Registry1745_SetState(p, 8, sub_0810A540);
        }
    }
    return 0;
}
