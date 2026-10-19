#define Nidhoggr_UpdateSecondPairedBounds sub_0819494C
#include "global.h"

struct SceneAnimationPair { u32 x, y; };
struct SceneAnimationPart {
    u32 unk0;
    u16 unk4, flags;
    u8 unk8[12];
    struct SceneAnimationPair lower, upper;
};
struct SceneAnimationController {
    u8 unk0[0x5E50];
    struct SceneAnimationPart part5E50;
    u8 unk5E74[0x5EA4 - 0x5E74];
    struct SceneAnimationPart part5EA4;
    u8 unk5EC8[0x66D8 - 0x5EC8];
    struct SceneAnimationPart part66D8;
    u8 unk66FC[0x672C - 0x66FC];
    struct SceneAnimationPart part672C;
    u8 unk6750[0x6F60 - 0x6750];
    struct { u32 unk0, flags; } part6F60;
    u8 unk6F68[0x7248 - 0x6F68];
    u8 hidden, unk7249[2], alternate;
};
extern struct SceneAnimationPair gUnk_0824F56C, gUnk_0824F564;
extern struct SceneAnimationPair gUnk_0824F57C, gUnk_0824F574;
s32 sub_08190C98(void *, u32, u32);

void sub_0819494C(struct SceneAnimationController *owner)
{
    struct SceneAnimationController *p = owner;
    struct SceneAnimationPair *upper, *lower;
    struct SceneAnimationPart *part;
    if (p->alternate) {
        if (sub_08190C98(p, 1, 0x35)) {
            part = &p->part66D8;
            upper = &gUnk_0824F56C;
            part->upper = *upper;
            lower = &gUnk_0824F564;
            part->lower = *lower;
            part = &p->part5E50;
            part->upper = *upper;
            part->lower = *lower;
        }
    } else {
        if (sub_08190C98(p, 1, 0x14)) {
            part = &p->part672C;
            upper = &gUnk_0824F57C;
            part->upper = *upper;
            lower = &gUnk_0824F574;
            part->lower = *lower;
            part = &p->part5EA4;
            part->upper = *upper;
            part->lower = *lower;
        }
    }
flags:
    if (p->hidden) {
        part = &p->part672C;
        part->flags |= 4;
        part = &p->part5EA4;
        part->flags |= 4;
        { struct SceneAnimationWordFlags { u32 unk0, flags; }; struct SceneAnimationWordFlags *other = (void *)((u8 *)p + 0x6F70); u32 one = 1; other->flags |= one; }
    } else {
        s32 mask;
        part = &p->part672C;
        mask = ~4;
        part->flags &= mask;
        part = &p->part5EA4;
        part->flags &= mask;
    }
}
