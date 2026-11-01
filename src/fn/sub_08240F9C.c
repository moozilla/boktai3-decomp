// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "player_cue_sequence.h"
struct V8 {
    u32 a, b;
};
struct SprBig {
    u32 flags;
    u8 u4[0xC];
    u16 frame;
    u8 u12[0xA];
    struct V8 savedPos;
    u8 u24[0x8];
    void *palette;
};

struct PRecover {
    u8 pad000[0x18];
    u32 song;
    u8 pad00[0x20 - 0x1C];
    u32 flags20;
    u8 pad0[0x14C - 0x24];
    struct SprBig sprite;
    u8 pad1[0x3A2 - 0x14C - sizeof(struct SprBig)];
    u8 facing, flip, dir;
    u8 pad2[0x3FC - 0x3A5];
    struct DisplaySequenceState seq;
    u32 unknown40C;
    const void *resource;
    const void *resource2;
    u8 variant;
    u8 pad3[0x457 - 0x419];
    u8 tetherActive;
    u8 pad4;
    u16 timer;
    u8 pad5[0x464 - 0x45C];
    u8 f464, f465;
    u8 pad6[0xAD6 - 0x466];
    u16 f_ad6, f_ad8;
};
void LinkPlayer_FlushPendingEffects(struct PRecover *);
void SpiralTrail_CreateWithParticles(struct PRecover *, s32, s32, s32, s32);
void Player_SetSecondaryActionFlags(struct PRecover *, u32);
s32 ActorVariant_GetTableValue(u32, s32);
void Player_SetAnimationIndex(struct PRecover *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
s32 sub_0823C4B4(struct PRecover *);
s32 Player_GetAdjustedMovementInterval(struct PRecover *);
void Player_SetDirectionalVelocity_08238300(struct PRecover *, s32);
void sub_08238238(struct PRecover *);
void Player_PlayLocalSong(u32, s32);
void Player_SetTetherMode(struct PRecover *, s32, s32);
static inline void Step(struct SprBig *sp, struct DisplaySequenceState *st)
{
    PlayerCue_StepSequence(sp, st);
}

#define B(o) (*(u8 *)((u8 *)p + (o)))
#define H(o) (*(u16 *)((u8 *)p + (o)))
void Player_UpdateChargeSpin(struct PRecover *p)
{
    if (p->tetherActive != 0xE) {
        Player_SetTetherMode(p, 0xE, 0);
        p->f_ad8 = 0;
        LinkPlayer_FlushPendingEffects(p);
        Player_PlayLocalSong(p->song, 0x39A);
        SpiralTrail_CreateWithParticles(p, 1, 0, 0x20, 0x38);
    }
    if (p->timer == 0x10)
        Player_PlayLocalSong(p->song, 0x3A1);
    if ((p->timer & 7) >= 7 - (p->timer >> 2)) {
        Player_SetSecondaryActionFlags(p, 0x41);
        p->sprite.savedPos = *(struct V8 *)((u8 *)p + 0x30);
    }
    Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->variant, 0), 0x40);
    p->f_ad6 = 0x16F;
    p->f_ad8 += 3;
    if (p->f_ad8 > 0x20)
        p->f_ad8 = 0x20;
    Anim_SetSequence(&p->seq, p->resource, 0, p->facing, p->flip);
    Step(&p->sprite, &p->seq);
    {
        register u32 off asm("r2") = 0x178;
        void *pal;
        s32 bank;
        asm("" : "+r"(off));
        {
            register u8 *a asm("r0") = (u8 *)p + off;
            pal = *(void **)a;
        }
        bank = 0x16F;
        AnimData_SelectPaletteBank(pal, bank);
    }
    {
        register u32 ot asm("r0") = 0x45A;
        u16 *tp;
        register u32 t asm("r0");
        asm("" : "+r"(ot));
        tp = (u16 *)((u8 *)p + ot);
        t = *tp + 1;
        *tp = t;
        t = (u16)t;
        if (t > 0x1F) {
            u32 o1 = 0x464, o2;
            u8 *a1;
            register u32 v asm("r0");
            a1 = (u8 *)p + o1;
            v = 1;
            *a1 = v;
            o2 = 0x465;
            a1 = (u8 *)p + o2;
            v = 4;
            *a1 = v;
            Player_SetTetherMode(p, 0, 0);
        }
    }
}
