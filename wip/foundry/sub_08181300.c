#define ActorMarker_EmitOffsetEffects sub_08181300
#define gActorMarkerBurstOffsets gUnk_0824E6F8
#include "global.h"
struct ActorMarker {
    u8 pad0[0xd4];
    u16 x, y, z;
    u8 padDA[0xb39 - 0xda];
    u8 direction;
};
struct ActorMarkerEffectPosition { u32 x : 16, y : 16, z : 16, pad : 16; };
extern const u16 gUnk_0824E6F8[][4];
extern u8 gUnk_0824E768[];
void sub_0806A160(void *, s32, void *, s32, s32, s32, s32, s32);
void sub_08181300(struct ActorMarker *p)
{
    struct ActorMarkerEffectPosition position;
    s32 row = 0;
    s32 i;
    switch (((p->direction + 0x60) >> 6) & 3) {
    case 1: case 3: row = 0; break;
    case 0: case 2: row = 1; break;
    }
    for (i = 0; i < 7; i++) {
        const u16 *offset = gUnk_0824E6F8[row * 7 + i];
        u32 value;
        u32 actorValue;
        value = offset[0];
        actorValue = p->x;
        value += actorValue;
        position.x = value;
        value = offset[1];
        actorValue = p->y;
        value += actorValue;
        position.y = value;
        value = offset[2];
        actorValue = p->z;
        value += actorValue;
        position.z = value;
        sub_0806A160(&position, 1, gUnk_0824E768, p->direction + 0x80, 3, 8, 2, 2);
    }
}
