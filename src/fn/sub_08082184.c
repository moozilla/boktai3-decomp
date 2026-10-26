// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 98.3%
#include "scene_actor_reaction.h"
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
struct RouteResponseGroup {
    u8 unknown0[0x74];
    struct {
        u8 unknown0[8];
        const u16 *point;
    } route;
    u8 unknown80[0xC];
    u32 savedControl;
    u8 savedState, savedPhase, savedBehavior, savedSubstate;
    u8 state, phase, behavior, unknown97;
    s32 threshold;
    void *control;
    u8 unknownA0[0x1FC - 0xA0];
    void *context;
    struct ContactPosition *destination;
};
static inline void SetTerrainHeight(struct ContactPosition *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    u32 cell, shape;
    register u32 flags asm("r1");
    u32 height;
    register u32 adjustment asm("r0");
    u8 *data;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = (u8 *)((u32)map->cells + offset);
    }
    flags = *data;
    shape = flags >> 4;
    {
        u32 lowHeight = flags & 15;
        height = lowHeight << 8;
    }
    switch (shape) {
        case 1:
            adjustment = *(u8 *)&position->z;
            break;
        case 2:
            adjustment = *(u8 *)&position->x;
            break;
        default:
            goto store_height;
    }
    height -= adjustment;
store_height:
    position->y = height;
}
void SceneActorReaction_TryResumeRoute(struct SceneActorRegistryResetView *input, s32 inputFrame)
{
    register struct SceneActorRegistryResetView *actor asm("r6") = input;
    register s32 frame asm("r5");
    struct RouteResponseGroup *group;
    frame = inputFrame;
    group = (void *)((u8 *)actor + 0x3D4);
    if (SceneActorReaction_TakePrepared(actor)) {
        register u32 selection asm("r4") = 0x2B6;
        asm("" : "+r"(selection));
        {
            register u8 *field asm("r0") = (u8 *)actor + selection;
            actor->direction = (((*field + 3) & 7) << 5) - 64;
        }

        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 0);
        actor->field2B3 = 0;
    }
    if (frame > 60 && actor->field2B3) {
        struct ContactPosition *destination;
        {
            u32 offset = 0x2B0;
            u8 *prepared;
            prepared = (u8 *)actor + offset;
            *prepared = 1;
        }
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
        destination = group->destination;
        {
            register __typeof__(&group->route) route asm("r2") = &group->route;
            destination->x = route->point[0];
            destination->z = route->point[1];
        }
        SetTerrainHeight(destination);
        {
            register void *context asm("r5") = group->context;
            register u32 queryWord asm("r4");
            struct ContactPosition *source = &actor->position;
            u32 clearMask;
            register u32 accepted asm("r5");
            asm("" : "+r"(source));
            queryWord = 0x200;
            asm("" : "+r"(queryWord));
            clearMask = queryWord;
            queryWord = *(u32 *)((u32)group + clearMask);
            {
                u16 *failedFlags;
                u32 updated;
                register u32 resultFlag asm("r0");
                s32 result;
                if (!Nav_IsPositionInMesh(context, source, (void *)queryWord)) {
                    failedFlags = &actor->flags2D4;
                    updated = *failedFlags;
                    updated &= ~clearMask;
                    goto query_failed;
                }
                result = Terrain_ApplyBlocked(context, source, (void *)queryWord);
                if (result) {
                    if (result != 2)
                        Memory_Clear(&actor->motion, 8);
                    resultFlag = 1;
                    goto query_done;
                }
                {
                    register u32 offset asm("r4") = 0x2D4;
                    asm("" : "+r"(offset));
                    failedFlags = (void *)((u8 *)actor + offset);
                }
                updated = *failedFlags & ~clearMask;
            query_failed:
                *failedFlags = updated;
                resultFlag = 0;
            query_done:
                accepted = resultFlag;
            }
            if (!accepted) {
                void *control = SceneActorControl_OnIdle;
                u32 zero = accepted;
                u32 one;
                u32 zeroByte;
                register u32 preparedOffset asm("r2") = 0x2B2;
                u32 controlOffset;
                u8 *field;
                field = (u8 *)actor + preparedOffset;
                one = 1;
                *field = one;
                controlOffset = 0x2B0;
                field = (u8 *)actor + controlOffset;
                *field = zero;
                preparedOffset += 3;
                field = (u8 *)actor + preparedOffset;
                *field = zero;
                controlOffset -= 0x94;
                {
                    void **output = (void **)((u8 *)actor + controlOffset);
                    *output = control;
                }
                {
                    register u32 offset asm("r1") = 0x2C8;
                    register u16 *output asm("r0");
                    asm("" : "+r"(offset));
                    output = (u16 *)((u8 *)actor + offset);
                    zeroByte = 0;
                    *output = zero;
                }
                controlOffset = 0x114;
                asm("" : "+r"(controlOffset));
                {
                    u32 *flags = (u32 *)((u8 *)actor + controlOffset);
                    u32 mask = ~1;
                    *flags &= mask;
                }
                {
                    u32 offset = 0x104;
                    u32 *output;
                    output = (u32 *)((u8 *)actor + offset);
                    *output = zero;
                }
                {
                    register u32 state asm("r1") = 3;
                    register u32 stateOffset asm("r2") = 0x2A8;
                    u32 phaseOffset;
                    field = (u8 *)actor + stateOffset;
                    *field = state;
                    phaseOffset = 0x2A9;
                    field = (u8 *)actor + phaseOffset;
                    *field = zeroByte;
                    {
                        u32 offset = 0x2AA;
                        field = (u8 *)actor + offset;
                    }
                    *field = zeroByte;
                    stateOffset += 4;
                    asm("" : : "r"(stateOffset));
                    {
                        register u32 *counter asm("r0") = (u32 *)((u8 *)actor + stateOffset);
                        *counter = zero;
                    }
                    phaseOffset += 8;
                    field = (u8 *)actor + phaseOffset;
                    *field = one;
                }
            } else {
                u32 zeroByte;
                register u32 zero asm("r3");
                register u32 one asm("r2");
                u32 phaseOffset;
                u32 stateOffset;
                {
                    struct ContactPosition *position = group->destination;
                    zeroByte = 0;
                    asm("" : "+r"(zeroByte));
                    zero = 0;
                    one = 1;
                    position->pad = 1;
                }
                {
                    u32 state = 3;
                    register u8 *field asm("r0");
                    stateOffset = 0x2A8;
                    field = (u8 *)actor + stateOffset;
                    *field = state;
                    phaseOffset = 0x2A9;
                    field = (u8 *)actor + phaseOffset;
                    *field = one;
                    {
                        register u32 offset asm("r1") = 0x2AA;
                        asm("" : "+r"(offset));
                        field = (u8 *)actor + offset;
                    }
                    *field = zeroByte;
                    stateOffset += 4;
                    *(u32 *)((u8 *)actor + stateOffset) = zero;
                    phaseOffset += 8;
                    *((u8 *)actor + phaseOffset) = one;
                }
                {
                    void *control = SceneActorReaction_RestoreAfterDelay;
                    u32 kind = 14;
                    phaseOffset += 1;
                    *((u8 *)actor + phaseOffset) = one;
                    one = 0x2B0;
                    *((u8 *)actor + one) = zeroByte;
                    phaseOffset += 3;
                    *((u8 *)actor + phaseOffset) = kind;
                    actor->target = control;
                    one += 0x18;
                    {
                        register u16 *field asm("r0") = (u16 *)((u8 *)actor + one);
                        *field = zero;
                    }
                }
                {
                    register u32 offset asm("r4") = 0x114;
                    register u32 *flags asm("r2");
                    u32 mask;
                    asm("" : "+r"(offset));
                    flags = (void *)((u8 *)actor + offset);
                    mask = ~1;
                    *flags &= mask;
                }
                phaseOffset = 0x104;
                asm("" : : "r"(phaseOffset));
                *(u32 *)((u8 *)actor + phaseOffset) = zero;
                {
                    u32 mask = 0x10;
                    register u32 offset asm("r0") = 0x2CC;
                    register u32 *flags asm("r1");
                    asm("" : "+r"(offset));
                    flags = (void *)((u8 *)actor + offset);
                    *flags |= mask;
                }
                actor->field2C2 = 64;
                {
                    struct ResponsePacket {
                        u8 unknown0[8];
                        u8 state, phase, behavior, unknownB;
                        s32 threshold;
                        void *control;
                    };
                    struct ResponsePacket *packet = (void *)((u8 *)group + 0x8C);
                    register u32 sourceOffset asm("r4") = 0x228;
                    register void **controlSource asm("r0");
                    void *control;
                    u32 behavior;
                    u32 threshold;
                    asm("" : "+r"(sourceOffset));
                    controlSource = (void **)((u8 *)actor + sourceOffset);
                    control = *controlSource;
                    behavior = 7;
                    threshold = 60;
                    packet->threshold = threshold;
                    packet->control = control;
                    packet->state = zeroByte;
                    packet->phase = zeroByte;
                    packet->behavior = behavior;
                }
            }
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", SceneActorReaction_TryResumeRoute);
#endif
