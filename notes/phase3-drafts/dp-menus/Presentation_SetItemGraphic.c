// MATCHING_CONCESSION: register-pinning
void Presentation_SetItemGraphic(struct SelectionMenu *p, struct SceneInputWidget *q, u32 item)
{
    int index = Inventory_GetConsumableId(item);
    if (index >= 0) {
        struct SelectionMenuItem *entry = &gUnk_08E60F80[index];
        void *context = &p->iconResource;
        u16 resource = 0x121;
        resource += entry->graphic;
        Anim_LoadKeyframe(q, context, resource, 0);
        q->flags &= ~1;
    } else
        q->flags |= 1;
}

