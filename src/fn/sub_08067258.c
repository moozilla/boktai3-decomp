// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control.h"
#define SceneActorControl_TickActive sub_08067258
struct SceneActorControlPlayerView {
    u8 unknown0[0x20];
    u32 flags;
    u8 unknown24[0x4F4 - 0x24];
    u8 parameter;
};
struct SceneActorControlGameView {
    u8 unknown0[0x876];
    s16 parameter;
};
extern struct SceneActorControlPlayerView *gUnk_02000580[4];
void sub_08214374(void *, u32);
void sub_082143A0(void *, u32);
void sub_08067018(struct SceneActorControlManager *, struct SceneActorControlSlot *);
void sub_08158EC8(struct ContactPosition *);
u32 sub_08067190(struct SceneActorControlSlot *);
void sub_080674F0(struct SceneActorControlSlot *);
static inline u8 PlayerHasControlFlag(void)
{
    u32 mask = 0x80000;
    if (gUnk_02000580[0]->flags & mask)
        return 1;
    return 0;
}
static inline void ClearFlags(u16 *address, s32 clear)
{
    register u16 *shared asm("r5") = address;
    u32 flags = *shared;
    s32 mask = ~clear;
    mask &= flags;
    *shared = mask;
}
void SceneActorControl_TickActive(struct SceneActorControlManager *p,
                                  struct SceneActorControlSlot *input)
{
    struct SceneActorControlSlot *slot = input;
    struct SceneActorControlSprite *sprite = (void *)slot;
    s32 delta;
    register u16 *flags asm("r5");
    register u32 initialFlags asm("r1");
    s32 clearMask;
    u32 clearValue;
    slot->position = slot->savedPosition;
    flags = &slot->flags;
    initialFlags = *flags;
    delta = (s16)(initialFlags & 0x10);
    if (delta) {
        s32 parameter = slot->parameter;
        s32 current = gUnk_02000580[0]->parameter;

        if (current < parameter) {
            s32 wrapped = parameter - 256;
            delta = current - wrapped;
        } else
            delta = current - parameter;
        {
            u16 *address;
            u16 *previous;
            u32 stored;
            address = (void *)slot;
            address = (u16 *)((u8 *)address + 0x4E);
            stored = *address;
            flags = &slot->flags;
            previous = address;
            if (stored != delta && (delta & 4)) {
                u32 value = *flags;
                register u32 mask asm("r0") = 4;
                mask |= value;
                *flags = mask;
            }
            if (!sprite->active)
                sub_08214374(sprite, 0);
            if (*flags & 4) {
                slot->timer = 60;
                sub_08067018(p, slot);
                sub_08158EC8(&slot->position);
                {
                    u16 *target = previous;
                    *target = delta;
                }
                ClearFlags(flags, 4);
            }
        }
        if (delta > 9 || (*flags & 2)) {
            slot->stateFrame = 0;
            slot->kind = 2;
            sprite->flags &= ~1;
            clearValue = *flags;
            clearMask = ~0x12;
            goto applyMask;
        }
    } else if (initialFlags & 0x40) {
        if (!slot->active)
            sub_08214374(slot, 0);
        if (*flags & 4) {
            sub_08067018(p, slot);
            ClearFlags(flags, 4);
        }
        if (*flags & 2) {
            slot->stateFrame = delta;
            slot->kind = 2;
            clearValue = *flags;
            clearMask = ~2;
            goto applyMask;
        }
    } else {
        u32 visible = PlayerHasControlFlag();
        flags = &slot->flags;
        if (visible ||
            ((*flags & 0x20) &&
             ((struct SceneActorControlGameView *)gUnk_02000710)->parameter > 0) ||
            (u8)sub_08067190(slot)) {
            if (!sprite->active)
                sub_08214374(sprite, 0);
        } else {
            if (sprite->active)
                sub_082143A0(sprite, 0);
        }
        if (*flags & 2) {
            if (!sprite->active)
                sub_08214374(sprite, 0);
            slot->stateFrame = 0;
            slot->kind = 2;
            clearValue = *flags;
            clearMask = 0;
            asm("" : "+r"(clearMask));
            clearMask -= 3;
            goto applyMask;
        }
    }
    goto normal;
applyMask:
    clearMask &= clearValue;
    *flags = clearMask;
    return;
normal:
    if (!(slot->flags & 0x50))
        sub_080674F0(slot);
}
