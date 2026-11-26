/* Provisional names and object interpretation; offsets follow static accesses. */
#define MapMenu_IsAreaUnlocked   GameState_GetMapRevealFlag
#define MapMenu_RefreshSelection MapScreen_RefreshAreaDisplay
#include "global.h"
struct MapMenuEntry {
    u8 area;
    u8 rest[15];
};
struct MapMenu {
    u8 pad[0x48f8];
    struct MapMenuEntry *entries;
    u8 pad48fc[8];
    u8 index;
    u8 unlocked;
    u8 pad4906[2];
    s8 adjacent[4];
};
extern struct {
    u16 held, pressed;
} gPadState;
u32 GameState_GetMapRevealFlag(u32);
void MapScreen_RefreshAreaDisplay(struct MapMenu *);
u32 MapMenu_SelectDirection(struct MapMenu *s)
{
    struct MapMenuEntry *entries = s->entries;
    s32 index = -1;
    u32 result;
    if (gPadState.pressed & 0x40) {
        if (s->adjacent[0] != -1) {
            index = s->adjacent[0];
            if (GameState_GetMapRevealFlag(entries[index].area))
                s->unlocked = 1;
            else
                s->unlocked = 0;
        }
    } else if (gPadState.pressed & 0x80) {
        if (s->adjacent[1] != -1) {
            index = s->adjacent[1];
            if (GameState_GetMapRevealFlag(entries[index].area))
                s->unlocked = 1;
            else
                s->unlocked = 0;
        }
    } else if (gPadState.pressed & 0x20) {
        if (s->adjacent[2] != -1) {
            index = s->adjacent[2];
            if (GameState_GetMapRevealFlag(entries[index].area))
                s->unlocked = 1;
            else
                s->unlocked = 0;
        }
    } else if (gPadState.pressed & 0x10) {
        if (s->adjacent[3] != -1) {
            index = s->adjacent[3];
            if (GameState_GetMapRevealFlag(entries[index].area))
                s->unlocked = 1;
            else
                s->unlocked = 0;
        }
    }
    if (index < 0)
        result = 0;
    else {
        s->index = index;
        MapScreen_RefreshAreaDisplay(s);
        result = 1;
    }
    return result;
}
