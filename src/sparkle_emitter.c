#include "sparkle_emitter.h"

static inline s32 Sparkle_ProjectScale(s32 value)
{
    if (value >= 0)
        value >>= 5;
    else
        value = -((-value) >> 5);
    return value;
}

static inline void Sparkle_ProjectPosition(struct ContactPosition *screen, s16 *position)
{
    s32 x, z, sum, height, y, depth;
    x = position[0];
    z = position[2];
    screen->x = Sparkle_ProjectScale(3 * (x - z));
    sum = Sparkle_ProjectScale(3 * (x + z));
    height = Sparkle_ProjectScale(3 * *++position);
    y = sum - height;
    depth = sum + height;
    screen->x = (u16)screen->x - gCameraOrigin[0] + 120;
    screen->y = y - gCameraOrigin[1] + 80;
    screen->z = depth - gCameraOrigin[2];
}

static inline void SparkleEmitter_RemoveParticle(struct SparkleEmitter *emitter)
{
    emitter->count--;
}

void SparkleEmitter_UpdateParticles(struct SparkleEmitter *arg0)
{
    struct SparkleEmitter *emitter = (struct SparkleEmitter *)arg0;
    struct SparkleParticle *particle = emitter->particles;
    s32 i;
    i = 0;
    do {
        if (!(particle->sprite.flags & 1)) {
            struct ContactPosition screen;
            struct ContactPosition *position = &particle->sprite.position;
            position->x += particle->velocity.x;
            position->y += particle->velocity.y;
            position->z += particle->velocity.z;
            particle->sprite.frame = (particle->age >> 3) % 4;
            particle->age++;
            Sparkle_ProjectPosition(&screen, &position->x);
            if ((u16)(screen.x + 128) > 448 || screen.y > 320 || screen.y < -128) {
                particle->sprite.flags |= 1;
                SparkleEmitter_RemoveParticle(emitter);
            }
        }
        particle++;
        i++;
    } while (i < 32);
}

/* Tentative particle pool: caller clears bit zero and increments the count. */

struct SparkleParticle *ParticlePool_FindFreeEntry(struct SparkleEmitter *p)
{
    s32 i, offset;
    u32 mask;
    struct SparkleEmitter *cursor;
    if (p->count > 31)
        return NULL;
    i = 0;
    mask = 1;
    cursor = p;
    offset = 0;
    for (; i <= 31; i++) {
        if (cursor->particles[0].sprite.flags & mask)
            return (void *)((u8 *)p + (offset + (u32) & ((struct SparkleEmitter *)0)->particles));
        cursor = (void *)((u8 *)cursor + sizeof(struct SparkleParticle));
        offset += sizeof(struct SparkleParticle);
    }
    return 0;
}

extern const s16 gUnk_086149C4[];

static inline s32 random(void)
{
    const u16 *table = gRandomTable;
    u32 *indexPtr = &gRandomIndex;
    u32 index = *indexPtr + 1;
    u32 mask = 1023;
    index &= mask;
    *indexPtr = index;
    return table[index];
}

static inline s32 randomCircle(void)
{
    /* The shared random table resides in work RAM. */
    return ((const u16 *)0x0203B400)[gRandomIndex = (gRandomIndex + 1) & 1023];
}

static inline s32 scale12(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}

static inline s32 half(s32 x)
{
    return x >= 0 ? x >> 1 : -((-x) >> 1);
}

static inline void circle(s32 angle, struct ContactPosition *p, s32 radius)
{
    p->x = scale12(gUnk_086149C4[(angle + 64) & 255] * radius);
    p->y = scale12(gUnk_086149C4[angle] * radius);
    p->z = 0;
}

static inline void unproject(struct ContactPosition *p, struct ContactPosition *screen)
{
    s32 x = screen->x + ((s16 *)gCameraOrigin)[0] - 120;
    s32 y = screen->y + ((s16 *)gCameraOrigin)[1] - 80;
    p->x = half(((x + y) << 5) / 3 + 1024);
    p->y = 1024;
    p->z = half(((y - x) << 5) / 3 + 1024);
}

static inline void polar(struct ContactPosition *p, s32 angle, s32 speed)
{
    p->x = scale12(gUnk_086149C4[(angle + 64) & 255] * speed);
    p->y = 0;
    p->z = scale12(gUnk_086149C4[angle & 255] * speed);
}

