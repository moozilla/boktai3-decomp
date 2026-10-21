#include "global.h"
struct ActorMarker {
    u8 pad0[0xd4];
    u16 position[3];
    u8 padDA[0xb39 - 0xda];
    u8 direction;
};
struct ActorMarkerEffectPosition {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
extern const u16 gUnk_0824E6F8[][4];
extern u8 gUnk_0824E768[];
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void ActorMarker_EmitRingEffects(struct ActorMarker *p)
{
    s32 row = 0;
    struct ActorMarkerEffectPosition position;
    s32 i;
    const u16 *actorPosition;
    const u16(*tbl)[4];
    switch (((p->direction + 0x60) >> 6) & 3) {
        case 1:
        case 3:
            row = 0;
            break;
        case 0:
        case 2:
            row = 1;
            break;
    }
    i = 0;
    tbl = gUnk_0824E6F8;
    actorPosition = p->position;
    if (i <= 6) {
        do {
            const u16 *offset = (const u16 *)(((row * 8 - row + i) * 8) + (u32)tbl);
            position.x = actorPosition[0] + offset[0];
            position.y = p->position[1] + offset[1];
            position.z = p->position[2] + offset[2];
            SparkParticle_Spawn(&position, 1, gUnk_0824E768, p->direction + 0x80, 3, 8, 2, 2);
            i++;
        } while (i <= 6);
    }
}
