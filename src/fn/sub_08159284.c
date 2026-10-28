#define gDirectionSineTable gUnk_086149C4
#include "global.h"
#include "player.h"
extern const s16 gUnk_086149C4[];
static inline void SetPosition(struct SceneVec *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
    v->pad = 0;
}
static inline s32 Scale(s32 value)
{
    value = 180 * value;
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void PlayerContact_GetOffsetPosition(s32 slot, struct SceneVec *out)
{
    struct SceneVec pos;
    struct Player *p = gPlayers[slot];
    s32 angle, x, z;
    if (!p->statModifierContext.statBOverrideEnabled)
        SetPosition(&pos, p->position.x, p->position.y + 200, p->position.z);
    else
        SetPosition(&pos, p->position.x, p->position.y + 230, p->position.z);
    angle = ((p->facing + 5) & 7) * 32;
    x = gDirectionSineTable[(angle + 64) & 255];
    z = gDirectionSineTable[angle & 255];
    if (p->side) {
        pos.x += Scale(x);
        pos.z += Scale(z);
    } else {
        pos.x += Scale(x);
        pos.z += Scale(z);
    }
    *out = pos;
}