void SparkleEmitter_EmitParticle(struct SparkleEmitter *input)
{
    struct SparkleEmitter *emitter = input;
    struct SparkleParticle *particle;
    struct ContactPosition screen;
    s32 angle;
    s32 baseAngle;
    if (emitter->speed <= 1)
        return;
    particle = ParticlePool_FindFreeEntry(emitter);
    if (!particle)
        return;
    emitter->count++;
    particle->sprite.flags &= ~1;
    angle = randomCircle();
    baseAngle = emitter->angle;
    angle = ((angle >> 4) & 63) + baseAngle;
    circle((angle + 128) & 255, &screen, 150);
    screen.x += 120;
    screen.y += 80;
    unproject(&particle->sprite.position, &screen);
    polar(&particle->velocity, (emitter->angle + ((random() >> 4) & 15)) - 8,
          emitter->speed + ((random() >> 4) & 3));
    particle->age = 0;
}

void SparkleEmitter_OnUpdateHook(void *unused)
{
}

void SparkleEmitter_UpdateEmitting(struct SparkleEmitter *p)
{
    u32 *c = (u32 *)&p->timer;
    if ((*c & 1) == 0) {
        SparkleEmitter_EmitParticle(p);
    }
    SparkleEmitter_UpdateParticles(p);
    *c = *c + 1;
}

void SparkleEmitter_UpdateDraining(struct SparkleEmitter *p)
{
    SparkleEmitter_UpdateParticles(p);
    p->timer += 1;
}

void Sound_PauseChannel12(u32);

static inline u8 Entry_MessageCount(struct SceneEventReceiver *entry)
{
    struct SceneEventDispatchState *controller = gSceneEventDispatchState;
    return entry->count[controller->phase];
}

static inline void SparkleEmitter_SetUpdate(struct SparkleEmitter *p,
                                            void (*update)(struct SparkleEmitter *))
{
    p->update = update;
}

static inline s32 SparkleEmitter_SetTimer(struct SparkleEmitter *p, s32 timer)
{
    return p->timer = timer;
}

static inline struct SparkleEmitterCommand *Entry_GetMessage(struct SceneEventReceiver *entry,
                                                             s32 i)
{
    struct SceneEventDispatchState *controller = gSceneEventDispatchState;
    return (struct SparkleEmitterCommand *)entry->pending[controller->phase][i];
}

static inline u8 SparkleEmitter_QuantizeDirection(s16 direction)
{
    s16 phase = direction + 5;
    return (u32)phase << 5;
}

// confidence=low: handles the emitter's cutscene messages: 0xC starts emitting with a speed,

// direction and palette from the message, 0xD drains it.

void SparkleEmitter_ProcessMessages(struct SparkleEmitter *p)
{
    struct SceneEventReceiver *entry = &p->receiver;
    s32 i;
    for (i = 0; i < Entry_MessageCount(entry); i++) {
        struct SparkleEmitterCommand *m = Entry_GetMessage(entry, i);
        SceneMessages_SetPendingTrigger(entry, (struct SceneEventCommand *)m);
        switch (m->command.kind) {
            case 12: {
                s32 speed = m->command.value;
                s16 direction = SparkleEmitter_QuantizeDirection(m->command.parameterA);
                u32 palette = m->palette;
                p->speed = speed;
                p->angle = direction;
                p->palette = palette;
                if (palette <= 14)
                    AnimData_SelectPaletteBank(p->animation, palette + 0x255);
                Sound_PlaySong(0x83);
                SparkleEmitter_SetUpdate(p, SparkleEmitter_UpdateEmitting);
                SparkleEmitter_SetTimer(p, 0);
                Trigger_ConsumePending(entry, 1);
                break;
            }

            case 13:
                Sound_PauseChannel12(8);
                SparkleEmitter_SetUpdate(p, SparkleEmitter_UpdateDraining);
                p->timer = 0;
                Trigger_ConsumePending(entry, 1);
                break;
        }
    }
}

void SparkleEmitter_OnTaskUpdate(void *arg)
{
    struct SparkleEmitter *p = arg;
    SparkleEmitter_ProcessMessages(p);
    SparkleEmitter_OnUpdateHook(p);
    if (p->update != 0)
        Callback_InvokeActor(p, p->update);
}

void SceneMessages_UnregisterReceiver(void *);

