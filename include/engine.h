#ifndef GUARD_ENGINE_H
#define GUARD_ENGINE_H

/*
 * Shared declarations for the engine core (ROM 0x08213000-0x0822FFFF and the
 * widely used helpers elsewhere): tasks, heap, script reader, animation,
 * colliders, sound and text windows. Names are provisional; "(guess)" marks
 * interpretations that have not been confirmed. The `#define NewName sub_X`
 * aliases are turned into real names by tools/promote_names.py.
 */

#include "global.h"
#include "game_state.h"
#include "render.h"

/* ---- Globals ---------------------------------------------------------- */

struct Task {
    struct Task *prev, *next;
    void (*update)(struct Task *);  /* called every frame while alive */
    void (*destroy)(struct Task *); /* called once after Task_Kill */
    u16 id;                         /* unique, never 0 (gNextTaskId counter) */
    u16 flags;                      /* bit 0: marked for deletion */
    u8 list;                        /* index into gTaskLists (priority list, 0..13) */
};

struct TaskList {
    struct Task *head;
    u32 pauseMask; /* list is skipped while (gEnginePauseFlags & pauseMask) */
};

struct HeapBlock {
    struct HeapBlock *prev, *next;
    u32 size; /* low 20 bits: size including this 16 byte header; bit 31: free */
    u32 unused;
};

/* Terrain/collision map header of the current scene. Terrain_LoadHP (08225C48)
 * fills it from the stage's "HP" resource (type AE6C, the third argument of the
 * script's proc_884(map, tileset, hp); format in docs/ASSETS.md, decoder
 * tools/assets/hp.py): +4 the grid, +0xC named boxes (doors, camera area),
 * +0x10 patrol paths, +0x14 nav meshes. +0x1C..+0x22 are the neighbour index
 * offsets -width, +1, +width, -1 (Terrain_SetNeighborOffsets). */
struct TerrainTrap {
    u32 serial;
    u16 f4, f6, id, fa, flags, fe;
    u16 w[4], s[4];
    u32 f20, command;
    void *f28;
};

struct EngineMap {
    u16 trapCount;
    u16 unknown2;
    u8 *cells; /* grid: +0 u32 0, +4 u16 width (the row stride), +6 u16 height, +8 s16 origin x/y
                  (BG map pixels * 16), cells (struct SceneCell) from +12 */
    void *resource8;
    void *boxes;
    void *routes;
    void *navigation;
    void *boxList;
    s16 neighborOffsets[4];
    u16 rows[256]; /* first cell index of each row */
    struct TerrainTrap traps[64];
    void *trapTops[64];
};

extern u16 gNextTaskId;                   /* next Task id */
extern struct TaskList gTaskLists[14];    /* task lists by priority */
extern struct HeapBlock gUnk_02000714;    /* head of the general heap (Heap_Alloc/Heap_Free) */
extern struct EngineMap *gTerrainMap;     /* current terrain map */
extern u32 gTerrainWidth, gTerrainHeight; /* terrain map width, height (cells) */
extern u32 gRandomIndex;                  /* index (0..1023) into the random table */
extern const u16 gUnk_0203B400[];         /* 1024 entry random table (guess) */
extern const s16 gUnk_086149C4[];         /* sine table (guess) */
extern u32 gEngineFlags; /* more engine flags (bit 10 mutes Sound_PlaySongChecked; guess) */

/* ---- Tasks and memory -------------------------------------------------- */

s32 Task_Kill(void *task); /* mark for deletion (flag bit 0); always returns 0 */

void Task_SetCallbacks(void *task, void (*update)(void *), void (*destroy)(void *));

void *Task_Create(u32 list, u32 size); /* allocate zeroed task of size bytes in a priority list */

void *Heap_Alloc(s32 size); /* first-fit allocation from gUnk_02000714; NULL on failure */

void Heap_Free(void *block); /* free and coalesce with neighbours */

void Memory_Clear(void *dst, s32 bytes); /* zero fill (bytes, then CpuSet words) */

void Memory_Copy(void *dst, const void *src, s32 bytes);

/* libgcc _call_via_r1 / _call_via_r2 (bx r1 / bx r2), aliased in symbols/ram.ld.
 * Explicit calls are a matching workaround; prefer calling the function pointer. */
s32 Callback_InvokeActor(void *arg, void *function);
s32 sub_08249240(void *arg0, void *arg1, void *function);

/* ---- Sound -------------------------------------------------------------- */

void Sound_PlaySong(u32 song); /* m4aSongNumStart, remembering the song per track unless muted */

