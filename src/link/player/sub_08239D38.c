#include "global.h"
#include "engine/math/monster_particle_random.h"
#include "player/player.h"

void EffectGroup_FadeFirstActive(void *);
void Player_PlayLocalSong(s32, u32);
s32 EffectGroup_SpawnPart(void *, s32, struct SceneVec *, struct SceneVec *, struct SceneVec *, s32,
                          s32);

// confidence=low: link player effect tick: fades the group or spawns a randomized particle.
void sub_08239D38(struct Player *p)
{
    struct SceneVec position;
    struct SceneVec velocity;
    struct SceneVec spread;
    u32 kind = p->movementModifierKind;
    const struct SceneVec *source;

    if (kind == 1) {
        EffectGroup_FadeFirstActive(&p->effectGroup);
        return;
    }
    if (kind == 2)
        Player_PlayLocalSong(p->slot, 0x134);
    else if (kind == 3)
        Player_PlayLocalSong(p->slot, 0x134);
    source = p->visualPosition;
    position.x = source->x;
    position.y = source->y + 0x80;
    position.z = source->z;
    velocity.x = (ParticleRandom_ReadDirectSample(gRandomTable) & 15) - 7;
    velocity.y = (ParticleRandom_ReadDirectSample(gRandomTable) & 31) - 16;
    velocity.z = (ParticleRandom_ReadDirectSample(gRandomTable) & 15) - 7;
    spread.x = 5;
    spread.y = 10;
    spread.z = 5;
    EffectGroup_SpawnPart(&p->effectGroup, 3, &position, &velocity, &spread, 40, 40);
}
