#include "global.h"
#include "scene_pool.h"
#include "ram.h"
#define Packet_Send SceneMessages_Send
struct Packet {
    u16 id;
    u8 kind, zero;
    u16 reserved;
    u8 command, count;
};
void SceneMessages_Send(struct Packet *);
void Sound_PlayTrackedSong(int);
void Glove_UpdateStopping(void);
static inline void clearDrawFlags(struct ScenePoolPlayerDraw *d, s16 mask)
{
    d->flags = mask & d->flags;
}
void Glove_StopWithMessage(struct ScenePoolPlayer *glove)
{
    struct Packet *packet = &gMessagePacket;
    u32 zero = 0;
    u32 one;
    packet->zero = zero;
    packet->reserved = zero;
    packet->command = 0x41;
    packet->count = zero;
    packet->kind = 4;
    packet->id = 0x7c56;
    SceneMessages_Send(&gMessagePacket);
    {
        struct ScenePoolPlayerDraw *draw = &glove->gloveDraw;
        one = 1;
        clearDrawFlags(draw, ~one);
    }
    Sound_PlayTrackedSong(0x29);
    {
        void (*update)(void) = Glove_UpdateStopping;
        u8 *active = &glove->gloveActive;
        *active = one;
        glove->gloveUpdate = update;
        glove->gloveTimer = zero;
    }
}
