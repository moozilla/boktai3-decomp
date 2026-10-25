// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "game_state.h"

#ifdef NONMATCHING
// fuzzy: 92.2%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
struct GameStartRes {
    u32 w[8];
};
struct GameStartPos {
    u32 x : 16;
    u32 y : 16;
    u32 z;
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

struct GameStartWidget {
    u8 unk00[8];
    u32 flags;
    u8 unk0C[0x14];
    u16 x;
    u8 unk22[0x3E];
};
struct GameStart {
    u8 unk000[0x1d4];
    struct GameStartWidget items[5];
    struct GameStartWidget choices[2];
};

extern u32 gUnk_020003A0[];
extern u8 gUnk_020005C0[];
extern u8 gUnk_020005B4[];
extern u16 gUnk_03004FC0[];

void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void Anim_SetSequence(void *, void *, u16, u32, u32);
void Menu_StartFadeIn(u32);
s32 GameStart_LoadAvailableSave(void *);
void Sound_PlaySong(s32);
void GameStart_DrawSaveInfo(void *);
void GameStart_RunInitHook(void *);

#define W(o)    (*(s32 *)(p + (o)))
#define U16(o)  (*(u16 *)(p + (o)))
#define GS_H(o) (*(s16 *)((u8 *)gUnk_02000710 + (o)))

static inline void StepAnim(struct GameStartAnim *a, struct GameStartSprite *s)
{
    const volatile u16 *entry = a->data + a->frame;

    s->frame = *entry >> 6;
    {
        u32 left = a->flags & 1;
        if (left != (((*entry & 0x30) >> 4) & 1))
            s->flags |= 4;
        else
            s->flags &= ~4;
    }
    {
        /* Load the flags before materializing the shared mask. */
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

// Tentative: sets up the Game Start screen (backgrounds, widgets, cursor sprite, save state).
inline static void StepAnim_Init(struct GameStartAnim *a, struct GameStartSprite *s)
{
    const volatile u16 *entry = a->data + a->frame;
    s->frame = (*entry) >> 6;
    {
        u32 left = a->flags & 1;
        if (left != ((u8)((((*entry) & 0x30) >> 4) & 1)))
            s->flags |= 4;
        else
            s->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        if (((u8)(mask & input)) != ((((*entry) & 0x30) >> 4) & mask))
            s->flags |= 8;
        else
            s->flags &= ~8;
    }
    if (((u16)(++a->timer)) >= a->rate) {
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
        } else if (((u16)(++a->frame)) >= a->count) {
            if ((u8)(a->flags & 8)) {
                a->timer = a->rate;
                a->frame = a->count - 1;
            } else {
                a->frame = 0;
            }
        }
        entry = a->data + a->frame;
        a->factor = (*entry) & 15;
        a->rate = (a->factor * a->scale) >> 6;
        if (a->rate == 0)
            a->rate = 1;
    }
}

s32 GameStart_Init(u8 *p)
{
    u32 layers;
    struct GameStartPos pos;
    void *res;
    s32 i;
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    {
        void *bg = ScriptProc_Start(0xc091, 0x3536);
        layers = 4;
        Bg_SetupLayers(2, 0, bg, 0, 0, 1, &layers);
    }
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet(((u8 *)ScriptProc_Start(0x92b3, 0x204)) + 0x14, gUnk_03004FC0, 0x100);
    res = ScriptProc_Start(0xcb05, 0xe2ab);
    ResourceHeader_Relocate(p + 0x7c, res);
    *((struct GameStartRes *)(p + 0x1b4)) = *((struct GameStartRes *)(p + 0x7c));
    Widget_InitLinked(p + 0x1c, p + 0x7c, 0x36, 0x10, 0, 0, 0, 0);
    ResourceHeader_Relocate(p + 0x104, res);
    Widget_InitLinked(p + 0xa4, p + 0x104, 0x28, 0x10, 0, 0, 0, 0);
    *((s32 *)(p + 0xac)) |= 1;
    *((u16 *)(p + 0xc4)) = 0;
    {
        s32 k1 = 0xffe0;
        *((u16 *)(p + 0xc6)) = k1;
    }
    {
        u32 group = 0xcb05;
        asm("" : "+r"(group));
        res = ScriptProc_Start(group, 0x530d);
    }
    ResourceHeader_Relocate(p + 0x18c, res);
    Widget_InitLinked(p + 0x12c, p + 0x18c, 0x8a, 0x10, 0, 0, 0, 0);
    *((u16 *)(p + 0x14c)) = 0;
    *((u16 *)(p + 0x14e)) = 0xfff8;
    AnimData_Load(p + 0x4a0, 0x593a);
    Sprite_Init(p + 0x474, p + 0x4a0, 0x11);
    *((u16 *)(p + 0x490)) = 0x18;
    *((u16 *)(p + 0x492)) = 0x2c;
    *((void **)(p + 0x4ec)) = ScriptProc_Start(0x922e, 0x4bbf);
    Anim_SetSequence(p + 0x4bc, *((void **)(p + 0x4ec)), 0, 0, 0);
    StepAnim_Init((struct GameStartAnim *)(p + 0x4bc), (struct GameStartSprite *)(p + 0x474));
    pos.x = 0xa8;
    pos.y = 0x28;
    for (i = 0; i <= 4; i++) {
        Widget_InitLinked((p + 0x1d4) + (i * 0x60), p + 0x1b4, (i == 2) ? (0x14) : (0), 0x30, 1, 0,
                          0x3c, &pos);
        *((u16 *)(p + (0x1f4 + (i * 0x60)))) += i * 8;
        ((struct GameStart *)p)->items[i].flags |= 1;
    }

    pos.x = 0xc0;
    pos.y = 0x38;
    for (i = 0; i <= 1; i++) {
        Widget_InitLinked((p + 0x3b4) + (i * 0x60), p + 0x1b4, 0, 0x30, 1, 0, 0x3c, &pos);
        *((u16 *)(p + (0x3d4 + (i * 0x60)))) += i * 8;
        ((struct GameStart *)p)->choices[i].flags |= 1;
    }

    *((s32 *)(p + 0x4d8)) = 0;
    if (gUnk_020003A0[6] == 1)
        Menu_StartFadeIn(0);
    else
        Menu_StartFadeIn(3);
    if (gUnk_020003A0[6] == 0) {
        u8 *name = gUnk_020005C0;
        u8 *slot = gUnk_020005B4;
        u8 *end = name;
        u8 *q = name + 0x17;
        do {
            *q = 0x41;
            q--;
        } while (((s32)q) >= ((s32)end));
        name[0x18] = 0;
        slot[0] = 4;
        slot[1] = 0;
        if (gUnk_020003A0[6] == 0) {
            *((s32 *)(p + 0x4e4)) = GameStart_LoadAvailableSave(p);
            Sound_PlaySong(0xb3);
            goto check;
        }
    }
    if ((*((s16 *)(((u8 *)gUnk_02000710) + 0x86e))) != 0)
        GameState_RestoreActiveRegions();
check:
    if ((*((s16 *)(((u8 *)gUnk_02000710) + 0x86e))) == 0)
        *((s32 *)(p + 0x4cc)) = 1;

    GameStart_DrawSaveInfo(p);
    GameStart_RunInitHook(p);
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", GameStart_Init);
#endif
