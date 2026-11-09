s32 EffectManager_UpdateAll(struct EffectManager *manager)
{
    struct EffectContext context;
    struct EffectContext *ctx;
    struct EffectPackedVec actorWorld;
    struct EffectVec actorScreen;
    struct Effect *p, *view;
    s32 i;
    u32 mask = 512;
    u32 flags;
    flags = gEngineFlags | gScenePauseFlags;
    mask &= flags;
    if (mask) {
        flags &= 256;
        if (!flags)
            goto done;
    }
    actorWorld.x = manager->actor->position.x;
    actorWorld.y = manager->actor->position.y + 128;
    actorWorld.z = manager->actor->position.z;
    projectActorToScreen(&actorScreen, (struct EffectVec *)&actorWorld);
    ctx = &context;
    p = manager->effects;
    i = 5;
    do {
        view = p;
        if (view->active) {
            if (view->type == 2) {
                ctx->world.x = ((struct EffectVec *)&actorWorld)->x;
                ctx->world.y = ((struct EffectVec *)&actorWorld)->y;
                ctx->world.z = ((struct EffectVec *)&actorWorld)->z;
            } else if (manager->actor->facing == 0) {
                ctx->world.x = manager->actor->position.x + 24;
                ctx->world.y = manager->actor->position.y + 350;
                ctx->world.z = manager->actor->position.z - 24;
            } else {
                ctx->world.x = manager->actor->position.x - 32;
                ctx->world.y = manager->actor->position.y + 320;
                ctx->world.z = manager->actor->position.z + 32;
            }
            projectToScreen(&context.lower, &context.world);
            context.upper = context.lower;
            ctx->lower.x -= 5;
            ctx->lower.y -= 5;
            ctx->upper.x += 5;
            ctx->upper.y += 5;
            context.actorLower = actorScreen;
            context.actorUpper = actorScreen;
            ctx->actorLower.x -= 8;
            ctx->actorLower.y -= 20;
            ctx->actorUpper.x += 8;
            ctx->actorUpper.y += 20;
            EffectManager_UpdateParticles(manager, p, &context);
            if (view->soundDelay) {
                view->soundDelay--;
                if (!view->soundDelay) {
                    u32 type = view->type;
                    Sound_PlaySongChecked(gUnk_08604A6C[type]);
                }
            }
            if (view->timer)
                view->timer--;
        }
        p++;
    } while (--i >= 0);
    {
        struct EffectManager *end = manager;
        end->counter++;
    }
done:
    return 0;
}
