#include "animated_tile.h"
#include "ram.h"
#include "display_sprite_playback.h"
#include "engine.h"
#include "script_utility.h"

static inline u8 AnimatedTile_TestSlot(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct AnimatedTile *AnimatedTile_Allocate(struct AnimatedTileManager *manager)
{
    s32 index;
    struct AnimatedTile *tile;
    for (index = 0; index < 3; index++) {
        if (!AnimatedTile_TestSlot(&manager->used, index)) {
            tile = &manager->tiles[index];
            manager->used |= 1 << index;
            Memory_Clear(tile, sizeof(*tile));
            tile->slot = index;
            return tile;
        }
    }
    return 0;
}

/* Provisional names and field interpretations; confidence low. */

static inline u8 AnimatedTile_IsUsed(u32 mask, s32 index, s32 one)
{
    if (mask & (one << index))
        return 1;
    return 0;
}

struct AnimatedTile *Pool1BC_FindByName(u32 name)
{
    struct AnimatedTileManager *pool = (void *)gNamedEntryPool;
    struct AnimatedTile *entry;
    s32 i;
    u32 used;
    s32 one;
    if (!pool)
        return 0;
    i = 0;
    used = pool->used;
    one = 1;
    entry = pool->tiles;
    do {
        if (AnimatedTile_IsUsed(used, i, one) && entry->name == name)
            return entry;
        entry++;
        i++;
    } while (i < 3);
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
    struct AnimatedTile *tile;
    struct AnimatedTile *slot;
    s32 palette;
    struct DisplaySequenceState *playback;
    u32 one;
    if (!manager) {
        manager = AnimatedTile_CreateManager();
        if (!manager)
            return;
    }
    slot = AnimatedTile_Allocate(manager);
    if (!slot)
        return;
    tile = slot;
    AnimData_Load(&tile->graphics, 0x3876);
    Sprite_Init(tile, &tile->graphics, 0);
    one = tile->state = 1;
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
    playback = &tile->animation;
    Anim_SetSequence(playback, manager->sequences, 0, 0, tile->sequence);
    PlayerCue_StepSequenceEndedWithFirstFlipMask((void *)tile, &tile->animation, one);
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

void Pool1BC_UpdateDelay(void *unused, struct AnimatedTile *entry)
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

static inline u8 AnimatedTile_IsSlotActive(u32 *used, s32 i)
{
    u32 bit = 1 << i;
    if (*used & bit)
        return 1;
    return 0;
}

static inline void AnimatedTile_DispatchOccupied(struct AnimatedTileManager *obj,
                                                 struct AnimatedTile *slots)
{
    s32 i;
    u32 *used;
    void **table;
    i = 0;
    used = &obj->used;
    table = gUnk_08606530;
    do {
        if (AnimatedTile_IsSlotActive(used, i)) {
            struct AnimatedTile *slot = (void *)((u32)i * sizeof(*slots) + (u32)slots);
            sub_08249240(obj, slot, table[slot->state]);
        }
        i++;
    } while (i < 3);
}

s32 CallbackSlots_DispatchActive(struct AnimatedTileManager *obj)
{
    struct AnimatedTile *slots = obj->tiles;
    if (!obj->used)
        return 0;
    AnimatedTile_DispatchOccupied(obj, slots);
    return 0;
}

void Sprite_Remove(void *);

u32 AnimatedTile_OnManagerDestroy(struct AnimatedTileManager *manager)
{
    s32 index = 0;
    u32 *used = &manager->used;
    for (; index < 3; index++) {
        if (AnimatedTile_TestSlot(used, index)) {
            struct AnimatedTile *tile = &manager->tiles[index];
            Sprite_Remove(tile);
        }
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
        struct AnimatedTile *tile = Pool1BC_FindByName(name);
        if (tile != 0) {
            tile->state = 2;
            tile->entered = 1;
        }
    }
}
