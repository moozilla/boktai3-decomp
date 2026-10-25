#include "global.h"

// Sword contact callback helpers for sub_080F4708.

// confidence=low: sword contact callback: applies the hit response, starts the palette restore and sound, and aims the child at the contact.
// Sword contact callback helpers for sub_080F4708.

// confidence=low: sword contact callback: applies the hit response, starts the palette restore and sound, and aims the child at the contact.
// Sword contact callback helpers for sub_080F4708.
struct Sub080F4708Contact {
    u8 pad0[0x6];
    u16 flags6;
    u8 pad8[0x4];
    u16 pos[3];
    u8 pad12[0x22 - 0x12];
    u16 h22;
    u8 pad24[0x34 - 0x24];
    u32 flags34;
    u8 pad38[0x3e - 0x38];
    u16 h3e;
    u8 pad40[0x42 - 0x40];
    u8 b42;
    u8 pad43[0x44 - 0x43];
    u16 h44;
};
struct Sub080F4708Child {
    u8 pad0[0xc];
    u8 *ptr0c;
    u8 pad10[0x52 - 0x10];
    u16 h52;
    u8 pad54[0x6a - 0x54];
    u16 h6a;
    u8 pad6c[0xa6 - 0x6c];
    u16 ha6;
    u8 padA8[0xe0 - 0xa8];
    u16 he0;
    u8 padE2[0x100 - 0xe2];
    s16 h100;
    u8 pad102[2];
    s16 h104;
    u8 pad106[0x10a - 0x106];
    s16 h10a;
    u8 pad10c[0x119 - 0x10c];
    u8 f119;
    u8 f11a;
    u8 pad11b[0x11e - 0x11b];
    u8 f11e;
    u8 f11f;
    u8 pad120[0x128 - 0x120];
    u16 h128;
    u8 pad12a[0x160 - 0x12a];
    u8 *shared;
    u8 pad164[0x4];
    void (*fn164)(void);
    void (*fn168)(void);
    u8 pad16c[0x174 - 0x16c];
    void (*fn174)(void);
    void (*fn178)(void);
    void (*fn17c)(void);
    u8 pad180[0x184 - 0x180];
    u32 u184;
};
extern const s16 gUnk_086149C4[];
void Collision_CalculateDamageResponse(void *, void *);
void PlayerStats_AddContactHundredths(void *, void *, s32);
void Sword_SetTrajectoryFromCurrentPosition(void *);
void Sword_UpdateComponentPaletteRestore(void);
void Sword_DecelerateReturn(void);
void Sword_UpdateThrustDelay(void);
void Sword_UpdateThrustExtension(void);
void Sword_UpdateLungeSpinup(void);
void Sword_UpdateSpinningLunge(void);
void Sword_DampTargetReturn(void);
void ActorPhase_DampMovement(void);
void TargetMotion_AdvanceFollowSegment(void);
void TargetMotion_UpdateFlashingExit(void);

#ifdef NONMATCHING
// fuzzy: 55.9%
// scope shape 77 (differences once low registers are set aside)
// confidence=low: sword contact callback: applies the hit response, starts the palette restore and sound, and aims the child at the contact.
void sub_080F4708(struct Sub080F4708Contact *a, struct Sub080F4708Contact *b,
                  struct Sub080F4708Child *c)
{
    s16 d[3];
    s16 *t;
    u8 *shared;
    u8 state;
    u8 angle;

    shared = c->shared;
    t = &c->h10a;
    if (*t <= 0)
        return;
    if (a->flags34 & 0x2000)
        return;
    if (c->he0)
        return;
    Collision_CalculateDamageResponse(a, b);
    b->h44 = a->h44;
    if (c->fn168 == 0 && b->h3e != 0) {
        c->h128 = 8;
        c->fn168 = Sword_UpdateComponentPaletteRestore;
        AnimData_SelectPaletteBank(c->ptr0c, *(u16 *)(shared + 0xd58));
        *t -= b->h3e;
        PlayerStats_AddContactHundredths(a, b, *(s16 *)(shared + 0xd1a));
        if (*t <= 0) {
            c->f11e = 1;
            c->f11f = 5;
            c->f119 |= 1;
            c->fn164 = TargetMotion_UpdateFlashingExit;
            Sound_PlaySong(0x13E);
            return;
        }
    }

    state = c->f11f;
    if ((state == 2 && c->fn174 == Sword_DecelerateReturn) ||
        (state == 3 &&
         (c->fn178 == Sword_UpdateThrustDelay || c->fn178 == Sword_UpdateThrustExtension)) ||
        (state == 4 &&
         (c->fn17c == Sword_UpdateLungeSpinup || c->fn17c == Sword_UpdateSpinningLunge ||
          c->fn17c == TargetMotion_AdvanceFollowSegment))) {
        Sword_SetTrajectoryFromCurrentPosition(c);
        state = c->f11f;
        if (state == 2) {
            c->u184 = 0x80;
            c->fn174 = Sword_DecelerateReturn;
        } else if (state == 3) {
            c->u184 = 0x14;
            c->fn178 = ActorPhase_DampMovement;
        } else if (state == 4) {
            c->u184 = 0x64;
            c->fn17c = Sword_DampTargetReturn;
        }

        if (a->flags6 & 0x100) {
            angle = a->b42;
        } else {
            d[0] = a->pos[0] - b->pos[0];
            d[1] = a->pos[1] - b->pos[1];
            d[2] = a->pos[2] - b->pos[2];
            if (d[0] == 0 && d[2] == 0) {
                u32 *counter = (u32 *)0x03005308;
                *counter = (*counter + 1) & 0x3FF;
                angle = ((u16 *)RANDOM_TABLE_ADDR)[*counter];
            } else {
                angle = Math_Atan2(d[0], d[2]);
            }
        }
        c->f11a = angle;
        c->ha6 = 1;
        c->h52 = 1;
        c->h6a = a->h22;
        c->h100 = gUnk_086149C4[(u8)(angle + 0x40)] >> 7;
        c->h104 = gUnk_086149C4[angle] >> 7;
        Sound_PlaySong(0x170);
    } else {
        if (b->h3e != 0)
            Sound_PlaySong(0x13E);
        else
            Sound_PlaySong(0x190);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080F4708);
#endif