void SparkleEmitter_OnTaskDestroy(void *arg)
{
    struct SparkleEmitter *p = arg;
    s32 i;
    struct SparkleParticle *e = p->particles;
    u32 z;
    for (i = 31; i >= 0; i--) {
        Sprite_Remove(e);
        e++;
    }
    z = 0;
    SceneMessages_UnregisterReceiver(&p->receiver);
    gSparkleEmitter = z;
}

void SceneMessages_RegisterReceiver(void *, void *, u32);

static inline void SparkleEmitter_Start(struct SparkleEmitter *p, s32 speed, s32 angle, u32 palette)
{
    p->speed = speed;
    p->angle = angle;
    p->palette = palette;
    if (palette <= 14)
        AnimData_SelectPaletteBank(p->animation, palette + 0x255);
    Sound_PlaySong(0x83);
    SparkleEmitter_SetUpdate(p, SparkleEmitter_UpdateEmitting);
    SparkleEmitter_SetTimer(p, 0);
}

s32 SparkleEmitter_Initialize(struct SparkleEmitter *p, void *script)
{
    void *animation;
    struct SparkleParticle *sprite;
    s32 i;
    u32 one;

    p->script = script;
    animation = p->animation;
    AnimData_Load(animation, 0xCC88);
    AnimData_SelectPaletteBank(animation, 0x255);
    sprite = p->particles;
    i = 0;
    one = 1;
    do {
        u32 flags;
        Sprite_Init(sprite, p->animation, 0);
        flags = sprite->sprite.flags | one;
        sprite->sprite.flags = flags;
        if (i & one)
            flags |= 0x200;
        else
            flags |= 0x400;
        sprite->sprite.flags = flags;
        sprite++;
        i++;
    } while (i <= 31);
    SceneMessages_RegisterReceiver(&p->receiver, p->script, 10);
    SparkleEmitter_SetUpdate(p, SparkleEmitter_UpdateEmitting);
    SparkleEmitter_SetTimer(p, 0);
    p->count = 0;
    p->angle = SparkleEmitter_QuantizeDirection(Script_GetKeywordValue('d', 5));
    p->speed = Script_GetKeywordValue('p', 64);
    p->palette = Script_GetKeywordValue('c', 8);
    if (Script_GetKeywordValue('i', 1)) {
        SparkleEmitter_Start(p, p->speed, p->angle, p->palette);
    } else {
        Sound_PauseChannel12(8);
        SparkleEmitter_SetUpdate(p, SparkleEmitter_UpdateDraining);
        SparkleEmitter_SetTimer(p, 0);
    }
    return 0;
}

void SparkleEmitter_ReconfigureFromScript(void)
{
    struct SparkleEmitter *p = (struct SparkleEmitter *)gSparkleEmitter;
    s32 speed;
    s32 direction;
    s32 palette;

    if (p == 0)
        return;

    if (Script_SeekToKeyword(0x70) != 0)
        speed = Script_GetValue();
    else
        speed = p->speed;

    if (Script_SeekToKeyword(0x64) != 0) {
        u32 t = (u32)Script_GetValue();
        direction = ((t << 29) + 0xA0000000) >> 24;
    } else {
        direction = p->angle;
    }

    if (Script_SeekToKeyword(0x63) != 0)
        palette = Script_GetValue();
    else
        palette = p->palette;

    p->speed = speed;
    p->angle = direction;
    p->palette = palette;
    if ((u32)palette <= 14)
        AnimData_SelectPaletteBank(p->animation, (u32)palette + 0x255);
    Sound_PlaySong(0x83);
    {
        void (*fn)(struct SparkleEmitter *) = SparkleEmitter_UpdateEmitting;
        p->update = fn;
    }
    p->timer = 0;
}

void SparkleEmitter_StopEmitting(void)
{
    struct SparkleEmitter *p = gSparkleEmitter;
    if (p != 0) {
        void (*f)(struct SparkleEmitter *);
        Sound_PauseChannel12(8);
        f = SparkleEmitter_UpdateDraining;
        p->update = f;
        p->timer = 0;
    }
}

void *SparkleEmitter_CreateFromScript(void *a)
{
    void *p;
    if (gSparkleEmitter != 0)
        return gSparkleEmitter;
    p = gSparkleEmitter = Task_Create(8, 0x780);
    if (p != 0) {
        Task_SetCallbacks(p, SparkleEmitter_OnTaskUpdate, SparkleEmitter_OnTaskDestroy);
        if (SparkleEmitter_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
