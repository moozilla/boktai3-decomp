#include "global.h"
#include "scene_event_dispatch.h"

#define SparkleEmitter_ProcessPendingTriggers sub_081C2C04

struct SparkleTriggerCommand {
    struct SceneEventCommand base;
    u8 rotation;
    u8 unused9;
    s16 duration;
};

extern struct SceneEventDispatchState *gUnk_0200047C;

void AnimData_SelectPaletteBank(void *, u32);
void Sound_PauseChannel12(s32);
void Sound_PlaySong(u32);
u32 Trigger_ConsumePending(void *, u32);
void SparkleEmitter_UpdateEmitting(void *);
void SparkleEmitter_UpdateDraining(void *);

void SparkleEmitter_ProcessPendingTriggers(void *p0)
{
    u8 *p = (u8 *)p0;
    struct SceneEventReceiver *receiver = (struct SceneEventReceiver *)(p + 0x734);
    s32 i = 0;

    goto test;
    for (;;) {
        u8 phase = gUnk_0200047C->phase;
        struct SparkleTriggerCommand *cmd =
            (struct SparkleTriggerCommand *)receiver->pending[phase][i];
        SceneMessages_SetPendingTrigger(receiver, &cmd->base);
        if (cmd->base.kind == 12) {
            s16 v776 = cmd->base.value;
            u32 rot = cmd->rotation + 5;
            u16 v774 = (rot << 29) >> 24;
            s32 v768 = cmd->duration;
            *(s16 *)(p + 0x776) = v776;
            *(u16 *)(p + 0x774) = v774;
            *(s32 *)(p + 0x768) = v768;
            if ((u32)v768 <= 14)
                AnimData_SelectPaletteBank(p + 0x718, (u32)v768 + 0x255);
            Sound_PlaySong(0x83);
            {
                void (*fn)(void *) = SparkleEmitter_UpdateEmitting;
                *(void (**)(void *))(p + 0x77C) = fn;
            }
            *(u32 *)(p + 0x770) = 0;
            Trigger_ConsumePending(receiver, 1);
        } else if (cmd->base.kind == 13) {
            Sound_PauseChannel12(8);
            {
                void (*fn)(void *) = SparkleEmitter_UpdateDraining;
                *(void (**)(void *))(p + 0x77C) = fn;
            }
            *(u32 *)(p + 0x770) = 0;
            Trigger_ConsumePending(receiver, 1);
        }
        i++;
test:
        if (i < receiver->count[gUnk_0200047C->phase])
            continue;
        break;
    }
}
