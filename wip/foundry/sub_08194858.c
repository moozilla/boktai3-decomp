#define Nidhoggr_UpdatePairedBounds sub_08194858
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
extern struct SceneAnimationPair gUnk_0824F54C, gUnk_0824F544;
extern struct SceneAnimationPair gUnk_0824F55C, gUnk_0824F554;
s32 sub_08190C98(void *, u32, u32);

void sub_08194858(u8 *owner)
{
    u8 *p = owner;
    struct SceneAnimationPair *upper, *lower;
    struct SceneAnimationPart *part;
    if (p[0x724B]) {
        if (sub_08190C98(p, 0, 0x37)) {
            part = (void *)(p + 0x672C);
            upper = &gUnk_0824F54C;
            part->upper = *upper;
            lower = &gUnk_0824F544;
            part->lower = *lower;
            part = (void *)(p + 0x5EA4);
            part->upper = *upper;
            part->lower = *lower;
        }
    } else {
        if (sub_08190C98(p, 0, 0x12)) {
            part = (void *)(p + 0x66D8);
            upper = &gUnk_0824F55C;
            part->upper = *upper;
            lower = &gUnk_0824F554;
            part->lower = *lower;
            part = (void *)(p + 0x5E50);
            part->upper = *upper;
            part->lower = *lower;
        }
    }
flags:
    if (p[0x7248]) {
        part = (struct SceneAnimationPart *)(p + 0x66D8);
        part->flags |= 4;
        part = (struct SceneAnimationPart *)(p + 0x5E50);
        part->flags |= 4;
        { struct SceneAnimationWordFlags { u32 unk0, flags; }; struct SceneAnimationWordFlags *other = (void *)(p + 0x6F60); u32 one = 1; other->flags |= one; }
    } else {
        s32 mask;
        part = (struct SceneAnimationPart *)(p + 0x66D8);
        mask = ~4;
        part->flags &= mask;
        part = (struct SceneAnimationPart *)(p + 0x5E50);
        part->flags &= mask;
    }
}
