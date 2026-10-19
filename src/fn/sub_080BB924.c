#include "global.h"
#ifdef NONMATCHING
// fuzzy: 96.9% (agent draft, in progress; see notes/cloud-08122DF4.md)
typedef struct Ctx {
    u8 pad0[0x62A];
    u16 unk62A;
    u8 pad62C[0x52];
    u16 unk67E;
} Ctx;
typedef struct Obj {
    u8 pad0[0xC];
    u8 unkC[0x4D];
    u8 unk59;
    u8 pad5A[0x42];
    u16 unk9C;
    u8 pad9E[0x12];
    s16 unkB0;
    u8 padB2[0x2A];
    u32 unkDC;
    u8 padE0[0x24];
    u32 unk104;
    u8 pad108[0xC];
    u32 unk114;
    u32 unk118;
    u32 unk11C;
    u8 unk120;
    u8 unk121;
    u8 unk122;
    u8 unk123;
    u32 unk124;
    u8 unk128;
    u8 pad129[0x3];
    u16 unk12C;
    u16 unk12E;
    u8 pad130[0xA];
    u16 unk13A;
    u8 pad13C[0x52];
    u16 unk18E;
    u8 pad190[0x8C];
    void *unk21C;
    u8 pad220[0x88];
    u8 unk2A8;
    u8 unk2A9;
    u8 unk2AA;
    u8 pad2AB[0x1];
    u32 unk2AC;
    u8 unk2B0;
    u8 unk2B1;
    u8 unk2B2;
    u8 unk2B3;
    u8 pad2B4[0x1];
    u8 unk2B5;
    u8 unk2B6;
    u8 pad2B7[0x9];
    u16 unk2C0;
    u16 unk2C2;
    u8 pad2C4[0x4];
    u16 unk2C8;
    u8 pad2CA[0x2];
    u32 unk2CC;
    u32 unk2D0;
    u16 unk2D4;
    u16 unk2D6;
    u32 unk2D8;
    u32 unk2DC;
    u32 unk2E0;
    u8 pad2E4[0x2];
    u16 unk2E6;
    u8 pad2E8[0x2];
    u16 unk2EA;
    u8 pad2EC[0x18];
    u32 unk304;
    u8 pad308[0x18];
    u8 unk320[0xac];
    u8 unk3CC;
    u8 pad3CD[0x3];
    Ctx *unk3D0;
} Obj;
void ActorShadow_Enable(void *);
void SceneActorAnimation_SelectDirectionalEntry(void *, u32);
void Sound_PlaySong(u32);
void SceneActorPalette_ClearCompletedTransitions(void);
void sub_080B5F38(Obj *);
void SceneActorControl_OnIdle(void);
void Trigger_ConsumePending(void *, s32);
static inline u8 take_flag(Obj *s)
{
    if (s->unk2B2) {
        s->unk2B2 = 0;
        return 1;
    }
    return 0;
}

static inline u8 peek_flag(Obj *s)
{
    return s->unk2B3;
}

void sub_080BB924(Obj *s)
{
    Ctx *ctx = s->unk3D0;
    s32 n;
    u8 saved;
    u32 keep;
    if (take_flag(s)) {
        ActorShadow_Enable(s->unk320);
        SceneActorAnimation_SelectDirectionalEntry(s, 13);
        Sound_PlaySong(0x3AE);
    }
    if (!peek_flag(s))
        return;
    s->unk13A &= ~4;
    s->unk18E &= ~4;
    ctx->unk62A &= ~4;
    ctx->unk67E &= ~4;
    {
        u16 *f = &s->unk2D4;
        u32 mask = ~0x800;
        *f &= mask;
    }
    saved = s->unk59;
    keep = (s->unk2D4 & 0x4000) ? 0x4000 : 0;
    s->unk2CC = s->unk118;
    s->unk2D0 = s->unk11C;
    s->unk59 = s->unk120;
    s->unk2A8 = s->unk121;
    s->unk2A9 = s->unk122;
    s->unk2AA = s->unk123;
    s->unk21C = (void *)s->unk124;
    s->unk2B5 = s->unk128;
    s->unk2C0 = s->unk12E;
    s->unk2EA = s->unk12C;
    n = s->unkB0;
    if (n <= 0)
        n = 0;
    s->unk9C = n;
    s->unk2D6 = 0;
    s->unk2D4 = 0;
    s->unk2B0 = 0;
    s->unk2B1 = 1;
    s->unk2B2 = 1;
    s->unk2B3 = 0;
    s->unk304 = 0;
    s->unk2B6 = 0;
    s->unk2C2 = 64;
    s->unk2D8 = 0;
    s->unk2DC = 0;
    s->unk2E0 = 0;
    s->unk2E6 = 0;
    s->unk3CC = 0;
    s->unkDC = 0;
    SceneActorPalette_ClearCompletedTransitions();
    s->unk2D4 |= keep;
    s->unk59 = saved;
    sub_080B5F38(s);
    {
        u32 mask = 0x4000;
        if (s->unk2D4 & mask) {
            s->unk2A8 = 2;
            s->unk2A9 = 0;
            s->unk2AA = 2;
            s->unk2AC = 0;
            s->unk2B1 = 1;
            {
                long long mask1 = ~1;
                void *control = SceneActorControl_OnIdle;
                u32 kind = 16;
                s->unk2B2 = 1;
                s->unk2B0 = 0;
                s->unk2B5 = kind;
                s->unk21C = control;
                s->unk2C8 = 0;
                {
                    u32 *f = &s->unk114;
                    s32 m = mask1;
                    *f &= m;
                }
                s->unk104 = 0;
            }
            Trigger_ConsumePending(s->unkC, 1);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080BB924);
#endif
