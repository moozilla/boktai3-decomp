// Tentative descriptive names; inferred from static field accesses and callees.
#include "global.h"
struct ActorAction {
    void (*callback)(void);
    u8 unknown4, mode;
    u8 parameters[0x10 - 2];
    u8 unknown14[0x40];
    u8 active, ready;
    u8 unknown56[6];
    u16 duration, animation;
    void *owner;
};
struct ActorActionHost {
    u8 unknown0[0x55];
    u8 kind;
    u8 unknown56[0x2b0 - 0x56];
    struct ActorAction action;
};
void Memory_Clear(void *, u32);
void ScriptRecord_FindExtendedModifier(void *, s32);
void ActorEmitter_TryEmit(void), ActorEmitter_TryEmitPair(void), ActorEmitter_TryEmitLow(void), ActorEmitter_TryEmitAnimatedPair(void),
    ActorEmitter_UpdateAnimation(void);
void ActorAction_InitializeMode(struct ActorActionHost *host, void *owner)
{
    u8 *source = (u8 *)host + 0x48;
    struct ActorAction *action = &host->action;
    u16 *params;
    action->callback = 0;
    host->action.active = 0;
    action->owner = owner;
    params = (u16 *)((u8 *)host + 0x2b4);
    Memory_Clear(params, 16);
    ScriptRecord_FindExtendedModifier(params, source[13]);
    switch (action->mode) {
        case 0:
            action->callback = ActorEmitter_TryEmit;
            action->ready = 1;
            action->animation = 6;
            action->duration = 0xffff;
            action->active = 1;
            break;
        case 1:
            action->callback = ActorEmitter_TryEmitPair;
            action->ready = 1;
            action->animation = 7;
            action->duration = 0xffff;
            action->active = 1;
            break;
        case 2:
            action->callback = ActorEmitter_TryEmitLow;
            action->ready = 1;
            action->animation = 8;
            action->duration = 0xffff;
            action->active = 1;
            break;
        case 3:
            action->callback = ActorEmitter_TryEmitAnimatedPair;
            action->ready = 1;
            action->animation = 9;
            action->duration = 30;
            action->active = 1;
            break;
        case 4:
            action->callback = ActorEmitter_TryEmitPair;
            action->ready = 1;
            action->animation = 10;
            action->duration = 0xffff;
            action->active = 1;
            break;
        case 5:
            action->callback = ActorEmitter_UpdateAnimation;
            action->ready = 1;
            action->animation = 11;
            action->duration = 31;
            action->active = 1;
            break;
    }
}
