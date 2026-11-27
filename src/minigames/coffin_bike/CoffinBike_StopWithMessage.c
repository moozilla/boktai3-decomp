#include "scene/actor/scene_event_types.h"
#include "link/wireless/scene_pool.h"
#include "engine/ram.h"
s32 SceneMessages_Send(struct SceneEventCommand *);
void Sound_PlayTrackedSong(int);
void CoffinBike_UpdateStopping(void);
static inline void clearDrawFlags(struct ScenePoolPlayerDraw *d, s16 mask)
{
    d->flags = mask & d->flags;
}
void CoffinBike_StopWithMessage(struct ScenePoolPlayer *glove)
{
    struct SceneEventCommand *packet = (struct SceneEventCommand *)&gMessagePacket;
    u32 one;
    packet->trigger = 0;
    packet->unknown4 = 0;
    packet->kind = 0x41;
    packet->argumentCount = 0;
    packet->receiverKind = 4;
    packet->id = 0x7c56;
    SceneMessages_Send(packet);
    {
        struct ScenePoolPlayerDraw *draw = &glove->gloveDraw;
        one = 1;
        clearDrawFlags(draw, ~one);
    }
    Sound_PlayTrackedSong(0x29);
    {
        void (*update)(void) = CoffinBike_UpdateStopping;
        u8 *active = &glove->gloveActive;
        *active = one;
        glove->gloveUpdate = update;
        glove->gloveTimer = 0;
    }
}
