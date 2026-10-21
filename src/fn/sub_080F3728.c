#include "global.h"
/* Tentative family name: owner of the active child-record group. */
struct ChildGroupPosition {
    u32 x, y;
};
struct ChildGroup {
    u8 pad0[0x7E];
    s16 health;
    u8 pad80[0xD];
    u8 flags;
    u8 pad8E[4];
    u8 state92, state93;
    u8 pad94[8];
    u16 flashTimer;
    u8 pad9E[0xAD6];
    void *animation;
    u8 padB78[0xC];
    struct ChildGroupPosition position;
    u8 padB8C[0x13C];
    void (*callback)(struct ChildGroup *);
    u8 padCCC[0x87];
    u8 fieldD53;
    u8 padD54[4];
    u16 palette;
    u8 padD5A[6];
    u32 fieldD60, fieldD64, fieldD68;
    u8 padD6C[0xC];
    u16 damage, fieldD7A;
    u8 padD7C[0x1C];
    struct ChildGroupPosition savedPosition;
    u8 padDA0[0x10];
};
struct ChildGroupEvent {
    u8 pad0[0x40];
    u16 timer;
    u8 pad42[0xE];
    struct ChildGroup *owner;
};
void sub_080F38E0(struct ChildGroup *);
void ChildGroup_UpdateLandingState(struct ChildGroup *);
void AnimData_SelectPaletteBank(void *, u32);
static inline void SetState(struct ChildGroup *p, u8 one, u8 id)
{
    p->state92 = one;
    p->state93 = id;
    p->flags |= one;
}
s32 ChildGroup_ProcessEvent(struct ChildGroupEvent *event, s32 kind, s32 damage)
{
    struct ChildGroup *p = event->owner;
    if (!p || p->health <= 0)
        return 0;
    switch (kind) {
        case 0:
            event->timer = 0;
            *(u32 *)((u8 *)p + 0xDA0) = 10;
            if (damage) {
                p->damage = damage;
                p->fieldD7A = 1;
                p->fieldD68 = 1;
                p->fieldD64 = 0;
                p->fieldD60 = 0;
                p->flashTimer = 8;
                AnimData_SelectPaletteBank(p->animation, p->palette);
            } else {
                p->damage = 0;
                p->fieldD7A = 0;
                p->fieldD68 = 0;
                p->fieldD64 = 0;
                p->fieldD60 = 0;
                p->callback = sub_080F38E0;
                SetState(p, 1, 7);
            }
            break;
        case 1:
            p->fieldD53 = 0;
            p->callback = ChildGroup_UpdateLandingState;
            p->position = p->savedPosition;
            SetState(p, 1, 4);
            break;
    }
    return 1;
}
