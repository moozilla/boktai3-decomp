#include "global.h"
#include "ram.h"
#include "render.h"
void Palette_ResetObjectAllocation(void);
void ObjGfx_ResetAllocationCounters(void);
void sub_08249238(u32);
void SpriteOam_FinalizeQueue(void);
void sub_08217C7C(void);
void sub_08217BBC(void);
void Palette_RefreshObjectBanks(void);
void Palette_ApplyBackgroundFade(void);
void sub_08217430(void);
void SpriteOam_ResetQueue(u32);
u32 InterruptsAndVideo_OnGraphicsUpdateTask(void)
{
    Palette_ResetObjectAllocation();
    ObjGfx_ResetAllocationCounters();
    sub_08249238(gSpriteRenderCallback);
    sub_08249238(gExtraRenderCallback);
    sub_08249238(gProjectedSpriteCallback);
    SpriteOam_FinalizeQueue();
    if ((*(s32 *)&gGrayscaleControl)) {
        sub_08217C7C();
        sub_08217BBC();
        if ((*(s32 *)&gGrayscaleControl) > 0)
            (*(s32 *)&gGrayscaleControl)--;
        if (!(*(s32 *)&gGrayscaleControl))
            Palette_RefreshObjectBanks();
    } else {
        Palette_ApplyBackgroundFade();
        sub_08217430();
    }
    SpriteOam_ResetQueue(0);
    gSceneEffectPalettePhase++;
    return 0;
}
