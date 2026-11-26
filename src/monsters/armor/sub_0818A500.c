#include "global.h"
// confidence=low: applies queued scene commands to the armor actor (behavior, attachments, flags).

typedef struct {
    u8 pad0[6];
    u8 count[2];
    s16 *messages[2][4];
} ArmorQueue;

extern u8 *gSceneEventDispatchState;
void SceneMessages_SetPendingTrigger(void *, void *);
void ActorController_SetBehavior(void *, s32);
u32 Trigger_ConsumePending(void *, u32);
void *ActorAttachment_CreateSpriteSlot(u32, void *, void *, s32, s32, u32, void *, void *);
void *ActorAttachment_CreateValueSpriteSlot(u32, void *, void *, s32, s32, u32, s16 *, s32, u32,
                                            void *, void *);
void *ActorAttachment_ReleaseResource(void *, u32, void *);

typedef struct {
    u16 flags;
} ArmorFlags;

static inline void SETF_(u16 *fp)
{
    ((ArmorFlags *)fp)->flags |= 0x400;
}
#define SETF(p) SETF_((u16 *)((p) + 0xB32))

void Armor_ApplySceneCommands(u8 *arg)
{
    u8 *p = arg;
    ArmorQueue *q = (ArmorQueue *)(p + 0xADC);
    s32 i;
    i = 0;
    if (i >= q->count[gSceneEventDispatchState[0x24]])
        return;
    do {
        u8 *scene = gSceneEventDispatchState;
        s16 *m = q->messages[scene[0x24]][i];
        SceneMessages_SetPendingTrigger(q, m);
        switch (((u8 *)m)[6]) {
            case 1:
                ActorController_SetBehavior(p, 0x2a);
                {
                    u32 v = (((u8)m[4] + 5) & 7) << 5;
                    u8 *d = p + 0xB3A;
                    *d = v & 0xc0;
                }
                break;
            case 2:
                ActorController_SetBehavior(p, 0);
                {
                    u32 v = (((u8)m[4] + 5) & 7) << 5;
                    u8 *d = p + 0xB39;
                    *d = v & 0xc0;
                }
                Trigger_ConsumePending((p + 0xADC), 1);
                break;
            case 14:
                ActorController_SetBehavior(p, 0x2d);
                Trigger_ConsumePending((p + 0xADC), 1);
                break;
            case 3: {
                s32 a = m[4];
                s32 b = m[5];
                s32 c = m[6];
                switch (a) {
                    case 4: {
                        s16 *v = (s16 *)(p + 0xB46);
                        u32 z = 0;
                        *v = b;
                        *(void **)(p + 0xB48) = ActorAttachment_CreateValueSpriteSlot(
                            0, p + 0x48, *(void **)(p + 0xB18), 0x2c0, 0x2c0, z, v, c, z, (void *)z,
                            p + 0x3c);
                        break;
                    }
                    case 8:
                        if (*(u32 *)(p + 0xB48) != 0) {
                            ActorAttachment_ReleaseResource(*(void **)(p + 0xB48), 1,
                                                            *(void **)(p + 0xB18));
                            ActorAttachment_ReleaseResource(*(void **)(p + 0xB48), 0,
                                                            *(void **)(p + 0xB18));
                            *(u32 *)(p + 0xB48) = 0;
                        }
                        break;
                    case 1:
                        *(void **)(p + 0xB48) = ActorAttachment_CreateSpriteSlot(
                            3, p + 0x48, *(void **)(p + 0xB18), 0x2c0, 0x2c0, 0, 0, p + 0x3c);
                        break;
                    case 2:
                        *(void **)(p + 0xB48) = ActorAttachment_CreateSpriteSlot(
                            0, p + 0x48, *(void **)(p + 0xB18), 0x2c0, 0x2c0, 0, 0, p + 0x3c);
                        break;
                    case 0:
                        *(void **)(p + 0xB48) = ActorAttachment_CreateSpriteSlot(
                            1, p + 0x48, *(void **)(p + 0xB18), 0x2c0, 0x2c0, 0, 0, p + 0x3c);
                        break;
                    case 3:
                    case 5:
                    case 6:
                    case 7:
                        break;
                }
                Trigger_ConsumePending((p + 0xADC), 1);
                break;
            }
            case 12: {
                s32 active = m[4];
                if (active != 0)
                    SETF(p);
                else
                    *(u16 *)(p + 0xB32) &= 0xFBFF;
                Trigger_ConsumePending((p + 0xADC), 1);
                break;
            }
            case 13:
                ActorController_SetBehavior(p, 0x2b);
                if (m[4] != 0) {
                    {
                        u32 *w = (u32 *)(p + 0xB4);
                        w[2] &= ~1;
                    }
                    SETF(p);
                    *(p + 0xB3D) = 1;
                } else {
                    {
                        u32 *w = (u32 *)(p + 0xB4);
                        w[2] &= ~1;
                    }
                    SETF(p);
                }
                {
                    u16 v = m[5];
                    *(p + 0xB3E) = v;
                }
                break;
            case 15: {
                u16 v = m[4];
                *(u16 *)(p + 0x48) = v;
            }
                {
                    u16 v = m[5];
                    *(u16 *)(p + 0x4a) = v;
                }
                {
                    u16 v = m[6];
                    *(u16 *)(p + 0x4c) = v;
                }
                break;
        }
    } while (++i < q->count[gSceneEventDispatchState[0x24]]);
}
