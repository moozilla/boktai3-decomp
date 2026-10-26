#include "global.h"
#include "engine.h"
#include "ram.h"

/* tentative: steps a keyframe state machine for a category-14 scene actor */

struct Obj {
    u8 unk0[8];
    u32 flags;
    u8 unkC[0x11];
    u8 flags1D;
    u8 unk1E[0x42];
    u8 body[0xD0];
    s16 state;
    s16 base;
    s16 step;
    s16 unk136;
    s16 counter;
    s16 unk13A;
    s16 unk13C;
    s16 bit;
    u16 flags140;
};
extern const u8 gUnk_086106F0[];
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);
void Anim_Remove(void *);
void Sound_PlaySong(u32);
void TerrainOverlay_QueueRemoveActor(void *);
void SceneBody_Unregister(void *);

s32 SceneActor14_UpdateAnimState(struct Obj *p)
{
    u8 *data = gAnimatedBodyPool;
    s32 idx;

    if (p->flags1D & 1) {
        if (p->state == -1 && p->unk136 > p->unk13C) {
            Anim_SetKeyframes(p, data + 0x18, gUnk_086106F0[0x12], 2, 0);
            return 0;
        }
        if (p->state == 0 && !(gEngineFlags & 0x800))
            Sound_PlaySong(0x1A8);
        if (p->state == 1 && p->counter > 0) {
            data += 0x18;
            Anim_SetKeyframes(p, data, gUnk_086106F0[p->base + p->state], 2, 0);
            Anim_Step(p, data);
            return 0;
        }
        p->state += p->step;
        idx = p->base + p->state;
        if (p->state == 3) {
            s32 b = p->bit;
            *(u32 *)(data + 0x54C) &= ~(1 << b);
            p->flags |= 1;
            Anim_Remove(p);
            if (p->flags140 & 1)
                TerrainOverlay_QueueRemoveActor(p->body);
            SceneBody_Unregister(p->body);
            p->bit = -1;
            p->unk13A = 2;
            return 1;
        }
        data += 0x18;
        Anim_SetKeyframes(p, data, gUnk_086106F0[idx], 2, 0);
        Anim_Step(p, data);
    } else {
        Anim_Step(p, data + 0x18);
    }
    if (p->state == 1)
        p->counter--;
    return 0;
}
