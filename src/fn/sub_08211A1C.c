#include "global.h"
#define ForgingBook_SetAnimation sub_08211A1C
#define Anim_SetFrameDurationScale sub_08220F00

struct ForgingBook {
    u8 unknown[0xA68];
    u8 animationData[0x20];
    u8 animationHead[8];
    u32 animationFlags;
    u8 unknownA94[0x54];
    u8 animationBase;
    u8 animationFlag;
    s16 animationIndex;
};
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);
void sub_08220F00(void *, u32);

s32 sub_08211A1C(struct ForgingBook *book, s32 index, s32 scale, s32 mode)
{
    s32 same = 0;
    void *anim;
    void *data;
    s32 key;
    if (book->animationIndex == index)
        same = 1;
    book->animationIndex = index;
    anim = book->animationHead;
    data = book->animationData;
    key = index;
    key += book->animationBase;
    Anim_SetKeyframes(anim, data, (u16)key, (u16)mode, same);
    if (book->animationFlag)
        book->animationFlags |= 4;
    else
        book->animationFlags &= ~4;
    sub_08220F00(book->animationHead, (u16)scale);
    return Anim_Step(book->animationHead, book->animationData);
}
