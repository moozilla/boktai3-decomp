// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "game_state.h"

struct GameStartSeqs {
    u32 seq[3];
};
struct GameStartAnim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unk0A, scale, timer;
};
struct GameStartSprite {
    u32 flags;
    u8 unk04[0xC];
    u16 frame;
};

extern const struct GameStartSeqs gUnk_0824FD50;
extern const u8 gUnk_0824FD5C[];
extern const u8 gUnk_0824FDBC[];
extern u32 gUnk_020003A0[];

void *memcpy(void *, const void *, u32);
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
u8 *PlayerName_GetBuffer(void);
void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawText(u32, u32, const u8 *);
void Anim_SetSequence(void *, void *, u16, u32, u32);
void Bg_SetTilemapEntry(s32, s32, s32, s32, s32, s32);
int Script_ParseStringRef(const void *);
int SceneSequence_GetSelectedMessage(void);
const u8 *Text_LookupString(int);
s32 Div(s32, s32);
s32 Mod(s32, s32);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);

#define W(o)    (*(s32 *)(p + (o)))
#define GS_H(o) (*(s16 *)((u8 *)gGameState + (o)))
#define GS_W(o) (*(s32 *)((u8 *)gGameState + (o)))

static inline void StepAnim(struct GameStartAnim *a, struct GameStartSprite *s)
{
    const volatile u16 *entry = a->data + a->frame;

    s->frame = *entry >> 6;
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 1;
        if ((mask & input) != (((*entry & 0x30) >> 4) & mask))
            s->flags |= 4;
        else
            s->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        if ((u8)(mask & input) != (((*entry & 0x30) >> 4) & mask))
            s->flags |= 8;
        else
            s->flags &= ~8;
    }
    if ((u16)++a->timer >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8)
                    a->timer = a->rate;
                else
                    a->frame = a->count - 1;
            } else {
                a->frame--;
            }
        } else if ((u16)++a->frame >= a->count) {
            if ((u8)(a->flags & 8)) {
                a->timer = a->rate;
                a->frame = a->count - 1;
            } else {
                a->frame = 0;
            }
        }
        entry = a->data + a->frame;
        a->factor = *entry & 15;
        a->rate = (a->factor * a->scale) >> 6;
        if (a->rate == 0)
            a->rate = 1;
    }
}

// Tentative: draws the Game Start save summary (name, region, play time, level, labels).
void GameStart_DrawSaveInfo(u8 *p)
{
    u8 text[0x20];
    struct GameStartSeqs seqs;
    u8 names[6][16];
    s32 i;
    const u8 *s;
    u8 *levelRes;

    seqs = gUnk_0824FD50;
    memcpy(names, gUnk_0824FD5C, sizeof(names));
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    if (GS_H(0x86e) != 0) {
        u8 misc[10];
        u32 zero;
        u32 *zp;
        s32 idx;
        s32 hours, minutes, level;
        u8 *q;
        u8 *tens, *res;
        u8 *minuteOnes, *minuteTens, *colon, *levelOnes, *hourOnes;

        memcpy(misc, gUnk_0824FDBC, sizeof(misc));
        s = PlayerName_GetBuffer();
        Menu_EraseRect(8, 7, 12, 2);
        Menu_DrawText(8, 7, s);
        {
            s32 region = GS_H(0x5a8);
            asm("" : : "r"(region));
        }
        idx = 0;
        Anim_SetSequence(p + 0x4bc, *(void **)(p + 0x4ec), seqs.seq[idx], 0, 0);
        StepAnim((struct GameStartAnim *)(p + 0x4bc), (struct GameStartSprite *)(p + 0x474));
        W(0x474) &= ~1;
        for (i = 0; i <= 11; i++) {
            Bg_SetTilemapEntry(3, i + 8, 6, names[idx][i], 0, 2);
            zp = &zero;
            hourOnes = p + 0x234;
            minuteOnes = p + 0x354;
            minuteTens = p + 0x2f4;
            colon = p + 0x294;
            levelOnes = p + 0x414;
        }
        Menu_EraseRect(3, 10, 24, 2);
        *zp = 0;
        CpuSet(zp, text, 0x05000008);
        s = Text_LookupString(Script_ParseStringRef((void *)gUnk_020003A0[1]) +
                              SceneSequence_GetSelectedMessage());
        q = text;
        while (*s)
            *q++ = *s++;
        Menu_DrawText(3, 10, text);
        if (GS_H(0x59e) > 0)
            W(0xac) &= ~1;

        tens = p + 0x1d4;
        hours = Div(GS_W(0x614), 0x34bc0);
        minutes = Div(GS_W(0x614), 0xe10) - hours * 60;
        if (hours > 99) {
            hours = 99;
            minutes = 59;
        }
        res = p + 0x1b4;
        Anim_LoadKeyframe(hourOnes, res, (u16)Mod(hours, 10), 0);
        hours = Div(hours, 10);
        Anim_LoadKeyframe(tens, res, (u16)Mod(hours, 10), 0);
        *(u32 *)(tens + 8) &= ~1;
        *(u32 *)(tens + 0x68) &= ~1;
        Anim_LoadKeyframe(minuteOnes, res, (u16)Mod(minutes, 10), 0);
        minutes = Div(minutes, 10);
        Anim_LoadKeyframe(minuteTens, res, (u16)Mod(minutes, 10), 0);
        *(u32 *)(minuteTens + 8) &= ~1;
        *(u32 *)(minuteOnes + 8) &= ~1;
        *(u32 *)(colon + 8) &= ~1;

        if (GS_H(0x5a8) == 6) {
            level = GS_H(0x40) + 10;
            if (level > 99)
                level = 99;
        } else {
            level = GS_H(0x40);
        }
        tens = p + 0x3b4;
        levelRes = p + 0x1b4;
        Anim_LoadKeyframe(levelOnes, levelRes, (u16)Mod(level, 10), 0);
        level = Div(level, 10);
        Anim_LoadKeyframe(tens, levelRes, (u16)Mod(level, 10), 0);
        *(u32 *)(tens + 8) &= ~1;
        *(u32 *)(tens + 0x68) &= ~1;
        s = Text_LookupString(Script_ParseStringRef((void *)gUnk_020003A0[0]) + 3);
        Menu_EraseRect(0x15, 7, 3, 2);
        Menu_DrawText(0x15, 7, s);
    } else {
        for (i = 0; i <= 8; i++)
            Bg_SetTilemapEntry(3, i + 8, 6, 1, 0, 2);
        W(0x474) |= 1;
        s = Text_LookupString(Script_ParseStringRef((void *)gUnk_020003A0[0]) + 2);
        Menu_EraseRect(8, 6, 0x12, 2);
        Menu_DrawText(8, 6, s);
    }
    s = Text_LookupString(Script_ParseStringRef((void *)gUnk_020003A0[0]) + 9);
    Menu_EraseRect(3, 0xe, 0x18, 2);
    Menu_DrawText(3, 0xe, s);
}
