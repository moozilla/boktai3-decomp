#define gActorMarkerIndexedEffectOffsets gUnk_0824E778
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
extern const u16 gUnk_0824E778[][10][4];
extern u8 gUnk_0824E770[];
s32 Math_Atan2(s32, s32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void ActorMarker_EmitIndexedEffect(struct ActorMarker *p, s32 index)
{
    s32 row = 0;
    struct ActorMarkerEffectPosition position;
    const u16 *offset;
    u32 value;
    const u16 *actorPosition;
    s32 direction;
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
    actorPosition = p->position;
    {
        u32 table = (u32)gUnk_0824E778;
        u32 scaled = index * 8;
        scaled += row * 80;
        offset = (const u16 *)(scaled + table);
    }
    position.x = actorPosition[0] + offset[0];
    position.y = p->position[1] + offset[1];
    position.z = p->position[2] + offset[2];
    direction = Math_Atan2((s16)offset[0], (s16)offset[2]);
    SparkParticle_Spawn(&position, 1, gUnk_0824E770, direction, 3, 8, 2, 2);
}
