#include "scene_pool.h"
void ScenePool_UpdateSpeedPulse(struct ScenePoolActor *, struct ScenePoolEntry *,
                                const struct ScenePoolSpeedBounds *);
extern const struct ScenePoolSpeedBounds gUnk_086058A0[];
void Player_GetWorldPosition(s32, struct ScenePoolVec32 *);
s32 Player_GetHorizontalVelocity(s32);
s32 Terrain_GetHeightAtFixedPosition(struct ScenePoolVec32 *);
s32 SceneArc_Create(struct ScenePoolVec32 *, struct ScenePoolVec32 *, s32, s32, s32);
s32 ScenePool_StateLaunchShortArc(void *manager, struct ScenePoolActor *actor,
                                  struct ScenePoolEntry *entry)
{
    struct ScenePoolVec32 from, to, path;
    u32 fire;
    if (entry->stateEntered)
        entry->stateEntered = 0;
    ScenePool_UpdateSpeedPulse(actor, entry, gUnk_086058A0);
    if (entry->stateFrame <= 19) {
        s32 lateral = entry->unknown5c;
        s32 magnitude = lateral;
        if (lateral < 0)
            magnitude = -magnitude;
        if (magnitude > 0xa000) {
            if (lateral > 0)
                actor->controlFlags |= 0x80;
            else
                actor->controlFlags |= 0x40;
        }
    }
    fire = 0;
    if (entry->kind == 3) {
        if (entry->stateFrame == 10 || entry->stateFrame == 40)
            fire = 1;
    } else {
        switch (entry->mode) {
            case 0:
                if (entry->stateFrame == 20)
                    fire = 1;
                break;
            case 1:
                if (entry->stateFrame == 10 || entry->stateFrame == 40)
                    fire = 1;
                break;
        }
    }
    if (fire) {
        s32 speed;
        Player_GetWorldPosition(entry->motionPhase, &path);
        from = entry->position;
        from.y += actor->restrictionMetric;
        to = entry->position;
        speed = Player_GetHorizontalVelocity(entry->motionPhase);
        if (speed <= 0xfff)
            speed = 0x1000;
        to.x += entry->direction + speed * 48;
        to.y = Terrain_GetHeightAtFixedPosition(&to);
        to.z = path.z;
        SceneArc_Create(&from, &to, 0x20, 48, 0x20);
    }
    if (entry->stateFrame > 59) {
        s32 random;
        gRandomIndex = (gRandomIndex + 1) & 0x3ff;
        random = *(const u8 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1));
        switch (entry->kind) {
            case 0:
                entry->state = 0;
                entry->stateFrame = 0;
                entry->stateEntered = 1;
                Memory_Clear(entry->history, 0x100);
                break;
            case 1:
                entry->state = 0;
                entry->stateFrame = 0;
                entry->stateEntered = 1;
                Memory_Clear(entry->history, 0x100);
                break;
            case 2:
                if (random <= 63) {
                    entry->state = 2;
                    entry->stateFrame = 0;
                    entry->stateEntered = 1;
                    Memory_Clear(entry->history, 0x100);
                } else {
                    entry->state = 1;
                    entry->stateFrame = 0;
                    entry->stateEntered = 1;
                    Memory_Clear(entry->history, 0x100);
                }
                break;
            case 3:
                if (random <= 63) {
                    entry->state = 2;
                    entry->stateFrame = 0;
                    entry->stateEntered = 1;
                    Memory_Clear(entry->history, 0x100);
                } else {
                    entry->state = 1;
                    entry->stateFrame = 0;
                    entry->stateEntered = 1;
                    Memory_Clear(entry->history, 0x100);
                }
                break;
        }
        return 0;
    }
    entry->stateFrame++;
    return 0;
}
