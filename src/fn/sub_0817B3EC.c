#include "global.h"
#define gSceneScriptFlags gUnk_02000560
struct EffectScriptActor {
    u8 unknown0[0x51];
    u8 scriptArgument;
    u8 unknown52[2];
    s16 x, y, z;
    u8 unknown5A[0x50];
    u8 animation;
    u8 unknownAB[4];
    u8 scriptReady, enter;
    u8 unknownB1[0x13];
    u32 timer;
    u8 unknownC8[4];
    u8 defaultDirection;
    u8 unknownCD[0x47];
    u8 direction;
    u8 unknown115[0x3D5];
    u8 scriptIndex;
    u8 unknown4EB[0x775];
    u32 scripts[1];
};
struct EffectScriptArgs { u32 count; s32 *values; };
extern u32 gUnk_02000560;
s32 Player_IsSlotInActionTwelveOrThirtyEight(s32);
void Script_Call(u32, struct EffectScriptArgs *);
static inline u8 enter_idle(struct EffectScriptActor *actor)
{
    if (actor->enter) {
        actor->enter = 0;
        actor->scriptReady = 0;
        return 1;
    }
    return 0;
}
void ActorEffect_UpdateIdleScript(struct EffectScriptActor *actor)
{
    /* The ROM writes values but passes an uninitialized descriptor, as in
     * DamageController_Update. Retain the observed original behavior. */
    struct EffectScriptActor *scriptsActor = actor;
    volatile s32 values[8];
    struct EffectScriptArgs args;
    if (enter_idle(actor)) {
        actor->direction = actor->defaultDirection;
        {
            u32 animation = 14;
            actor->animation = animation;
        }
    } else if (actor->scriptReady) {
        s32 busy = Player_IsSlotInActionTwelveOrThirtyEight(0);
        if (!busy) {
            u32 script = scriptsActor->scripts[scriptsActor->scriptIndex];
            if (script) {
                u32 argument = actor->scriptArgument;
                s16 *position = &actor->x;
                u32 mask = 4;
                if (!(gUnk_02000560 & mask)) {
                    values[0] = position[0];
                    values[1] = position[1];
                    values[2] = position[2];
                    values[3] = argument;
                    Script_Call(script, &args);
                }
                scriptsActor->scripts[scriptsActor->scriptIndex] = busy;
            }
        }
    }
    actor->timer++;
}
