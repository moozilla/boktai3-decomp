#include "global.h"
#ifdef NONMATCHING
// fuzzy: 81.1%
// Scene-actor cutscene dispatcher (Vanargand, Ratatosk). Commands mostly set
// state 2 with a sub-state (setState); 2 places the actor, 9 configures an
// effect, 15/16 pick the body graphics variant, 25 flashes the background.
// Still differs: sb/r8 swapped for entry/i; in cases 15/16 the graphics
// variant branches use other registers, so their tails are not cross-jumped
// as in the original.

typedef struct {
    u16 id;
    u8 unk[4];
    u8 count[2];
    s16 *messages[2][4];
    u8 pad[8];
    void *next;
} Entry;

struct S {
    u8 pad0[0x54];
    u16 h54, h56, h58;
    u8 pad5a[0xAC - 0x5A];
    u8 state;
    u8 entered;
    u8 sub;
    u8 padAF;
    u8 changed;
    u8 padB1[0xC4 - 0xB1];
    u32 timer;
    u8 padC8[0xD0 - 0xC8];
    u16 hD0, hD2, hD4;
    u8 padD6[0x114 - 0xD6];
    u8 angle;
    u8 pad115[0x118 - 0x115];
    Entry entry;
};

struct T {
    u8 pad0[0x260];
    u16 h260, h262, h264;
    u8 pad266[0x288 - 0x266];
    u8 *palette;
    u8 pad28c[0x344 - 0x28C];
    u16 h344;
    u8 pad346[0x38C - 0x346];
    u8 b38C;
    u8 pad38d;
    u8 b38E;
    u8 b38F;
    u32 w390;
    u8 pad394[0x3B8 - 0x394];
    u8 b3B8;
    u8 pad3b9;
    u8 b3BA;
    u8 pad3bb[0x3C4 - 0x3BB];
    u8 pal3C4[0x5C4 - 0x3C4];
    u8 pal5C4[0x7C4 - 0x5C4];
    u8 pal7C4[0x9C4 - 0x7C4];
    u16 h9C4, h9C6, h9C8;
    u8 pad9ca[0x9DC - 0x9CA];
    void *flash;
};

extern u8 *gUnk_0200047C;
extern u8 *gUnk_030042E4;

void sub_08020D54(void *, void *);
void Trigger_ConsumePending(void *, s32);
void sub_08022AF4(void *);
void BackgroundFlash_BeginAlternating(void *);
void BackgroundFlash_BeginAppearance(void *);
s32 SceneActorGraphics_GetResourceId(void *);
void CpuSet(const void *, void *, u32);
void SceneActors_ConfigureEffect(void *, s32, s32, s32);

static inline void setStateFull(struct S *s, u32 state, u32 entered, u32 sub)
{
    s->state = state;
    s->entered = entered;
    s->sub = sub;
    s->timer = entered;
}
#define setState(s, sub) setStateFull(s, 2, 0, sub)

static inline void effect2(u8 *p, int mode, int id, int duration)
{
    *(u32 *)(p + 0x390) = id;
    p[0x38e] = mode;
    p[0x38f] = duration;
    if (p[0x38e] == 2) {
        u32 value = duration == 5 ? 32 : 64;
        p[0x38c] = value;
    } else if (p[0x38e] == 1)
        p[0x38c] = 0;
}
#define effect(p, id, mode, d) effect2(p, mode, id, d)

