# Actor glyph trail

Tags: 08237938 ActorGlyphTrail_UpdateParticles

All 240 bytes match. Each active slot increments its own timer. At frame 15
the routine hides the slot. Before then, it uses a shrinking radius and two
samples a quarter-turn apart to offset the copy of the owner position.
