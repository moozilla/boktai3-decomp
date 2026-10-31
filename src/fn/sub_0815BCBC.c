#include "player.h"

extern const s16 gUnk_086149C4[];

void Player_EnterKindTwo(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Sound_PlaySong(s32);
void MovingSprite_Create_082375FC(struct SceneVec *, s32, s32, s32, s32, s32, s32);
void Player_UpdateSixPhaseAction(struct Player *);

static inline int trunc12(int x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}

void PlayerCue_Shoot(struct Player *p, s32 a, u32 c)
{
    struct SceneVec v;
    u32 angle;
    s32 id;

    v = p->position;
    angle = ((p->facing + 5) & 7) << 5;
    v.x += trunc12(gUnk_086149C4[(angle + 0x40) & 0xFF] * 150);
    v.y += 0xE6;
    v.z += trunc12(gUnk_086149C4[angle] * 150);
    if (p->actionMode == 6) {
        Sound_PlaySong(0x203);
        id = 0x51;
    } else {
        Sound_PlaySong(0xCD);
        id = 0x4B;
    }
    MovingSprite_Create_082375FC(&v, id, 0, p->facing, 0x20, 0, a);
    p->cueParameter = c;
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 3, 2);
    p->triggerAnimationCallback = Player_UpdateSixPhaseAction;
}
