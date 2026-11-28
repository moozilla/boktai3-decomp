#include "objects/animated_tile.h"
#include "engine/ram.h"
#include "engine/video/display_sprite_playback.h"
#include "engine/engine.h"
#include "script/script_utility.h"

static inline u8 AnimatedTile_TestSlot(struct AnimatedTileManager *manager, s32 i)
{
    if (manager->used & (1 << i))
        return TRUE;
    return FALSE;
}

struct AnimatedTile *AnimatedTile_Allocate(struct AnimatedTileManager *manager)
{
    s32 i;

    for (i = 0; i < 3; i++) {
        if (!AnimatedTile_TestSlot(manager, i)) {
            struct AnimatedTile *tile = &manager->tiles[i];
            manager->used |= 1 << i;
            Memory_Clear(tile, sizeof(*tile));
            tile->slot = i;
            return tile;
        }
    }
    return 0;
}

/* Provisional names and field interpretations; confidence low. */

struct AnimatedTile *AnimatedTilePool_FindByName(u32 name)
{
    struct AnimatedTileManager *manager = (void *)gNamedEntryPool;
    s32 i;
    struct AnimatedTile *tile;

    if (!manager)
        return 0;
    for (i = 0; i < 3; i++) {
        if (AnimatedTile_TestSlot(manager, i)) {
            tile = &manager->tiles[i];
            if (tile->name == name)
                return tile;
        }
    }
    return 0;
}

struct AnimatedTileManager *AnimatedTile_CreateManager(void);

struct AnimatedTile *AnimatedTile_Allocate(struct AnimatedTileManager *);

/* Registry routine 8192: takes a slot of the animated tile manager
   (0x020001BC, created on demand) with sprite 0x3876: -p x y z, -R (2), -r
   sequence, -n, -C, -c palette bank (+0x304); then starts the sequence and
   steps it once (tentative). */

void AnimatedTile_CreateFromScript(void)
{
    struct AnimatedTileManager *manager = (void *)gNamedEntryPool;
    struct AnimatedTile *allocated;
    struct AnimatedTile *tile;
    s32 palette;

    if (!manager) {
        manager = AnimatedTile_CreateManager();
        if (!manager)
            return;
    }
    allocated = AnimatedTile_Allocate(manager);
    if (!allocated)
        return;
    tile = allocated;
    AnimData_Load(&tile->graphics, 0x3876);
    Sprite_Init(tile, &tile->graphics, 0);
    tile->state = 1;
    tile->entered = 1;
    if (Script_SeekToKeyword('p')) {
        tile->sprite.position.x = Script_GetValue();
        tile->sprite.position.y = Script_GetValue();
        tile->sprite.position.z = Script_GetValue();
    }
    tile->sprite.priority = Script_GetKeywordValue('R', 2);
    tile->sequence = Script_GetKeywordValue('r', 0);
    tile->name = Script_GetKeywordValue('n', 0);
    tile->script = Script_GetKeywordValue('C', 0);
    palette = Script_GetKeywordValue('c', 0);
    AnimData_SelectPaletteBank((void *)tile->sprite.resource, palette + 0x304);
    Anim_SetSequence(&tile->animation, manager->sequences, 0, 0, tile->sequence);
    PlayerCue_StepSequenceEndedWithFirstFlipMask((void *)tile, &tile->animation, 1);
}

/* AnimatedTile_UpdateIdleState: Empty entry preceding an animation state handler; provisionally an idle state hook */

void AnimatedTile_UpdateIdleState(void)
{
}

void Sound_PlaySong(u32);

void AnimatedTile_TickAlternateAnimation(struct AnimatedTileManager *manager,
                                         struct AnimatedTile *tile)
{
    if (tile->entered) {
        Sound_PlaySong(0x10f);
        Anim_SetSequence(&tile->animation, manager->sequences, 5, 0, tile->sequence);
        tile->entered = 0;
    }
    if (PlayerCue_StepSequenceEnded((void *)&tile->sprite, &tile->animation)) {
        tile->state = 3;
        tile->entered = 1;
    }
}

/* Provisional names and field interpretations; confidence low. */

void AnimatedTilePool_UpdateDelay(void *unused, struct AnimatedTile *entry)
{
    if (entry->entered) {
        entry->timer = 0;
        entry->entered = 0;
    }
    if (entry->timer > 10) {
        entry->state = 4;
        entry->entered = 1;
    }
    entry->timer++;
}

u8 Sprite_BlinkUntilExpired(void *, u32, s32, s32);

void AnimatedTile_TickFadeOut(struct AnimatedTileManager *manager, struct AnimatedTile *tile)
{
    s32 value;
    struct ScriptUtilityArguments args;
    if (tile->entered) {
        value = tile->name;
        args.values = &value;
        args.count = 1;
        Script_Call(tile->script, &args);
        tile->timer = 0;
        tile->entered = 0;
    }
    if (Sprite_BlinkUntilExpired(tile, tile->timer, 45, 90)) {
        s32 slot = tile->slot;
        u32 *used = &manager->used;
        *used &= ~(1 << slot);
        Sprite_Remove(tile);
        tile->state = 0;
    } else
        tile->timer++;
}

/* AnimatedTile_OnStateComplete: Empty entry following animation state code; provisionally a completion hook */

void AnimatedTile_OnStateComplete(void)
{
}

extern void *gUnk_08606530[];

s32 sub_08249240(void *, void *, void *);

static inline void AnimatedTile_DispatchOccupied(struct AnimatedTileManager *manager,
                                                 struct AnimatedTile *tile)
{
    sub_08249240(manager, tile, gUnk_08606530[tile->state]);
}

s32 CallbackSlots_DispatchActive(struct AnimatedTileManager *manager)
{
    struct AnimatedTile *tiles = manager->tiles;
    s32 i;

    if (manager->used == 0)
        return 0;
    for (i = 0; i < 3; i++) {
        if (AnimatedTile_TestSlot(manager, i))
            AnimatedTile_DispatchOccupied(manager, &tiles[i]);
    }
    return 0;
}

void Sprite_Remove(void *);

u32 AnimatedTile_OnManagerDestroy(struct AnimatedTileManager *manager)
{
    s32 i;

    for (i = 0; i < 3; i++) {
        if (AnimatedTile_TestSlot(manager, i))
            Sprite_Remove(&manager->tiles[i]);
    }
    return gNamedEntryPool = 0;
}

s32 AnimatedTile_InitManager(struct AnimatedTileManager *manager)
{
    manager->sequences = ScriptProc_Start(0x922E, 0x9AF2);
    gNamedEntryPool = (u32)manager;
    return manager->used = 0;
}

struct AnimatedTileManager *AnimatedTile_CreateManager(void)
{
    struct AnimatedTileManager *p;
    if (gNamedEntryPool != 0)
        return (void *)gNamedEntryPool;
    p = Task_Create(0x8, sizeof(*p));
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))CallbackSlots_DispatchActive,
                          (void (*)(void *))AnimatedTile_OnManagerDestroy);
        if (AnimatedTile_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

void AnimatedTile_ResetManager(void)
{
    gNamedEntryPool = 0;
}

void AnimatedTile_StartAlternateAnimationFromScript(void)
{
    u32 name = Script_GetKeywordValue('n', 0);
    if (name != 0) {
        struct AnimatedTile *tile = AnimatedTilePool_FindByName(name);
        if (tile != 0) {
            tile->state = 2;
            tile->entered = 1;
        }
    }
}
