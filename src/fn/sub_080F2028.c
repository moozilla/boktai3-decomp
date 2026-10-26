#include "global.h"

struct Sub080F2028Position {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
struct Sub080F2028Scene {
    u8 unknown0[0x32];
    u16 height;
};
struct Sub080F2028Actor {
    u8 unknown0[0x60];
    struct Sub080F2028Scene *scene;
    u8 unknown64[0xB84 - 0x64];
    u16 x;
    u16 unknownB86;
    u16 z;
    u8 unknownB8A[0xC70 - 0xB8A];
    u8 primary[0xD80 - 0xC70];
    s32 frame;
};
extern const s32 gUnk_0860641C[];
s32 Mod(s32, s32);
void Collider_SetPosition(void *, void *, u32);
// scope shape 2 (differences once low registers are set aside)
// confidence=low: places the primary collider at the actor position offset by a table value.
void sub_080F2028(struct Sub080F2028Actor *actor)
{
    struct Sub080F2028Position position;
    u16 *xp;
    const s32 *radius;
    s32 slot;
    const s32 *table;

    slot = Mod(actor->frame, 3);
    xp = &actor->x;
    table = gUnk_0860641C;
    radius = table + slot;
    position.x = *xp + *radius;
    position.y = actor->scene->height + 220;
    position.z = actor->z + *radius;
    Collider_SetPosition(actor->primary, &position, 0);
}
