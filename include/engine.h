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

/* ---- Globals ---------------------------------------------------------- */

struct Task {
    struct Task *prev, *next;
    void (*update)(struct Task *);  /* called every frame while alive */
    void (*destroy)(struct Task *); /* called once after Task_Kill */
    u16 id;                         /* unique, never 0 (gUnk_03005240 counter) */
    u16 flags;                      /* bit 0: marked for deletion */
    u8 list;                        /* index into gUnk_03005280 (priority list, 0..13) */
};

struct TaskList {
    struct Task *head;
    u32 pauseMask; /* list is skipped while (gUnk_0300523C & pauseMask) */
};

struct HeapBlock {
    struct HeapBlock *prev, *next;
    u32 size; /* low 20 bits: size including this 16 byte header; bit 31: free */
    u32 unused;
};

/* Terrain/collision map header of the current scene. */
struct EngineMap {
    u32 unused0;
    u8 *cells; /* +0 u16 width? (`((s16 *)cells)[2]` is the row stride), cells start at +12 */
    u8 pad8[28];
    u16 rows[1]; /* first cell index of each row */
};

extern u32 gUnk_0300523C; /* engine pause/suspend flags (bits 1-2: sound off; list masks) */
extern u16 gUnk_03005240; /* next Task id */
extern struct TaskList gUnk_03005280[14]; /* task lists by priority */
extern struct HeapBlock gUnk_02000714;    /* head of the general heap (Heap_Alloc/Heap_Free) */
extern u8 *gUnk_02000710;                 /* main game-state block (guess: save/world state) */
extern struct EngineMap *gUnk_030052F4;   /* current terrain map */
extern u32 gUnk_030052F8, gUnk_030052FC;  /* terrain map width, height (cells) */
extern u32 gUnk_03005308;                 /* index (0..1023) into the random table */
extern const u16 gUnk_0203B400[];         /* 1024 entry random table (guess) */
extern const s16 gUnk_086149C4[];         /* sine table (guess) */
extern u32 gUnk_030053F4; /* more engine flags (bit 10 mutes Sound_PlaySongChecked; guess) */
extern u8 *gUnk_030042E4; /* graphics tile bank used by AnimData_SelectTiles */

/* ---- Tasks and memory -------------------------------------------------- */

#define Task_Kill sub_0821A0C0
s32 Task_Kill(void *task); /* mark for deletion (flag bit 0); always returns 0 */

#define Task_SetCallbacks sub_0821A04C
void Task_SetCallbacks(void *task, void (*update)(void *), void (*destroy)(void *));

#define Task_Create sub_08219FBC
void *Task_Create(u32 list, u32 size); /* allocate zeroed task of size bytes in a priority list */

#define Heap_Alloc sub_08219C40
void *Heap_Alloc(s32 size); /* first-fit allocation from gUnk_02000714; NULL on failure */

#define Heap_Free sub_08219D38
void Heap_Free(void *block); /* free and coalesce with neighbours */

#define Memory_Clear sub_08219DD8
void Memory_Clear(void *dst, s32 bytes); /* zero fill (bytes, then CpuSet words) */

#define Memory_Copy sub_08219EBC
void Memory_Copy(void *dst, const void *src, s32 bytes);

/* libgcc _call_via_r1 / _call_via_r2 (bx r1 / bx r2), aliased in symbols/ram.ld.
 * Explicit calls are a matching workaround; prefer calling the function pointer. */
s32 sub_0824923C(void *arg, void *function);
s32 sub_08249240(void *arg0, void *arg1, void *function);

/* ---- Sound -------------------------------------------------------------- */

#define Sound_PlaySong sub_0822B2F8
void Sound_PlaySong(u32 song); /* m4aSongNumStart, remembering the song per track unless muted */

#define Sound_PlaySongChecked sub_0822B3BC
void Sound_PlaySongChecked(
    u32 song); /* Sound_PlaySong unless a flag in gUnk_030053F0/F4 is set (guess) */

#define Sound_StopSong sub_0822B358
void Sound_StopSong(u32 song);

/* ---- Script reader ------------------------------------------------------ */

s32 Script_GetValue(void);
s32 Script_SeekToKeyword(s32 keyword);
u8 *Script_GetPc(void);
s32 Script_ParseStringRef(const void *pc);
u8 *Text_LookupString(s32 id);

#define Script_GetKeywordValue sub_0821ABA8
s32 Script_GetKeywordValue(s32 keyword,
                           s32 defaultValue); /* value operand after keyword, or default */

#define ScriptProc_Start sub_0821A520
void *ScriptProc_Start(u16 key,
                       u16 context); /* (guess) start a script procedure; returns a handle */

#define ScriptProc_Stop sub_0821AD08
void ScriptProc_Stop(void *handle,
                     s32 arg); /* (guess) release/finish a handle from ScriptProc_Start */

#define Trigger_ConsumePending sub_08020D68
u32 Trigger_ConsumePending(
    void *latch, u32 expected); /* (guess) if latch byte +3 == expected: fire and clear; 1 if so */

