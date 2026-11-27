#include "bosses/vanargand/scene_departure_display.h"
#include "effects/screen/background_controller.h"
#include "engine/video/render.h"

s32 SceneActors_Initialize(void *, s32, s32, void *, s32);
void SceneActors_Register(void *, u32);
s32 SceneActors_SetHealthLimits(void *, s32, s32, s32);
void VanargandHead_InitSecondaryCollider(void *, s32);
void VanargandHead_InitCollider(void *);
void VanargandHead_InitContactCollider(void *, s32);
void VanargandHead_RegisterRenderCallback(void *);
void VanargandHead_InitBackgroundAndMotion(void *);
void VanargandHead_InitDisplaySprites(void *);
void VanargandHead_InitResources(void *);
void *VanargandHead_CreateEffectPool(void);
s32 sub_081F0678(void);
extern u16 gUnk_03005040[4][16];

static inline void SetResponse(s32 (*fn)(void), s32 (**slot)(void))
{
    *slot = fn;
}

s32 VanargandHead_InitFromScript(struct DepartureActor *actor, s32 id)
{
    s32 health = 2000;
    s32 extra = 100;
    s32 zero = 0;
    s32 type = 7;
    struct DepartureActor *owner = actor;
    s32 mode = 0;
    s32 shared = 0;
    s32 hasShared = 0;
    struct SceneVec vec;
    u8 *modeField, *modePtr;
    u32 *flags;
    s32 i;

    if (Script_SeekToKeyword(0x65))
        actor->setting = Script_GetValue();
    if (Script_SeekToKeyword(0x61)) {
        health = Script_GetValue();
        extra = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x64)) {
        actor->parameterA = Script_GetValue();
        actor->parameterB = Script_GetValue();
        actor->parameterC = Script_GetValue();
        actor->parameterD = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x77)) {
        actor->initialPosition.x = Script_GetValue();
        actor->initialPosition.y = Script_GetValue();
        actor->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x4D))
        mode = Script_GetValue();
    if (Script_SeekToKeyword(0x70)) {
        shared = Script_GetValue();
        hasShared = 1;
    }
    if (gSceneCamera)
        actor->cameraActive = 1;
    else
        actor->cameraActive = zero;
    SceneActors_Initialize(owner, id, type, &vec, zero);
    owner->messageType = zero;
    SceneActors_Register(actor, type);
    {
        s32 v = Script_GetKeywordValue(0x69, 0);
        modePtr = &owner->motionMode;
        *modePtr = v;
    }
    SetResponse(sub_081F0678, &owner->response);
    SceneActors_SetHealthLimits(owner, health, health, extra);
    actor->healthSeventh = Div(health, 7);
    actor->healthTenth = Div(health, 10);
    VanargandHead_InitSecondaryCollider(actor, extra);
    VanargandHead_InitCollider(actor);
    VanargandHead_InitContactCollider(actor, extra);
    VanargandHead_RegisterRenderCallback(actor);
    VanargandHead_InitBackgroundAndMotion(actor);
    actor->effectGraphics = 0xEBF6;
    actor->sequenceResource = ScriptProc_Start(0x922E, 0xEBF6);
    actor->effectResource = 0x2E90;
    VanargandHead_InitDisplaySprites(actor);
    modeField = modePtr;
    /* The motion mode is 0x17 bytes above the actor flags. */
    flags = (u32 *)(modePtr - 0x17);
    {
        s32 fillIndex;
        for (fillIndex = 0; fillIndex < 16; fillIndex++)
            actor->flashPalette[fillIndex] = 0x7FFF;
    }
    VanargandHead_InitResources(actor);
    {
        u32 mask = 0x10;
        *flags = mask | *flags;
    }
    if (*modeField == 0)
        DepartureActor_StartPhase(owner, 0, 0, 5);
    else {
        if (hasShared) {
            actor->screen.x = shared;
            actor->targetX = actor->screen.x;
            actor->targetMode = 1;
            DepartureActor_StartPhase(owner, 1, 0, 3);
        } else {
            actor->screen.x = hasShared;
            DepartureActor_StartPhase(owner, 1, 0, 2);
        }
        if (gSceneCamera)
            Sound_PlaySong(0x586);
    }
    actor->invincibilityTimer = 0;
    actor->effectPool = VanargandHead_CreateEffectPool();
    actor->fadeShift = 7;
    actor->fadeFrame = 0x80;
    if (*modeField && mode > 0) {
        u8 holdSequence = 1;
        CpuSet(actor->fadeTo, gUnk_03005040, 0x40);
        AnimData_SelectPaletteBank(&actor->foregroundGraphics, 0x1F6);
        CpuSet(actor->foregroundGraphics.paletteData, actor->foregroundPalette, 16);
        AnimData_SelectPaletteBank(&actor->accentGraphics, 0x1F5);
        CpuSet(actor->accentGraphics.paletteData, actor->accentPalette, 16);
        actor->foregroundGraphics.paletteData = actor->foregroundPalette;
        actor->accentGraphics.paletteData = actor->accentPalette;
        actor->foregroundBank = 0x1F6;
        actor->accentBank = 0x1F5;
        actor->foreground.mode = holdSequence;
        actor->accent.mode = holdSequence;
        if (mode == 2) {
            actor->dispersalTimer = 60;
            actor->alternateLayer = 1;
        } else
            actor->alternateLayer = 0;
    } else {
        u16 *src = gBgPaletteBuffer;
        for (i = 0; i < 4; i++)
            CpuSet(src + 64 + i * 16, actor->savedPalettes[i], 16);
        actor->foregroundBank = 3;
        actor->accentBank = 2;
    }
    actor->phaseTimer2 = 0;
    return 0;
}
