#include "global.h"

#define SceneCombatActor_SpawnBouncingEffect sub_08180E68

struct ParticleVector {
    s32 x : 16, y : 16, z : 16;
    u32 : 16;
};
struct BouncingParticleSprite {
    u32 flags;
    u8 unknown4[2], phase;
    u8 unknown7[9];
    u16 frame;
    u8 unknown12[0xa];
    struct ParticleVector position;
    u8 unknown24[8];
};
struct BouncingSceneParticle {
    void (*update)(void *), (*destroy)(void *);
    struct BouncingParticleSprite sprite;
    struct ParticleVector velocity;
    s16 floor;
    u8 bounced;
};
struct SceneCombatActor {
    u8 unknown0[0xd4];
    u16 x;     /* 0xd4 */
    u16 floor; /* 0xd6, also the actor's own y */
    u16 z;     /* 0xd8 */
    u8 unknownda[0x318 - 0xda];
    u8 particleAnimation[0x1c];
    u8 unknown334[0x350 - 0x334];
    struct BouncingSceneParticle particles[2]; /* 0x350, 0x390 */
    u8 unknown3d0[0xb38 - 0x3d0];
    u8 kind, heading;
};

void ActorOverlay_InitSprite(struct SceneCombatActor *p);

void sub_08180E68(struct SceneCombatActor *p)
{
    ActorOverlay_InitSprite(p);
    p->particles[0].velocity.z = 0;
    p->particles[0].velocity.y = 0;
    p->particles[0].velocity.x = 0;
    p->particles[0].velocity.y = 30;
    switch (((p->heading + 0x60) >> 6) & 3) {
        case 0:
            p->particles[0].sprite.position.x = p->x + 0x64;
            p->particles[0].sprite.position.y = 0x28a + p->floor;
            p->particles[0].sprite.position.z = p->z + 0x64;
            p->particles[0].sprite.phase = 0xf;
            p->particles[0].velocity.z = 0xf;
            break;
        case 1:
            p->particles[0].sprite.position.x = p->x;
            p->particles[0].sprite.position.y = 0x28a + p->floor;
            p->particles[0].sprite.position.z = p->z + 0xc8;
            p->particles[0].sprite.phase = 0xf;
            p->particles[0].velocity.x = -0xf;
            break;
        case 2:
            p->particles[0].sprite.position.x = p->x + 0x96;
            p->particles[0].sprite.position.y = 0x28a + p->floor;
            p->particles[0].sprite.position.z = p->z;
            p->particles[0].sprite.phase = 0xf5;
            p->particles[0].velocity.z = -0xf;
            break;
        case 3:
            p->particles[0].sprite.position.x = p->x + 0x64;
            p->particles[0].sprite.position.y = 0x28a + p->floor;
            p->particles[0].sprite.position.z = p->z + 0x64;
            p->particles[0].sprite.phase = 0xf5;
            p->particles[0].velocity.x = 0xf;
            break;
    }
}
