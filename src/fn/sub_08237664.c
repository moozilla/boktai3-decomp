#include "terrain_edge_marker.h"

s32 MovingSprite_Initialize(void *, union TerrainMarkerPosition *, u32, u32, u32, u32, u32, u32);

s32 sub_08237664(void *p)
{
    union TerrainMarkerPosition position;
    s32 palette, id, angle, speed, delay, duration;
    if (Script_SeekToKeyword('p')) {
        position.packed.x = Script_GetValue();
        position.packed.y = Script_GetValue();
        position.packed.z = Script_GetValue();
    } else {
        u32 mask = 0xFFFF0000;
        position.words[0] = 0;
        position.words[1] &= mask;
    }
    if (Script_SeekToKeyword('c'))
        palette = Script_GetValue() + 0x4B;
    else
        palette = 0x4B;
    if (Script_SeekToKeyword('E'))
        id = Script_GetValue();
    else
        id = 0;
    if (Script_SeekToKeyword('d'))
        angle = Script_GetValue();
    else
        angle = 0;
    if (Script_SeekToKeyword('s'))
        speed = Script_GetValue();
    else
        speed = 0x20;
    if (Script_SeekToKeyword('w'))
        delay = Script_GetValue();
    else
        delay = 0;
    if (Script_SeekToKeyword('l'))
        duration = Script_GetValue();
    else
        duration = 0x5A;
    MovingSprite_Initialize(p, &position, palette, id, angle, speed, delay, duration);
    return 0;
}