void Sound_PlaySongChecked(
    u32 song); /* Sound_PlaySong unless a flag in gScenePauseFlags/F4 is set (guess) */

void Sound_StopSong(u32 song);

/* ---- Script reader ------------------------------------------------------ */

s32 Script_GetValue(void);
s32 Script_SeekToKeyword(s32 keyword);
u8 *Script_GetPc(void);
s32 Script_ParseStringRef(const void *pc);
u8 *Text_LookupString(s32 id);

s32 Script_GetKeywordValue(s32 keyword,
                           s32 defaultValue); /* value operand after keyword, or default */

void *ScriptProc_Start(u16 key, u16 context); /* misnamed: resource lookup; key = extension hash,
                                        context = name hash; returns the data (docs/ASSETS.md) */

s32 Script_Call(
    u32 key,
    void *args); /* look up script `key` and run it with args; returns its result (guess) */

u32 Trigger_ConsumePending(
    void *latch, u32 expected); /* (guess) if latch byte +3 == expected: fire and clear; 1 if so */

/* ---- Sprites, animation, resources ------------------------------------- */

void Sprite_Remove(void *sprite); /* unlink from pool gSpriteListHeads (guess) */

void Anim_Remove(void *anim); /* unlink from pool gMainSpriteListHeads (guess) */

void Effect_Remove(void *effect); /* unlink from pool gDrawableListHeads (guess) */

void Sprite_Init(void *sprite, void *animData,
                 s32 flags); /* (guess) set up an idle sprite object */

void AnimData_SelectPaletteBank(
    void *animData,
    s32 index); /* select a 32-byte bank of sixteen RGB15 colors from gPaletteBankData */

s32 AnimData_Load(
    void *animData,
    s32 id); /* (guess) fill from the resource entry with this id (header 0x03004300) */

void *AnimData_FindEntry(s32 id); /* (guess) entry with this id in header gUnk_030042EC, or NULL */

u32 Anim_SetSequence(void *anim, void *data, u32 sequence, u32 row,
                     u32 flags); /* (guess) 1 if restarted */

void Anim_SetKeyframes(void *anim, void *data, u32 index, u32 mode, u32 flags); /* (guess) */

s32 Anim_Step(void *anim,
              void *data); /* (guess) advance one tick; nonzero when the animation ended */

s32 Anim_LoadKeyframe(void *anim, void *data, u32 index,
                      u32 mode); /* -1 when the entry is missing */

s32 ResourceHeader_Relocate(void *dst,
                            void *src); /* (guess) copy 32 bytes, rebase words 3..7 by src */

/* ---- Colliders --------------------------------------------------------- */

void Collider_Init(void *collider, u16 id, u32 flags, u32 arg3, u32 arg4, void *size, void *offset);

void Collider_SetPosition(void *collider, void *position, u32 value);

void Collider_SetCallback(void *collider, void *callback, void *owner);

s32 Collider_Refresh(
    void *collider); /* (guess) re-link if flag 0x2000, otherwise run Collider_Register */

void Collider_Remove(void *collider); /* unlink from the active collider list */

/* ---- Math -------------------------------------------------------------- */

s32 Math_Atan2(s32 x,
               s32 y); /* angle in 1/256 turns (0 = +x, 0x40 = +y) from a table; (0,0) -> 0 */

/* ---- Backgrounds and text ---------------------------------------------- */

void Bg_ClearTilemap(s32 bg); /* zero the 0x800 byte tilemap of a background */

void BgGfx_LoadTilemapSimple(u32 a, u32 b, u32 c, u32 d,
                             u32 e); /* BgGfx_LoadTilemap with last argument 0 */

void TextWindow_BlankRect(s32 x, s32 y, s32 width,
                          s32 height); /* (guess) mark BG0 tiles in a rect as blank */

void TextState_Reset(void *state);

void TextState_SetBounds(void *state, u32 left, u32 top, u32 width, u32 height);

void *TextWindow_Find(u32 id);

s32 TextWindow_Refresh(
    s32 id,
    s32 index); /* (guess) restart the window's text at sequence entry `index`; -1 if none */

s32 TextWindow_SetScript(s32 id, const u8 *script); /* -1 if the window does not exist */

/* ---- Actors ------------------------------------------------------------ */

s32 Actor_PlayAnimationEntry(void *actor, s32 id, s32 a, s32 b,
                             s32 c); /* (guess) by entry kind (1..4) */

void ActorShadow_Enable(
    void *actor); /* (guess) clear flag bit 0, install Shadow_UpdateTerrain at +0x34 */

void ActorShadow_Disable(void *actor); /* (guess) set flag bit 0 and install a no-op at +0x34 */

#endif
