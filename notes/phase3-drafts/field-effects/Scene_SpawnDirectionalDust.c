void Scene_SpawnDirectionalDust(struct SceneObject *actor)
{
    struct SceneExtent size;
    struct SceneExtent point;
    struct SceneExtent *particle;
    s32 i;
    const u16 *table;
    u32 *cursor;
    u32 mask;
    u32 index;
    size.x = 32;
    size.y = 0;
    size.z = 32;
    particle = &point;
    table = gRandomTable;
    cursor = &gRandomIndex;
    i = 2;
    mask = 1023;
    do {
        point = *(struct SceneExtent *)&actor->body.position;
        switch (actor->direction) {
            case 0:
                index = (*cursor + 1) & mask;
                point.x = dustScatter(point.x, table[index]);
                index = (index + 1) & mask;
                *cursor = index;
                particle->z = frontEdge(table[index], particle->z);
                break;
            case 1:
                index = (*cursor + 1) & mask;
                point.x = backEdge(table[index], point.x);
                index = (index + 1) & mask;
                *cursor = index;
                particle->z = dustScatter(particle->z, table[index]);
                break;
            case 2:
                index = (*cursor + 1) & mask;
                point.x = dustScatter(point.x, table[index]);
                index = (index + 1) & mask;
                *cursor = index;
                particle->z = backEdge(table[index], particle->z);
                break;
            case 3:
                index = (*cursor + 1) & mask;
                point.x = frontEdge(table[index], point.x);
                index = (index + 1) & mask;
                *cursor = index;
                particle->z = dustScatter(particle->z, table[index]);
                break;
        }
        SceneParticleBurst_CreateWithPause((struct SceneVec *)&point, 0, &size, 1, 2, 4);
    } while (--i >= 0);
}
