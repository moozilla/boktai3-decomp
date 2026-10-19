#define Hresvelgr_SetAnimation        sub_0819B96C
#define MotionEntry_CreateWithDelta   sub_081A48E0
#define ActorReaction_EmitMotionBurst sub_081A0FF0
#define gRandomValues                 gUnk_0203B400
#define gRandomIndex                  gUnk_03005308
#include "global.h"
struct ActorReaction {
    u8 unknown0[0x9c];
    u32 flags;
    u8 unknownA0[3];
    u8 phase;
    u8 unknownA4[0xb];
    u8 finished;
    u8 unknownB0;
    u8 pending;
    u8 unknownB2[0xc2];
    u8 collider174[0x20];
    u8 collider194[0x20];
    u32 motion;
    u8 unknown1B8[0x158];
    u16 state;
};
struct MotionPosition {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 pad : 16;
};
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void Sound_PlaySong(u32);
void Hresvelgr_SetAnimation(struct ActorReaction *, void *, void *, u32, u32, u32);
void *MotionEntry_CreateWithDelta(u32, struct MotionPosition *, u32, u32);
void sub_0819C8F8(struct ActorReaction *, u32, u32);
void sub_081A0FF0(struct ActorReaction *actor, s32 step)
{
    struct ActorReaction *saved = actor;
    if (actor->pending) {
        s32 i;
        u32 motion;
        actor->pending = 0;
        Hresvelgr_SetAnimation(actor, (u8 *)actor + 0x194, (u8 *)actor + 0x174, 15, 1, 4);
        actor->state = 8;
        motion = (u32)&actor->motion;
        for (i = 3; i >= 0; i--) {
            struct MotionPosition delta = {0};

            u32 *current;
            u32 index;
            s32 value;

            current = &gUnk_03005308;
            index = (*current + 1) & 0x3ff;
            delta.x = ((*(u16 *)(RANDOM_TABLE_ADDR + (index << 1))) & 15) - 8;
            index = (index + 1) & 0x3ff;
            delta.y = (*(u16 *)(RANDOM_TABLE_ADDR + (index << 1))) & 7;
            index = (index + 1) & 0x3ff;
            delta.z = ((*(u16 *)(RANDOM_TABLE_ADDR + (index << 1))) & 15) - 8;
            index = (index + 1) & 0x3ff;
            *current = index;
            value = (*(u16 *)(RANDOM_TABLE_ADDR + (index << 1)));
            MotionEntry_CreateWithDelta(motion, &delta, value - ((value >> 2) << 2), 60);
        }
        Sound_PlaySong(0x4d1);
    }
    {
        u32 flags = 12;
        actor->flags |= flags;
    }
    sub_0819C8F8(saved, 24, step << 2);
    if (actor->phase > 3)
        actor->finished = 1;
}
