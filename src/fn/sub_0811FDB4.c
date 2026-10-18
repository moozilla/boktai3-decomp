#include "global.h"
#define ActorCollision_InitBounds sub_0811FDB4
#define ActorMotion_SetKnockback  sub_0811FCB4
struct CollisionPackedVector {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct CollisionPosition {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
struct CollisionActor {
    u8 unknown0[0x52C];
    u8 body[0x40];
    u8 contact[0xAC];
    u16 x, y;
};
void sub_0811B278(void *, void *, void *, void *, s32, s32, s32, s32, s32, void (*)(void), void *);
void sub_0811B2FC(void *);
void sub_0811FAB8(void);
void sub_0811FCB4(void);
void sub_0811FDB4(struct CollisionActor *actor)
{
    struct CollisionPosition position;
    struct CollisionPackedVector size, offset;
    s32 y;
    position.x = actor->x;
    position.y = actor->y - 96;
    position.z = actor->y - 96;
    size.x = 24;
    size.y = 32;
    size.z = 2;
    offset.x = 0;
    offset.y = -(s16)size.y;
    offset.z = -size.z;
    sub_0811B278(actor->contact, &position, &size, &offset, 20, 5, 1, 0, 12, sub_0811FAB8, actor);
    position.x = actor->x;
    y = actor->y - 96;
    ((s16 *)&position)[1] = y;
    ((s16 *)&position)[2] = y;
    *(s16 *)&size = 24;
    ((s16 *)&size)[1] = 32;
    ((s16 *)&size)[2] = 2;
    *(s16 *)&offset = 0;
    ((s16 *)&offset)[1] = -((u16 *)&size)[1];
    ((s16 *)&offset)[2] = -((u16 *)&size)[2];
    sub_0811B278(actor->body, &position, &size, &offset, 12, 5, 1, 0, 12, sub_0811FCB4, actor);
    sub_0811B2FC(actor->contact);
}
