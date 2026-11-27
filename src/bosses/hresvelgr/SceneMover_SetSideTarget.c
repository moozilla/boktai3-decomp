#include "global.h"
#include "scene/collision/contact_position.h"
#include "bosses/vanargand/vanargand_parts.h"
struct SceneMover {
    u8 unknown0[0x110];
    struct ContactPosition *anchor;
    u8 unknown114[0x217];
    u8 side;
    u8 unknown32C[0x70];
    struct ContactPosition target;
};
void SceneMover_SetSideTarget(struct SceneMover *p)
{
    struct ContactPosition *anchor = p->anchor;
    struct ContactPosition *other = anchor;
    if (p->side == 0) {
        Vanargand_SetVector(&p->target, 1024, 832, anchor->z);
    } else {
        Vanargand_SetVector(&p->target, other->x, 832, 1024);
    }
}
