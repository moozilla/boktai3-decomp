#include "global.h"
#include "engine_globals.h"
#include "ram.h"
void AnimatedParticles_EmitBurst(void *, int, int, int, int, int, int, int);
void SceneActors_SpawnAmbientParticle(u8 *p)
{
    s32 k2;
    struct {
        s16 x, y, z;
    } position;
    int r;
    int cursor;
    u16 *table = gRandomTable;
    *(&cursor) = gRandomIndex + 1;
    {
        int mask = 0x3ff;
        cursor &= 0x3ff;
    }
    r = table[cursor] & (k2 = 0x7f);
    position.x = *(s16 *)(p + 0x54) + r;
    cursor = (cursor + 1) & 0x3ff;
    gRandomIndex = cursor;
    cursor = table[cursor];
    {
        u16 *y = (u16 *)(p + 0x56);
        cursor &= 0x1f;
        {
            int height = *y;
            cursor += height;
        }
        position.y = cursor;
    }
    {
        u16 *z = (u16 *)(p + 0x58);
        r -= 0x7f;
        position.z = *z - r;
    }
    AnimatedParticles_EmitBurst(&position, 1, 0, 0x40, 12, 12, 4, 4);
}
