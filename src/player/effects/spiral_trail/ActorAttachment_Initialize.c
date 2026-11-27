#include "player/effects/spiral_trail.h"
#include "player/player.h"


s32 ActorAttachment_Initialize(struct SpiralTrailTask *p, struct Player *parent, u32 kind,
                               struct ContactPosition *offset, u32 a, u32 b, u32 mode)
{
    p->kind = kind;
    p->parent = parent;
    if (!offset) {
        p->offset.x = 0;
        p->offset.y = 0;
        p->offset.z = 0;
    } else {
        p->offset = *offset;
    }
    p->parentMode = p->parent->tetherMode;
    p->emissionLimit = a;
    p->fadeLimit = b;
    p->mode = mode;
    AttachmentSprite_Setup(p, (const struct ContactPosition *)&p->parent->position);
    if (p->mode == 1)
        sub_08236DC4(p);
    return 0;
}
