#include "global.h"
#include "scene_pool.h"
#define Glove_StopWithMessage sub_08119F9C
#define Packet_Send SceneMessages_Send
#define gMessagePacket        gUnk_020001F0
struct Packet {
    u16 id;
    u8 kind, zero;
    u16 reserved;
    u8 command, count;
};
extern struct Packet gUnk_020001F0;
void SceneMessages_Send(struct Packet *);
void Sound_PlayTrackedSong(int);
void sub_08117948(void);
static inline void clearDrawFlags(struct ScenePoolPlayerDraw *d, s16 mask)
{
    d->flags = mask & d->flags;
}
void sub_08119F9C(struct ScenePoolPlayer *glove)
{
    struct Packet *packet = &gUnk_020001F0;
    u32 zero = 0;
    u32 one;
    packet->zero = zero;
    packet->reserved = zero;
    packet->command = 0x41;
    packet->count = zero;
    packet->kind = 4;
    packet->id = 0x7c56;
    SceneMessages_Send(&gUnk_020001F0);
    {
        struct ScenePoolPlayerDraw *draw = &glove->gloveDraw;
        one = 1;
        clearDrawFlags(draw, ~one);
    }
    Sound_PlayTrackedSong(0x29);
    {
        void (*update)(void) = sub_08117948;
        u8 *active = &glove->gloveActive;
        *active = one;
        glove->gloveUpdate = update;
        glove->gloveTimer = zero;
    }
}
