#include "global.h"
// confidence=low: trigonometric offsets position the main and four paired part colliders.
#define Armor_UpdateOrbitingColliders sub_080F20AC
struct ArmorPosition {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
struct ArmorScene {
    u8 unknown0[0x32];
    u16 height;
};
struct ArmorSourcePart {
    u8 unknown0[0x1C];
    u16 x, y, z;
    u8 unknown22[0xA];
};
struct ArmorCollider {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[0x4C];
};
struct ArmorOrbitActor {
    u8 unknown0[0x60];
    struct ArmorScene *scene;
    u8 unknown64[0x44];
    struct ArmorSourcePart parts[4];
    u8 unknown158[0x70];
    struct ArmorCollider inner[4], outer[4];
    u8 unknown468[0x4A];
    u8 angle;
    u8 unknown4B3[0x6BB];
    u8 heading;
    u8 unknownB6F[0x15];
    u16 x;
    u16 unknownB86;
    u16 z;
    u8 unknownB8A[0x92];
    struct ArmorCollider primary, secondary;
    u8 unknownCC4[0xBC];
    s32 frame;
};
extern const s16 gUnk_086149C4[];
extern const s32 gUnk_08606428[];
s32 Mod(s32, s32);
s32 Div(s32, s32);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
static inline void Armor_CopyRadius(const s32 **out, const s32 *radius)
{
    *out = radius;
}
void Armor_UpdateOrbitingColliders(struct ArmorOrbitActor *actor)
{
    struct ArmorPosition position;
    const s32 *radius;
    s32 i;
    s32 slot;
    s32 cosine;
    const s32 *table;
    actor->angle = 28 - actor->heading;
    slot = Mod(actor->frame, 3);
    cosine = gUnk_086149C4[(actor->angle + 64) & 255];
    table = gUnk_08606428;
    radius = table + slot;
    position.x = ((cosine * *radius) >> 12) + actor->x;
    position.y = actor->scene->height + 220;
    position.z = ((*radius * gUnk_086149C4[actor->angle]) >> 12) + actor->z;
    Collider_SetPosition(&actor->primary, &position, 0);
    Collider_SetPosition(&actor->secondary, &position, 0);
    if (!(actor->primary.flags & 4))
        Collider_Refresh(&actor->primary);
    for (i = 0; i <= 3; i++) {
        const s32 *loopRadius;
        u8 *angle = &actor->angle;
        const s16 *trig = gUnk_086149C4;
        u16 *point;
        Armor_CopyRadius(&loopRadius, radius);
        point = (void *)&position;
        point[0] = actor->parts[i].x + Div(trig[(*angle + 64) & 255] * *loopRadius, 4096);
        point[1] = actor->scene->height + 220;
        point[2] = actor->parts[i].z + Div(trig[*angle] * *loopRadius, 4096);
        Collider_SetPosition(&actor->outer[i], &position, 0);
        if (!(actor->inner[i].flags & 4)) {
            Collider_Refresh(&actor->inner[i]);
            Collider_SetPosition(&actor->inner[i], &position, 0);
        }
    }
}
