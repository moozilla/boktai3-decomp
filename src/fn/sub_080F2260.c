#include "global.h"
#include "game_state.h"
/* Armor_UpdatePartColliders_080F2260: same as Armor_UpdatePartColliders but the scene comes from the owner at +0x160 (provisional name) */

struct ArmorVector {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
struct ArmorPart {
    u8 pad0[0x1C];
    s16 x, y, z;
    u8 pad22[0x26];
    u8 collider48[6];
    u16 flags4E;
    u8 pad50[0x4C];
    u8 collider9C[0xC4];
    struct {
        u8 pad[0x60];
        struct ArmorScene *scene;
    } *owner;
};
struct ArmorScene {
    u8 pad0[0x32];
    s16 height;
};
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);

void Armor_UpdatePartColliders_080F2260(struct ArmorPart *p)
{
    struct ArmorVector position;
    struct ArmorVector delta;
    delta.y = p->owner->scene->height - (s16)(p->y - 220);
    delta.x = delta.z = (delta.y * 724) >> 10;
    position.x = p->x + delta.x;
    position.y = p->y + delta.y;
    position.z = p->z + delta.z;
    Collider_SetPosition(p->collider9C, &position, 0);
    Collider_SetPosition(p->collider48, &position, 0);
    if (!(p->flags4E & 4))
        Collider_Refresh(p->collider48);
}
