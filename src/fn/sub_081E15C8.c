// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "game_state.h"
#include "ram.h"
#include "render.h"

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
    u8 unk12[0xA];
    u16 x, y;
    u8 unk20[0xC];
};

struct GameStartWidget {
    u8 unk00[8];
    u32 flags;
    u8 unk0C[0x14];
    u16 x;
    u16 y;
    u8 unk24[0x3C];
};
struct GameStart {
    u8 unk000[0x1C];
    struct GameStartWidget background;
    struct GameStartRes backgroundResource;
    u8 unk09C[8];
    struct GameStartWidget decoration;
    struct GameStartRes decorationResource;
    u8 unk124[8];
    struct GameStartWidget title;
    struct GameStartRes titleResource;
    u8 unk1AC[8];
    struct GameStartRes itemResource;
    struct GameStartWidget items[5];
    struct GameStartWidget choices[2];
    struct GameStartSprite cursor;
    struct GameStartAnim cursorData;
    u8 unk4B0[0xC];
    struct GameStartAnim cursorSequence;
    s32 active;
    u8 unk4D0[8];
    s32 timer;
    u8 unk4DC[8];
    s32 saveIndex;
    u8 unk4E8[4];
    void *cursorResource;
};

extern u32 gUnk_020003A0[];
extern u8 gUnk_020005B4[];

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
#define GS_H(o) (*(s16 *)((u8 *)gGameState + (o)))

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
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    {
        void *bg = ScriptProc_Start(0xc091, 0x3536);
        layers = 4;
        Bg_SetupLayers(2, 0, bg, 0, 0, 1, &layers);
    }
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet(((u8 *)ScriptProc_Start(0x92b3, 0x204)) + 0x14, gBgPaletteBuffer, 0x100);
    res = ScriptProc_Start(0xcb05, 0xe2ab);
    ResourceHeader_Relocate((u8 *)&((struct GameStart *)p)->backgroundResource, res);
    ((struct GameStart *)p)->itemResource = ((struct GameStart *)p)->backgroundResource;
    Widget_InitLinked((u8 *)&((struct GameStart *)p)->background,
                      (u8 *)&((struct GameStart *)p)->backgroundResource, 0x36, 0x10, 0, 0, 0, 0);
    ResourceHeader_Relocate((u8 *)&((struct GameStart *)p)->decorationResource, res);
    Widget_InitLinked((u8 *)&((struct GameStart *)p)->decoration,
                      (u8 *)&((struct GameStart *)p)->decorationResource, 0x28, 0x10, 0, 0, 0, 0);
    ((struct GameStart *)p)->decoration.flags |= 1;
    ((struct GameStart *)p)->decoration.x = 0;
    {
        u16 *targetY = &((struct GameStart *)p)->decoration.y;
        s32 k1 = 0xffe0;
        *targetY = k1;
    }
    {
        u32 group = 0xcb05;
        res = ScriptProc_Start(group, 0x530d);
    }
    ResourceHeader_Relocate((u8 *)&((struct GameStart *)p)->titleResource, res);
    Widget_InitLinked((u8 *)&((struct GameStart *)p)->title,
                      (u8 *)&((struct GameStart *)p)->titleResource, 0x8a, 0x10, 0, 0, 0, 0);
    ((struct GameStart *)p)->title.x = 0;
    {
        u16 *targetY = &((struct GameStart *)p)->title.y;
        s32 valueY = 0xfff8;
        *targetY = valueY;
    }
    AnimData_Load((u8 *)&((struct GameStart *)p)->cursorData, 0x593a);
    Sprite_Init((u8 *)&((struct GameStart *)p)->cursor, (u8 *)&((struct GameStart *)p)->cursorData,
                0x11);
    ((struct GameStart *)p)->cursor.x = 0x18;
    ((struct GameStart *)p)->cursor.y = 0x2c;
    ((struct GameStart *)p)->cursorResource = ScriptProc_Start(0x922e, 0x4bbf);
    Anim_SetSequence((u8 *)&((struct GameStart *)p)->cursorSequence,
                     ((struct GameStart *)p)->cursorResource, 0, 0, 0);
    StepAnim_Init((struct GameStartAnim *)((u8 *)&((struct GameStart *)p)->cursorSequence),
                  (struct GameStartSprite *)((u8 *)&((struct GameStart *)p)->cursor));
    pos.x = 0xa8;
    pos.y = 0x28;
    for (i = 0; i <= 4; i++) {
        Widget_InitLinked((u8 *)&((struct GameStart *)p)->items[i],
                          (u8 *)&((struct GameStart *)p)->itemResource, (i == 2) ? (0x14) : (0),
                          0x30, 1, 0, 0x3c, &pos);
        ((struct GameStart *)p)->items[i].x += i * 8;
        ((struct GameStart *)p)->items[i].flags |= 1;
    }

    pos.x = 0xc0;
    pos.y = 0x38;
    for (i = 0; i <= 1; i++) {
        Widget_InitLinked((u8 *)&((struct GameStart *)p)->choices[i],
                          (u8 *)&((struct GameStart *)p)->itemResource, 0, 0x30, 1, 0, 0x3c, &pos);
        ((struct GameStart *)p)->choices[i].x += i * 8;
        ((struct GameStart *)p)->choices[i].flags |= 1;
    }

    ((struct GameStart *)p)->timer = 0;
    if (gUnk_020003A0[6] == 1)
        Menu_StartFadeIn(0);
    else
        Menu_StartFadeIn(3);
    if (gUnk_020003A0[6] == 0) {
        u8 *name = gAnimPanelSavedSelections;
        u8 *slot = gUnk_020005B4;
        u8 *end = name;
        s32 blank = 0x41;
        u8 *q = name + 0x17;
        do {
            *q = blank;
            q--;
        } while (((s32)q) >= ((s32)end));
        name[0x18] = 0;
        slot[0] = 4;
        slot[1] = 0;
        if (gUnk_020003A0[6] == 0) {
            ((struct GameStart *)p)->saveIndex = GameStart_LoadAvailableSave(p);
            Sound_PlaySong(0xb3);
            goto check;
        }
    }
    if ((*((s16 *)(((u8 *)gGameState) + 0x86e))) != 0)
        GameState_RestoreActiveRegions();
check:
    if ((*((s16 *)(((u8 *)gGameState) + 0x86e))) == 0)
        ((struct GameStart *)p)->active = 1;

    GameStart_DrawSaveInfo(p);
    GameStart_RunInitHook(p);
    return 0;
}