void sub_08022F88(struct S *s)
{
    struct T *t = (struct T *)s;
    Entry *entry = &s->entry;
    s32 i;

    i = 0;
    if (i < entry->count[gUnk_0200047C[0x24]])
        do {
            s16 *m;
            u8 *controller = gUnk_0200047C;
            m = entry->messages[controller[0x24]][i];
            sub_08020D54(entry, m);
            switch (((u8 *)m)[6]) {
                case 0:
                    setState(s, 0);
                    s->changed = 1;
                    break;
                case 1:
                    s->angle = (m[4] + 5) << 5;
                    setState(s, 1);
                    s->changed = 1;
                    break;
                case 3:
                    s->angle = (m[4] + 5) << 5;
                    setState(s, 2);
                    s->changed = 1;
                    break;
                case 4:
                    s->angle = (m[4] + 5) << 5;
                    setState(s, 3);
                    s->changed = 1;
                    break;
                case 5:
                    s->angle = (m[4] + 5) << 5;
                    setState(s, 4);
                    s->changed = 1;
                    break;
                case 6:
                    s->angle = (m[4] + 5) << 5;
                    setState(s, 5);
                    s->changed = 1;
                    break;
                case 7:
                    setState(s, 6);
                    s->changed = 1;
                    break;
                case 8:
                    setState(s, 7);
                    s->changed = 1;
                    break;
                case 10:
                    setState(s, 8);
                    s->changed = 1;
                    break;
                case 11:
                    setState(s, 9);
                    s->changed = 1;
                    break;
                case 12:
                    t->h9C4 = m[4];
                    t->h9C6 = m[5];
                    t->h9C8 = m[6];
                    setState(s, 10);
                    s->changed = 1;
                    break;
                case 19:
                    setState(s, 11);
                    s->changed = 1;
                    break;
                case 20:
                    setState(s, 12);
                    s->changed = 1;
                    break;
                case 21:
                    setState(s, 13);
                    s->changed = 1;
                    break;
                case 23:
                    setState(s, 14);
                    s->changed = 1;
                    break;
                case 24:
                    setState(s, 15);
                    s->changed = 1;
                    break;
                case 25:
                    if (m[4])
                        sub_08022AF4(t);
                    else if (m[5])
                        BackgroundFlash_BeginAlternating(t->flash);
                    else
                        BackgroundFlash_BeginAppearance(t->flash);
                    break;
                case 13:
                    setState(s, 16);
                    s->changed = 1;
                    break;
                case 14:
                    setState(s, 18);
                    s->changed = 1;
                    break;
                case 22:
                    s->hD0 = m[4];
                    s->hD2 = m[5];
                    s->hD4 = m[6];
                    setState(s, 19);
                    s->changed = 1;
                    break;
                case 18:
                    setState(s, 20);
                    s->changed = 1;
                    break;
                case 15:
                    if (m[4] == 0) {
                        s32 r;
                        t->b38C = 0;
                        t->b38E = 0;
                        t->w390 = SceneActorGraphics_GetResourceId(t);
                        r = SceneActorGraphics_GetResourceId(t);
                        switch (r) {
                            case 0:
                                t->palette = t->pal5C4;
                                break;
                            case 1:
                                t->palette = t->pal7C4;
                                break;
                            default:
                                CpuSet(gUnk_030042E4 + r * 32, t->pal3C4, 0x04000008);
                                t->palette = t->pal3C4;
                                break;
                        }
                    } else if (m[5] == 0) {
                        effect((u8 *)t, 0x338, 3, 5);
                    } else {
                        effect((u8 *)t, 0x33A, 3, 5);
                    }
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 16:
                    if (m[4] == 0)
                        effect((u8 *)t, 0x335, 1, 5);
                    else if (m[5] == 0)
                        effect((u8 *)t, 0x338, 2, 5);
                    else
                        effect((u8 *)t, 0x33A, 2, 5);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 9:
                    SceneActors_ConfigureEffect(s, m[4], 0xB4, 0xB4);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 17:
                    t->b3B8 = m[4];
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 2:
                    s->h54 = m[4];
                    s->h56 = m[5];
                    s->h58 = m[6];
                    t->h260 = s->h54;
                    t->h262 = t->h344 + s->h56;
                    t->h264 = s->h58;
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 66:
                    t->b3BA = m[4];
                    setState(s, 21);
                    s->changed = 1;
                    break;
            }
            i++;
        } while (i < entry->count[gUnk_0200047C[0x24]]);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08022F88);
#endif
