#include "tile_animation.h"

struct TileAnimation *TileAnimation_GetTask(void)
{
    return (struct TileAnimation *)gTileAnimation;
}

void TileAnimation_Advance(struct TileAnimation *animation, struct TileAnimationChannel *channel)
{
    if (++channel->elapsed >= channel->duration[channel->frame]) {
        channel->elapsed = 0;
        if (++channel->frame >= channel->frameCount)
            channel->frame = 0;
        channel->dirty = 1;
    }
}

int TileAnimation_Update(struct TileAnimation *animation)
{
    struct TileAnimationChannel *channel = animation->channels;
    int index = 7;
    do {
        if (channel->active) {
            if (channel->removePending) {
                channel->active = 0;
                channel->removePending = 0;
            } else if (channel->ignorePause || !gEnginePauseFlags)
                TileAnimation_Advance(animation, channel);
        }
        index--;
        channel++;
    } while (index >= 0);
    return 0;
}

u32 TileAnimation_Destroy(struct TileAnimation *animation)
{
    if (animation->tiles != 0) {
        Heap_Free(animation->tiles);
        animation->tiles = 0;
    }
    return gTileAnimation = 0;
}

int TileAnimation_Initialize(struct TileAnimation *animation)
{
    int index, resource, mode;
    struct TileAnimationChannel *channel;
    void *data;
    gTileAnimation = (u32)animation;
    animation->tileCount = Script_GetKeywordValue('s', 32);
    animation->tiles = Heap_Alloc(animation->tileCount * 32);
    if (animation->tiles) {
        Memory_Clear(animation->tiles, animation->tileCount * 32);
        channel = animation->channels;
        {
            int zero = 0;
            index = 7;
            do {
                channel->active = zero;
                channel->removePending = zero;
                channel->tileCount = zero;
                channel->id = zero;
                index--;
                channel++;
            } while (index >= 0);
        }
        resource = Script_GetKeywordValue('p', 0);
        mode = Script_GetKeywordValue('m', 0);
        data = ScriptProc_Start(0xceef, (u16)resource);
        if (data && Bg_CopyTiles(data, mode, animation->tiles, 0, animation->tileCount) >= 0)
            return 0;
    }
    return -1;
}

struct TileAnimation *TileAnimation_Create(void)
{
    struct TileAnimation *animation;
    if (gTileAnimation != 0)
        return (struct TileAnimation *)gTileAnimation;
    animation = Task_Create(0xC, sizeof(struct TileAnimation));
    if (animation != 0) {
        Task_SetCallbacks(animation, (void (*)(void *))TileAnimation_Update,
                          (void (*)(void *))TileAnimation_Destroy);
        if (TileAnimation_Initialize(animation) < 0) {
            Task_Kill(animation);
            return 0;
        }
    }
    return animation;
}

void TileAnimation_Upload(void)
{
    struct TileAnimation *animation = TileAnimation_GetTask();
    struct TileAnimationChannel *channel;
    int index;
    if (animation) {
        channel = animation->channels;
        {
            void **tiles = &animation->tiles;
            volatile u32 *dma = (volatile u32 *)0x040000d4;
            index = 7;
            do {
                if (channel->active && channel->dirty) {
                    u32 src = (u32)*tiles + channel->source[channel->frame] * 32;
                    u32 dst = 0x06000000 + channel->destination[channel->frame] * 32;
                    dma[0] = src;
                    dma[1] = dst;
                    dma[2] = (channel->tileCount * 8) | 0x84000000;
                    dma[2];
                    channel->dirty = 0;
                }
                index--;
                channel++;
            } while (index >= 0);
        }
    }
}

struct TileAnimationChannel *TileAnimation_FindFreeChannel(struct TileAnimation *animation)
{
    struct TileAnimationChannel *channel = animation->channels;
    s32 index = 0;
    while (index <= 7) {
        if (channel->active == 0) {
            return channel;
        }
        index++;
        channel++;
    }
    return 0;
}

struct TileAnimationChannel *TileAnimation_FindChannel(struct TileAnimation *animation, u32 id)
{
    struct TileAnimationChannel *channel = animation->channels;
    s32 index = 0;
    do {
        if (channel->active != 0 && channel->id == id)
            return channel;
        index++;
        channel++;
    } while (index <= 7);
    return 0;
}

void TileAnimation_Configure(void)
{
    struct TileAnimation *animation = TileAnimation_GetTask();
    struct TileAnimationChannel *channel;
    int index, value;
    u16 *frameValue;
    if (!animation)
        return;
    channel = TileAnimation_FindFreeChannel(animation);
    if (!channel)
        return;
    channel->id = Script_GetKeywordValue('c', 0);
    if (channel->id && TileAnimation_FindChannel(animation, channel->id))
        return;
    channel->tileCount = Script_GetKeywordValue('s', 1);
    channel->frame = Script_GetKeywordValue('S', 0);
    channel->frameCount = Script_GetKeywordValue('a', 1);
    channel->ignorePause = Script_GetKeywordValue('p', 0);
    channel->dirty = 1;
    channel->elapsed = 0;
    if (channel->tileCount > animation->tileCount)
        channel->tileCount = animation->tileCount;
    if (!channel->tileCount)
        channel->tileCount = 1;
    if (channel->frameCount > 8)
        channel->frameCount = 8;
    else if (!channel->frameCount)
        channel->frameCount = 1;
    if (channel->frame >= channel->frameCount)
        channel->frame = channel->frameCount - 1;

    if (Script_SeekToKeyword('R')) {

        index = 0;
        if (index < channel->frameCount) {
            frameValue = channel->source;
            do {
                value = (s32)Script_GetPc();
                if (value)
                    value = Script_GetValue();
                *frameValue = value;
                frameValue++;
                index++;
            } while (index < channel->frameCount);
        }
    } else {

        for (index = 0; index < channel->frameCount; index++)
            channel->source[index] = 0;
    }

    if (Script_SeekToKeyword('d')) {

        index = 0;
        if (index < channel->frameCount) {
            frameValue = channel->destination;
            do {
                value = (s32)Script_GetPc();
                if (value)
                    value = Script_GetValue();
                *frameValue = value;
                frameValue++;
                index++;
            } while (index < channel->frameCount);
        }
    } else {

        for (index = 0; index < channel->frameCount; index++)
            channel->destination[index] = 0;
    }

    if (Script_SeekToKeyword('t')) {

        index = 0;
        if (index < channel->frameCount) {
            frameValue = channel->duration;
            do {
                value = (s32)Script_GetPc();
                if (value)
                    value = Script_GetValue();
                *frameValue = value;
                frameValue++;
                index++;
            } while (index < channel->frameCount);
        }
    } else {

        for (index = 0; index < channel->frameCount; index++)
            channel->duration[index] = 0;
    }
    channel->removePending = 0;
    channel->active = 1;
}

void TileAnimation_Stop(void)
{
    struct TileAnimation *animation = TileAnimation_GetTask();
    struct TileAnimationChannel *channel;
    int id, frame;
    if (animation) {
        id = Script_GetKeywordValue('c', 0);
        if (id) {
            channel = TileAnimation_FindChannel(animation, id);
            if (channel) {
                channel->removePending = 1;
                frame = Script_GetKeywordValue('i', -1);
                if (frame >= 0) {
                    channel->frame = frame;
                    if (channel->frame >= channel->frameCount)
                        channel->frame = channel->frameCount - 1;
                    channel->dirty = 1;
                }
            }
        }
    }
}
