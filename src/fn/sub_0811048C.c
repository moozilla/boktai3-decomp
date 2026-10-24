#include "global.h"
#include "game_state.h"
struct LinkScorePanel {
    u8 pad0[0xfe4];
    void (*update)(struct LinkScorePanel *);
    u8 padFE8[0xff8 - 0xfe8];
    u32 flags;
    u8 padFFC[0x1000 - 0xffc];
    u8 state;
    u8 pad1001[0x1024 - 0x1001];
    u8 redraw;
    u8 pad1025;
    u16 timer;
    u8 pad1028[0x1030 - 0x1028];
    u16 remaining;
    u16 step;
};
extern u16 gUnk_03005260[];
void CharacterModel_UpdatePairedAnimations(void *, s32);
void PaletteFade_CycleMenuColors(void *);
s32 Mod(s32, s32);
void Sound_PlaySong(u32);
void LinkScorePanel_SetNumber(struct LinkScorePanel *, s32, s32);
void LinkScorePanel_OnWaitSessionReady(struct LinkScorePanel *);

#define SCORE (*(s16 *)((u8 *)gUnk_02000710 + 0x7b2))

static inline void SetUpdate(struct LinkScorePanel *p, void (*fp)(struct LinkScorePanel *), u8 st)
{
    p->update = fp;
    p->redraw = 1;
    p->state = st;
}

static inline void SetFlag(u32 *f, u32 m)
{
    *f |= m;
}

static inline u8 FlagIsSet(u32 bits)
{
    if (bits)
        return TRUE;
    return FALSE;
}

void sub_0811048C(struct LinkScorePanel *p)
{
    if (p->redraw) {
        p->step = 2;
        p->timer = 0;
        p->redraw = 0;
    }
    CharacterModel_UpdatePairedAnimations((u8 *)p + 0xd8c, 0);
    PaletteFade_CycleMenuColors((u8 *)p + 0x770);
    if (p->timer > 0x3c) {
        if (p->remaining) {
            s32 d = p->remaining - p->step;
            if (d < 0) {
                d += p->step;
                SCORE += d;
                p->remaining -= d;
            } else {
                SCORE += p->step;
                p->remaining -= p->step;
            }
            if (SCORE < 0)
                SCORE = 0;
            if (SCORE > 9999)
                SCORE = 9999;
            if (Mod(p->timer, 2) == 0)
                Sound_PlaySong(0x2a5);
        } else if (gUnk_03005260[1] & 0x30f) {
            Sound_PlaySong(0xdd);
            SetUpdate(p, LinkScorePanel_OnWaitSessionReady, 3);
            return;
        }
        {
            u32 value = 1;
            u32 *flagWord = &p->flags;
            u32 bits = *flagWord & value;
            value = (u32)flagWord;
            if (!FlagIsSet(bits) && (gUnk_03005260[1] & 0x30f)) {
                p->step <<= 3;
                SetFlag((u32 *)value, 1);
            }
        }
    }
    LinkScorePanel_SetNumber(p, 0, p->remaining);
    LinkScorePanel_SetNumber(p, 1, SCORE);
    p->timer++;
}
