#include "global.h"

struct AttachmentVector {
    s16 x, y, z, pad;
};
struct AttachmentParent {
    u8 pad0[0x30];
    struct AttachmentVector position;
    u8 pad38[0x457 - 0x38];
    u8 variant;
};
struct ActorAttachment {
    u8 pad0[0x18];
    struct AttachmentParent *parent;
    struct AttachmentVector offset;
    u8 kind, variant;
    u8 pad26[2];
    u16 value28, value2A;
    u8 pad2C[0x284 - 0x2C];
    u8 mode;
};
void sub_08236FF4(struct ActorAttachment *, struct AttachmentVector *);
void sub_08236DC4(struct ActorAttachment *);

s32 ActorAttachment_Initialize(struct ActorAttachment *p, struct AttachmentParent *parent, u32 kind,
                               struct AttachmentVector *offset, u32 a, u32 b, u32 mode)
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
    p->variant = p->parent->variant;
    p->value28 = a;
    p->value2A = b;
    p->mode = mode;
    sub_08236FF4(p, &p->parent->position);
    if (p->mode == 1)
        sub_08236DC4(p);
    return 0;
}
