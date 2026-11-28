
#include "global.h"
#include "monsters/screamer/screamer_playback.h"
#include "scene/collision/packed_contact_vector.h"
struct ColliderFlags {
    u8 pad[6];
    u32 flags : 16;
};
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetBodyProperties(void *, u32, u32, u32);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void Screamer_OnColliderContact(void);
void Screamer_ApplyHitReaction(void);
void Screamer_InitColliders(struct ScreamerState *p)
{
    struct PackedContactVector size, offset;
    void *collider = (void *)&p->attackCollider;
    if (!p->f1D8) {
        size.x = 160;
        size.y = 128;
        size.z = 128;
        offset.x = 128;
        offset.y = 0;
        offset.z = 0;
    } else {
        size.x = 128;
        size.y = 128;
        size.z = 160;
        offset.x = 0;
        offset.y = 0;
        offset.z = 128;
    }
    Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetCallback(collider, Screamer_OnColliderContact, p);
    Collider_SetAttackProperties(collider, p->f1C4, p->f1C6, 0, 0, p->f1C8);
    collider = (void *)&p->bodyCollider;
    ((struct ContactPosition *)&size)->x = 32;
    ((struct ContactPosition *)&size)->y = 128;
    ((struct ContactPosition *)&size)->z = 32;
    ((struct ContactPosition *)&offset)->x = 0;
    ((struct ContactPosition *)&offset)->y = 0;
    ((struct ContactPosition *)&offset)->z = 0;
    Collider_Init(collider, 0, 0x5009, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &p->sprite.position, 0);
    Collider_SetCallback(collider, Screamer_ApplyHitReaction, p);
    Collider_SetBodyProperties(collider, 0, (u16)p->f1E8, (u16)p->f1EA);
    Collider_Refresh(collider);
    ((struct ColliderFlags *)collider)->flags |= 4;
    collider = (u8 *)&p->frontCollider;
    if (!p->f1D8) {
        ((struct ContactPosition *)&size)->x = 150;
        ((struct ContactPosition *)&size)->y = 120;
        ((struct ContactPosition *)&size)->z = 120;
        ((struct ContactPosition *)&offset)->x = 128;
        ((struct ContactPosition *)&offset)->y = 0;
        ((struct ContactPosition *)&offset)->z = 0;
    } else {
        ((struct ContactPosition *)&size)->x = 120;
        ((struct ContactPosition *)&size)->y = 120;
        ((struct ContactPosition *)&size)->z = 150;
        ((struct ContactPosition *)&offset)->x = 0;
        ((struct ContactPosition *)&offset)->y = 0;
        ((struct ContactPosition *)&offset)->z = 128;
    }
    Collider_Init(collider, 0, 0x4009, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &p->sprite.position, 0);
    Collider_SetCallback(collider, Screamer_ApplyHitReaction, p);
    Collider_SetBodyProperties(collider, 0, (u16)p->f1E8, (u16)p->f1EA);
    Collider_Refresh(collider);
    ((struct ColliderFlags *)collider)->flags |= 4;
}
