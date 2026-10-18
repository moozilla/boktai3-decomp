#include "global.h"
#include "directional_interaction.h"
struct AnimatedTile {
    u8 unknown0[0x58];
    s32 name;
    u32 script;
    u16 state;
    s16 slot;
    u8 sequence, entered;
    u16 frame;
};
struct AnimatedTileManager {
    u8 unknown0[0x154];
    u32 used;
};
u8 Sprite_BlinkUntilExpired(void *, u32, s32, s32);
void AnimatedTile_TickFadeOut(struct AnimatedTileManager *manager, struct AnimatedTile *tile)
{
    s32 value;
    struct DirectionalScriptArgs args;
    if (tile->entered) {
        value = tile->name;
        args.values = &value;
        args.count = 1;
        Script_Call(tile->script, &args);
        tile->frame = 0;
        tile->entered = 0;
    }
    if (Sprite_BlinkUntilExpired(tile, tile->frame, 45, 90)) {
        s32 slot = tile->slot;
        u32 *used = &manager->used;
        *used &= ~(1 << slot);
        Sprite_Remove(tile);
        tile->state = 0;
    } else
        tile->frame++;
}
