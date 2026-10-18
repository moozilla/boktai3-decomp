#include "global.h"
#include "scene_panel_schedule.h"
#define DirectionBurst_Emit                sub_081A376C
#define DirectionBurst_CreateSharedEmitter Hresvelgr_CreateShotPool
#define DirectionBurst_EmitConfigured      sub_081A3634
#define gDirectionBurstEmitter             gUnk_02000238
extern void *gUnk_02000238;
void sub_081A3634(const void *, u32, u32, u32, u32, u32, u32, u32);
void sub_081A376C(const void *origin, u32 angle, u32 kind, u32 parameter)
{
    if (!gDirectionBurstEmitter) {
        gDirectionBurstEmitter = DirectionBurst_CreateSharedEmitter(0, 0, 0);
        if (!gDirectionBurstEmitter)
            return;
    }
    DirectionBurst_EmitConfigured(origin, angle, kind, 0, 0, 0, parameter, 0);
}
