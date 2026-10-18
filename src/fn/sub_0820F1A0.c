#define OrangeSpine_TryFireAtPlayer sub_0820F1A0
#define OrangeSpine_FireProjectile sub_0820F078
// Orange spine family and descriptive names are tentative (low confidence).
#include "global.h"
struct OrangeSpinePosition { s16 x, y, z, pad; };
struct OrangeSpine {
    u8 unk0[0x18];
    u32 state;
    struct OrangeSpinePosition position;
    u8 unk24[0x108-0x24];
    void (*update)(void *);
    u8 animation[0x20];
    u16 timer;
    u8 unk12e[4];
    s8 facing;
    u8 unk133;
    u16 recoveryDelay;
    u8 unk136[0x170-0x136];
    void *animationData;
};
struct OrangeSpinePosition *Player_GetEffectivePosition(s32);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void sub_0820F078(struct OrangeSpine *);
void OrangeSpine_UpdateRecoveryDelay(void *);
u32 sub_0820F1A0(struct OrangeSpine *p) {
    struct OrangeSpinePosition position;
    struct OrangeSpinePosition *pos;
    struct OrangeSpinePosition *player = Player_GetEffectivePosition(0);
    pos = &position;
    position = *player;
    if (!p->facing) {
        if (p->position.z - 128 < pos->z && p->position.z + 128 > pos->z && p->position.x < position.x) {
            OrangeSpine_FireProjectile(p);
            Anim_SetSequence(p->animation, p->animationData, 15, (u8)p->facing, 0);
            p->timer = p->recoveryDelay;
            p->state = 0;
            p->update = OrangeSpine_UpdateRecoveryDelay;
        }
    } else {
        if (p->position.x - 128 < position.x && p->position.x + 128 > position.x && p->position.z < pos->z) {
            OrangeSpine_FireProjectile(p);
            Anim_SetSequence(p->animation, p->animationData, 15, (u8)p->facing, 0);
            p->timer = p->recoveryDelay;
            p->state = 0;
            p->update = OrangeSpine_UpdateRecoveryDelay;
        }
    }
}
