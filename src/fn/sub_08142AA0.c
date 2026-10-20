#include "global.h"

// confidence=low: continuation of the neighboring object-push action.
#define Player_UpdateObjectPushRecovery sub_08142AA0
#define F8(n) (*(u8 *)(p + (n)))
#define F16(n) (*(u16 *)(p + (n)))
extern const s16 gUnk_086149C4[];
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void ActorSequence_Begin(void *, s32, s32);
s32 SceneObjects_TryPushNeighborByIndex(void *, u32);
static inline s32 Scale(s32 v)
{
    if (v >= 0) return v >> 12;
    return -((-v) >> 12);
}
static inline u8 IsTwo(u8 *p)
{
    u16 active = F16(0x538);
    u8 result = 0;
    if (active && F16(0x534) == 2) result = 1;
    return result;
}
void Player_UpdateObjectPushRecovery(u8 *p)
{
    switch (F8(0x458)) {
    case 0: {
        s32 angle = ((F8(0x4a5) + 5) & 7) << 5;
        F16(0x38) = Scale(gUnk_086149C4[(angle + 0x40) & 255] * 4);
        F16(0x3c) = Scale(gUnk_086149C4[angle] * 4);
        Player_SetAnimation(p, ActorVariant_GetTableValue(F8(0x418), 6), 0x40, 1);
        if (++F16(0x45a) > 63) ActorSequence_Begin(p, 5, 1);
        break;
    }
    case 1:
        Player_SetAnimation(p, ActorVariant_GetTableValue(F8(0x418), 6), 0x40, 1);
        if (*(s16 *)(p + 0x34c) == F8(0x4a5) && SceneObjects_TryPushNeighborByIndex(p + 0x30, F8(0x3a4)) > 0) {
            if (p && IsTwo(p)) ActorSequence_Begin(p, 0, 0);
            else ActorSequence_Begin(p, 5, 0);
        } else ActorSequence_Begin(p, 5, 2);
        break;
    case 2:
        if (Player_SetAnimation(p, ActorVariant_GetTableValue(F8(0x418), 7), 0x40, 1)) {
            ActorSequence_Begin(p, 4, 1);
            F8(0x4a4) = 0;
        }
        break;
    }
}
