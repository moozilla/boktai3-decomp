#define SceneActor_AdvanceEffectTick Player_AdvanceEffectTick
#include "global.h"
#include "engine.h"
struct SceneActorVisualEffects {
    u8 unknown0[0x1C];
    u32 mode;
    u8 unknown20[0x10];
    u16 position[3];
    u8 unknown36[0x10C-0x36];
    u16 alternatePosition[3];
    u8 unknown112[0x457-0x112];
    u8 state;
    u8 unknown458[0x6F0-0x458];
    u16 effectPosition[3];
    u8 unknown6F6[2];
    u16 effectOffset[3];
    u8 unknown6FE[0x7FC-0x6FE];
    u8 callbackContext[0x864-0x7FC];
    void *callback;
    u8 unknown868[0xB04-0x868];
    void *attachment;
    u8 attachmentKind;
    u8 unknownB09;
    u16 attachmentDelay;
};
void sub_0813DB98(void *);
void Player_AdvanceEffectTick(void *);
s32 sub_0813D924(void *, s32);
s32 SceneActor_SelectEffectDisplayMode(void *, s32);
s32 sub_0813DB60(void *, s32);
void sub_0813CE20(void *, s32);
void sub_0813CB8C(void *, u32);
void sub_0813B950(void *);
void Player_UpdateOrbitingEffect(void *);
void sub_0813BAD8(void *);
void ConvergingEffects_UpdateParticles(void *);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, int, int, void *, int, int, int);
void SceneActor_UpdateVisualEffects(struct SceneActorVisualEffects *p)
{
    s32 mode;
    sub_0813DB98(p);
    Player_AdvanceEffectTick(p);
    mode = sub_0813D924(p, 0);
    mode = SceneActor_SelectEffectDisplayMode(p, mode);
    mode = sub_0813DB60(p, mode);
    if (p->mode == 2 && p->state != 10 && p->state != 11)
        sub_0813CE20(p, mode);
    else
        sub_0813CB8C(p, mode);
    sub_0813B950(p);
    Player_UpdateOrbitingEffect(p);
    sub_0813BAD8(p);
    ConvergingEffects_UpdateParticles(p);
    {
        void **callback = &p->callback;
        void *context = p->callbackContext;
        Callback_InvokeActor(context, *callback);
    }
    if (p->attachmentDelay) {
        if (--p->attachmentDelay == 0)
            p->attachment = ActorAttachment_UpdateFromDescriptor(p->attachment, (u8 *)p + 0x24,
                                       p->attachmentKind, 127, 0, 0, 0, 80);
    }
    if (p->state == 38 || p->state == 12) {
        p->effectPosition[0] = p->alternatePosition[0] + p->effectOffset[0];
        p->effectPosition[1] = p->alternatePosition[1] + p->effectOffset[1];
        p->effectPosition[2] = p->alternatePosition[2] + p->effectOffset[2];
    } else {
        p->effectPosition[0] = p->position[0] + p->effectOffset[0];
        p->effectPosition[1] = p->position[1] + p->effectOffset[1];
        p->effectPosition[2] = p->position[2] + p->effectOffset[2];
    }
}
