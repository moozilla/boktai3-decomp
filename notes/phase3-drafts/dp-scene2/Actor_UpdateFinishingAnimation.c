void Actor_UpdateFinishingAnimation(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    u8 done;
    if (take(p)) {
        p->state = 10;
        Anim_SetSequence(&p->anim, p->arg, 10, 0, p->last);
        ActorShadow_Disable(p->p2);
        Sound_PlaySong(287);
    }
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    if (++a->timer >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
    if (done) {
        if (p->releaseControl)
            ScriptUtility_ClearControlFlag();
        p->obj.flags |= 1;
        Actor_SetState(p, 1, 2);
        {
            u32 active = 4;
            register void (*fn)(void *, void *) = Actor_OnInteractionChildUpdate;
            u8 reset = 1;
            struct ActorChildCallbackView *child =
                (struct ActorChildCallbackView *)&p->children[0].fn; 
            s32 i;
            for (i = 15; i >= 0; i--) {
                union DisplaySequenceControl control;
                control.raw = child->flags;
                if (active & control.raw) {
                    u32 flags;
                    child->fn = fn;
                    control.raw = reset | control.raw;
                    child->flags = control.raw;
                }
                child++;
            }
        }
    }
}

