#include "global.h"

#include "scene_actor_reset.h"

struct AxeSparkContext {
    u8 unk_00[0xED2];
    u8 sparkStep;
};

s32 Mod(s32, s32);
void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);
extern const s16 gUnk_086149C4[];

static inline s32 Scale(s32 x)
{
    s32 result;
    if (x >= 0)
        result = x >> 12;
    else
        result = -(-x >> 12);
    return result;
}

inline static void CpuSet_2(int control, struct ContactPosition *dest, u32 *source)
{
    CpuSet(source, dest, control);
}

void sub_080FC1F8(struct SceneActorRegistryResetView *actor, struct ContactPosition *origin,
                  s32 angle, s32 frame)
{
    struct {
        u32 zero;
        u32 values[0];
    } buf;
    struct ContactPackedVector range;
    struct ContactPosition offset;
    union {
        struct ContactPosition p;
        struct ContactPackedVector v;
    } position;
    struct AxeSparkContext *context = actor->context;
    if (Mod(frame, 2) == 0) {
        s32 x, z, x2;
        range.x = 60;
        range.y = 60;
        range.z = 40;
        position.p = *origin;
        {
            struct ContactPosition *op = &offset;
            buf.zero = 0;
            CpuSet_2(0x05000002, op, &buf.zero);
        }
        switch (context->sparkStep) {
            case 0:
                offset.x = -120;
                offset.z = -120;
                context->sparkStep++;
                break;
            case 1:
                offset.x = -75;
                offset.z = -75;
                context->sparkStep++;
                break;
            case 2:
                offset.x = -30;
                offset.z = -30;
                context->sparkStep++;
                break;
            case 3:
                offset.x = 15;
                offset.z = 15;
                context->sparkStep++;
                break;
            case 4:
                offset.x = 60;
                offset.z = 60;
                context->sparkStep = 0;
                break;
        }
        x = Scale(offset.x * gUnk_086149C4[(angle + 64) & 255]);
        z = Scale(offset.z * gUnk_086149C4[angle & 255]);
        position.v.x = x + z + position.p.x;
        x2 = Scale(-(offset.x * gUnk_086149C4[angle & 255]));
        z = offset.z * gUnk_086149C4[(angle + 64) & 255];
        if (z >= 0)
            z >>= 12;
        else
            z = -(-z >> 12);
        position.v.z = x2 + z + position.p.z;
        SparkParticle_Spawn(&position, 2, &range, 0, 0, 22, 2, 1);
    }
}
