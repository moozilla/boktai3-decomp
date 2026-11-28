#include "global.h"
#include "engine/video/render.h"
#include "engine/engine_pause_flags.h"
#include "ui/pause/scene_hud_display.h"
#include "player/player.h"
#include "engine/engine.h"
#include "gba/io_reg.h"
void Hud_InitSelectionBounds(u32, u32);
void Hud_UpdateEquipmentPrompt(struct SceneHudState *);
void Hud_SetUpdateCallback(struct SceneHudState *, void (*)(struct SceneHudState *));
void Sound_ResumePersistentSongs(void);
void HealthBurstEffect_Create(struct Player *, u32);
struct Args {
    u16 count;
    u16 pad;
    s32 *values;
};
void EffectManager_SpawnParticles(u32, u32, u32, struct SceneVec *, struct SceneVec *, u32, u32);
void SceneHud_FinishPageExit(struct SceneHudState *hud)
{
    Hud_InitSelectionBounds(hud->pageFadeTimer * 4, 0x1084);
    if (!hud->pageFadeTimer) {
        REG_BLDCNT = hud->savedBlendControl;
        if (hud->savedDisplayControl & 0x200)
            gDispCntShadow |= 0x200;
        if (hud->savedDisplayControl & 0x400)
            gDispCntShadow |= 0x400;
        if (hud->savedDisplayControl & 0x800)
            gDispCntShadow |= 0x800;
    }
    if (++hud->pageFadeTimer > 16) {
        Hud_SetUpdateCallback(hud, Hud_UpdateEquipmentPrompt);
        gBgFadeColor = hud->savedFade[0];
        gBgFadeBankMask = hud->savedFade[1];
        gObjFadeColor = hud->savedFade[2];
        gObjFadeFlags = hud->savedFade[3];
        Engine_ClearPauseFlags(4);
        Sound_ResumePersistentSongs();
        if (hud->secondaryEffectRequested) {
            HealthBurstEffect_Create(hud->actionOwner, hud->secondaryEffectMode);
            hud->secondaryEffectRequested = 0;
        }
        if (hud->primaryEffectRequested) {
            if (hud->pageExitScript) {
                struct Args args;
                s32 value;
                args.count = 1;
                value = hud->primaryEffectMode;
                args.values = &value;
                Script_Call(hud->pageExitScript, &args);
            }
            hud->primaryEffectMode = 0;
            hud->primaryEffectRequested = 0;
        }
        if (hud->particleEffectRequested) {
            struct SceneVec position, size;
            s32 i;
            position = hud->actionOwner->position;
            position.x -= 512;
            position.z -= 512;
            position.y -= 256;
            size.x = 1024;
            size.z = 1024;
            size.y = 512;
            for (i = 0; i < 3; i++)
                EffectManager_SpawnParticles(4, hud->particleEffectMode, hud->particleEffectKind,
                                             &position, &size, 300, 120);
            hud->particleEffectMode = 0;
            hud->particleEffectRequested = 0;
        }
        if (hud->effectActive) {
            /* Tests one script and runs the other, as shipped. */
            if (hud->pageExitScript) {
                struct Args args;
                s32 value;
                args.count = 1;
                value = hud->effectActive - 1;
                args.values = &value;
                Script_Call(hud->effectScript, &args);
            }
            hud->effectActive = 0;
        }
    }
}
