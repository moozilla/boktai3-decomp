#define SceneActor_BeginGuardedReaction sub_080E7D9C
#include "global.h"
#include "scene_actor_reset.h"
void sub_0807FC90(void);
void sub_080E7D9C(struct SceneActorRegistryResetView *p)
{
    u32 mask = 0x80;
    u32 flags = (u32)&p->flags2D4;
    u32 value = *(u16 *)flags;
    if (value & mask) {
        u16 *pending = &p->group.field418;
        if (!*pending) {
            u32 zero = value & 0x38;
            if (!zero) {
                struct SceneActorReactionMotion *motion = (void *)&p->fieldDC;
                struct SceneActorRegistryControl *control;
                u8 byteZero = 0;
                motion->fieldDC = zero;
                p->field2C2 = 64;
                p->group.flags414 = zero;
                *pending = zero;
                {
                    s32 clear = ~0x380;
                    *(u16 *)flags = clear & *(u16 *)flags;
                }
                {
                    s32 clear = ~0x18;
                    motion->flags = clear & motion->flags;
                }
                control = &p->group.control;
                {
                    u32 mask = 0x200000;
                    if (p->flags2CC & mask) {
                        u32 *wordFlags = &p->flags2D0;
                        u32 clear = ~0x1000000;
                        *wordFlags &= clear;
                        {
                            s32 clear = ~0x200;
                            *(u16 *)flags = clear & *(u16 *)flags;
                        }
                        control->mode = byteZero;
                        control->counter = zero;
                        control->timer = zero;
                    }
                }
                {
                    s32 clear = ~0x400;
                    *(u16 *)flags = clear & *(u16 *)flags;
                }
                p->field5DA = 255;
                {
                    void *callback = (void *)sub_0807FC90;
                    u32 kind = 16;
                    u32 preparedOffset = 0x2B2;
                    u8 *field = (u8 *)p + preparedOffset;
                    u32 one = 1;
                    *field = one;
                    {
                        flags = 0x2B0;
                        *((u8 *)p + flags) = byteZero;
                        flags += 5;
                        *((u8 *)p + flags) = kind;
                    }
                    p->target = callback;
                    p->field2C8 = zero;
                    {
                        u32 *field;
                        u32 clear;
                        flags = 0x114;
                        field = (void *)((u8 *)p + flags);
                        clear = ~1;
                        *field &= clear;
                    }
                    p->word104 = zero;
                    {
                        u32 stateOffset;
                        kind = 17;
                        stateOffset = 0x2A8;
                        *((u8 *)p + stateOffset) = byteZero;
                        flags = 0x2A9;
                        *((u8 *)p + flags) = byteZero;
                        stateOffset += 2;
                        *((u8 *)p + stateOffset) = kind;
                        flags += 3;
                        *(u32 *)((u8 *)p + flags) = zero;
                    }
                    p->field2B1 = one;
                }
            }
        }
    }
}
