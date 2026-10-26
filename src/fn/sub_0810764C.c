#include "global.h"
#include "scene_vec.h"
#include "engine.h"
#include "ram.h"
struct TileTrigger {
    u32 flags;
    u8 unknown4[0x70];
    u8 animation[0x10];
    struct SceneVec position;
    u32 key;
    u8 unknown90[0x28];
    u16 sequence, delay, unknownBC;
    s16 state;
    u16 triggerFlags;
    u8 forced, frame;
};
struct TileTriggerManager {
    u8 unknown0[0x18];
    void *animationData;
    u8 unknown1C[0x1C];
    struct TileTrigger *tiles[12];
    u32 used;
};
void TileTrigger_EmitParticles(struct TileTrigger *);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
void TileTrigger_TickDelay(struct TileTrigger *p)
{
    struct TileTriggerManager *manager = gTileTriggerManager;
    if (p->frame >= p->delay) {
        TileTrigger_EmitParticles(p);
        Anim_SetSequence(p->animation, manager->animationData, 0, (u8)p->sequence, 0);
        p->state = 4;
        p->frame = 0;
        switch (p->sequence) {
            case 0:
            case 1:
                if (gEngineFlags & 0x800)
                    Sound_PlaySong(0x26D);
                else
                    Sound_PlaySong(0x1AB);
                break;
        }
    } else
        p->frame++;
}