/* ---- Sprites, animation, resources ------------------------------------- */

#define Sprite_Remove sub_08214514
void Sprite_Remove(void *sprite); /* unlink from pool gUnk_030042C0 (guess) */

#define Anim_Remove sub_082195E0
void Anim_Remove(void *anim); /* unlink from pool gUnk_030042C8 (guess) */

#define Effect_Remove sub_08217EAC
void Effect_Remove(void *effect); /* unlink from pool gUnk_030042D0 (guess) */

#define Sprite_Init sub_082144A4
void Sprite_Init(void *sprite, void *animData,
                 s32 flags); /* (guess) set up an idle sprite object */

#define AnimData_SelectTiles sub_08215284
void AnimData_SelectTiles(void *animData,
                          s32 index); /* (guess) point at 32 byte tile `index` of gUnk_030042E4 */

#define AnimData_Load sub_082151E4
s32 AnimData_Load(
    void *animData,
    s32 id); /* (guess) fill from the resource entry with this id (header 0x03004300) */

#define AnimData_FindEntry sub_08215184
void *AnimData_FindEntry(s32 id); /* (guess) entry with this id in header gUnk_030042EC, or NULL */

#define Anim_SetSequence sub_08220C8C
u32 Anim_SetSequence(void *anim, void *data, u32 sequence, u32 row,
                     u32 flags); /* (guess) 1 if restarted */

#define Anim_SetKeyframes sub_08220D78
void Anim_SetKeyframes(void *anim, void *data, u32 index, u32 mode, u32 flags); /* (guess) */

#define Anim_Step sub_08220F70
s32 Anim_Step(void *anim,
              void *data); /* (guess) advance one tick; nonzero when the animation ended */

#define Anim_LoadKeyframe sub_0821980C
s32 Anim_LoadKeyframe(void *anim, void *data, u32 index,
                      u32 mode); /* -1 when the entry is missing */

#define ResourceHeader_Relocate sub_082196C4
s32 ResourceHeader_Relocate(void *dst,
                            void *src); /* (guess) copy 32 bytes, rebase words 3..7 by src */

/* ---- Colliders --------------------------------------------------------- */

#define Collider_Init sub_0821FEB4
void Collider_Init(void *collider, u16 id, u32 flags, u32 arg3, u32 arg4, void *size, void *offset);

#define Collider_SetPosition sub_0821FF24
void Collider_SetPosition(void *collider, void *position, u32 value);

#define Collider_SetCallback sub_0821FF84
void Collider_SetCallback(void *collider, void *callback, void *owner);

#define Collider_Refresh sub_0821FE40
void Collider_Refresh(
    void *collider); /* (guess) re-link if flag 0x2000, otherwise run sub_0821FDC8 */

#define Collider_Remove sub_0821FE6C
void Collider_Remove(void *collider); /* unlink from the active collider list */

/* ---- Math -------------------------------------------------------------- */

#define Math_Atan2 sub_082215E4
s32 Math_Atan2(s32 x,
               s32 y); /* angle in 1/256 turns (0 = +x, 0x40 = +y) from a table; (0,0) -> 0 */

/* ---- Backgrounds and text ---------------------------------------------- */

#define Bg_ClearTilemap sub_082156B8
void Bg_ClearTilemap(s32 bg); /* zero the 0x800 byte tilemap of a background */

#define BgGfx_LoadTilemapSimple sub_0821656C
void BgGfx_LoadTilemapSimple(u32 a, u32 b, u32 c, u32 d,
                             u32 e); /* BgGfx_LoadTilemap with last argument 0 */

#define TextWindow_BlankRect sub_08218E5C
void TextWindow_BlankRect(s32 x, s32 y, s32 width,
                          s32 height); /* (guess) mark BG0 tiles in a rect as blank */

#define TextState_Reset sub_08032C34
void TextState_Reset(void *state);

#define TextState_SetBounds sub_08032C54
void TextState_SetBounds(void *state, u32 left, u32 top, u32 width, u32 height);

#define TextWindow_Find sub_08033634
void *TextWindow_Find(u32 id);

#define TextWindow_Refresh sub_08033924
s32 TextWindow_Refresh(
    s32 id,
    s32 index); /* (guess) restart the window's text at sequence entry `index`; -1 if none */

#define TextWindow_SetScript sub_0803386C
s32 TextWindow_SetScript(s32 id, const u8 *script); /* -1 if the window does not exist */

/* ---- Actors ------------------------------------------------------------ */

#define Actor_PlayAnimationEntry sub_0802DCD8
s32 Actor_PlayAnimationEntry(void *actor, s32 id, s32 a, s32 b,
                             s32 c); /* (guess) by entry kind (1..4) */

#define ActorShadow_Enable sub_08013424
void ActorShadow_Enable(
    void *actor); /* (guess) clear flag bit 0, install Shadow_UpdateTerrain at +0x34 */

#define ActorShadow_Disable sub_08013440
void ActorShadow_Disable(void *actor); /* (guess) set flag bit 0 and install a no-op at +0x34 */

#endif
