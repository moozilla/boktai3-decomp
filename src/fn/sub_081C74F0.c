// MATCHING_CONCESSION: register-pinning
#include "scene_actor_terrain.h"
// confidence=low: state actor kind 28 dash with decreasing speed and height above terrain.
struct NpcTag {
    u8 pad[2];
    s16 height;
};
struct Npc {
    u8 pad0[1];
    u8 kind;
    u8 pad2[5];
    u8 done, changed, entered;
    u8 padA[4];
    u16 flags;
    u32 timer;
    u8 pad14[4];
    u8 phase;
    u8 pad19[2];
    u8 angle;
    u8 pad1C[4];
    struct NpcTag tag;
    u8 pad24[0x68 - 0x24];
    struct ContactPosition pos, vel;
    u8 pad78[0xD4 - 0x78];
    u8 display[0x208 - 0xD4];
    u32 speed;
};
s32 DisplayAnimation_TestFinished(void *);
static inline s32 trunc12(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void polar(struct ContactPosition *v, u32 angle, s32 speed)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * speed);
    s32 zero = 0;
    v->x = x;
    v->y = zero;
    v->z = trunc12(gUnk_086149C4[angle] * speed);
}
void StateActor_UpdateDashAnimation13(void *ctx, struct Npc *npc, s32 tick)
{
    struct NpcTag *tag = &npc->tag;
    u32 anim;
    asm("" : "+r"(npc));
    if (npc->entered) {
        npc->entered = 0;
        npc->done = 0;
        switch (npc->kind) {
            case 4:
                anim = 0;
                break;
            case 5:
                anim = 0;
                break;
            case 6:
                anim = 0;
                break;
            case 7:
                anim = 0;
                break;
            case 8:
                anim = 0;
                break;
            case 9:
                anim = 0;
                break;
            case 10:
                anim = 0;
                break;
            case 11:
                anim = 0;
                break;
            case 12:
                anim = 0;
                break;
            case 13:
                anim = 0;
                break;
            case 14:
                anim = 0;
                break;
            case 15:
                anim = 0;
                break;
            case 16:
                anim = 0;
                break;
            case 17:
                anim = 0;
                break;
            case 18:
                anim = 0;
                break;
            case 19:
                anim = 0;
                break;
            case 20:
                anim = 0;
                break;
            case 21:
                anim = 0;
                break;
            case 22:
                anim = 0;
                break;
            case 23:
                anim = 0;
                break;
            case 24:
                anim = 0;
                break;
            case 25:
                anim = 0;
                break;
            case 26:
                anim = 0;
                break;
            case 27:
                anim = 13;
                break;
            case 28:
                anim = 0;
                break;
            case 29:
                anim = 0;
                break;
            default:
                anim = 0;
                break;
        }
        Actor_PlayAnimationEntry(npc, anim, 2, 0, 4);
        {
            s32 zero = 0;
            tag->height = 300;
            npc->phase = zero;
        }
    }
    {
        u32 mask = 0x400;
        npc->flags |= mask;
    }
    switch (npc->phase) {
        case 0:
            if (npc->timer > 3) {
                switch (npc->kind) {
                    case 4:
                        anim = 0;
                        break;
                    case 5:
                        anim = 0;
                        break;
                    case 6:
                        anim = 0;
                        break;
                    case 7:
                        anim = 0;
                        break;
                    case 8:
                        anim = 0;
                        break;
                    case 9:
                        anim = 0;
                        break;
                    case 10:
                        anim = 0;
                        break;
                    case 11:
                        anim = 0;
                        break;
                    case 12:
                        anim = 0;
                        break;
                    case 13:
                        anim = 0;
                        break;
                    case 14:
                        anim = 0;
                        break;
                    case 15:
                        anim = 0;
                        break;
                    case 16:
                        anim = 0;
                        break;
                    case 17:
                        anim = 0;
                        break;
                    case 18:
                        anim = 0;
                        break;
                    case 19:
                        anim = 0;
                        break;
                    case 20:
                        anim = 0;
                        break;
                    case 21:
                        anim = 0;
                        break;
                    case 22:
                        anim = 0;
                        break;
                    case 23:
                        anim = 0;
                        break;
                    case 24:
                        anim = 0;
                        break;
                    case 25:
                        anim = 0;
                        break;
                    case 26:
                        anim = 0;
                        break;
                    case 27:
                        anim = 14;
                        break;
                    case 28:
                        anim = 0;
                        break;
                    case 29:
                        anim = 0;
                        break;
                    default:
                        anim = 0;
                        break;
                }
                Actor_PlayAnimationEntry(npc, anim, 0, 0, 4);
                npc->phase++;
                npc->timer = 0;
            }
            break;
        case 1: {
            struct ContactPosition *pos;
            polar(&npc->vel, npc->angle, npc->speed);
            pos = &npc->pos;
            npc->pos.y =
                tag->height + SceneActorTerrain_GetHeight(SceneActorTerrain_GetCellIndex(pos), pos);
            npc->speed = (npc->speed * 3) >> 2;
            if (tick > 3) {
                if (tag->height >= (tick << 3))
                    tag->height -= tick << 3;
                else
                    tag->height = 0;
            }
            if (npc->speed <= 19 && tag->height == 0) {
                Sound_PlaySong(680);
                npc->phase++;
                switch (npc->kind) {
                    case 4:
                        anim = 0;
                        break;
                    case 5:
                        anim = 0;
                        break;
                    case 6:
                        anim = 0;
                        break;
                    case 7:
                        anim = 0;
                        break;
                    case 8:
                        anim = 0;
                        break;
                    case 9:
                        anim = 0;
                        break;
                    case 10:
                        anim = 0;
                        break;
                    case 11:
                        anim = 0;
                        break;
                    case 12:
                        anim = 0;
                        break;
                    case 13:
                        anim = 0;
                        break;
                    case 14:
                        anim = 0;
                        break;
                    case 15:
                        anim = 0;
                        break;
                    case 16:
                        anim = 0;
                        break;
                    case 17:
                        anim = 0;
                        break;
                    case 18:
                        anim = 0;
                        break;
                    case 19:
                        anim = 0;
                        break;
                    case 20:
                        anim = 0;
                        break;
                    case 21:
                        anim = 0;
                        break;
                    case 22:
                        anim = 0;
                        break;
                    case 23:
                        anim = 0;
                        break;
                    case 24:
                        anim = 0;
                        break;
                    case 25:
                        anim = 0;
                        break;
                    case 26:
                        anim = 0;
                        break;
                    case 27:
                        anim = 14;
                        break;
                    case 28:
                        anim = 0;
                        break;
                    case 29:
                        anim = 0;
                        break;
                    default:
                        anim = 0;
                        break;
                }
                Actor_PlayAnimationEntry(npc, anim, 2, 0, 4);
            }
        } break;
        case 2:
            if (DisplayAnimation_TestFinished(npc->display))
                npc->done = 1;
            break;
    }
}
