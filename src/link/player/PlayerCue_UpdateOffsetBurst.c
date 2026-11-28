#include "scene/collision/contact_swarm.h"
#include "engine/math/scene_vec.h"
#include "player/cutscene/player_cue_sequence.h"
#include "bosses/vanargand/vanargand_projectiles.h"
#include "player/player.h"

void Player_SetTetherMode(struct Player *, s32, s32);
void Player_EmitPositionEffect(struct Player *, const void *, s32, u32, const void *, u32, u32);
extern const s16 gUnk_086149C4[];
// confidence=low: a cue that emits a displaced positional effect, then advances its pose
void PlayerCue_UpdateOffsetBurst(struct Player *p)
{
    if (p->tetherMode != 4) {
        struct ContactPosition pt;
        u32 ang;
        pt = *(struct ContactPosition *)&p->position;
        ang = ((p->facing + 5) & 7) << 5;
        pt.x += VanargandProjectile_Scale(gUnk_086149C4[(ang + 0x40) & 0xff] * 64);
        pt.y += 0x40;
        pt.z += VanargandProjectile_Scale(gUnk_086149C4[ang] * 64);
        Player_SetTetherMode(p, 4, 0);
        Player_EmitPositionEffect(p, &p->position, 1, 1, &pt, 0, 0x3A1);
    }
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 0, p->heading, p->side);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->secondaryDrawFlags,
                           &p->cueCollider.sequence);
    p->tetherFrame++;
    if (p->tetherFrame > 0xf)
        Player_SetTetherMode(p, 0, 0);
}
