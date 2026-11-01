# RegionActor_UpdateBehaviorAnimation

All 944 bytes match using the shared sequence helpers. The first branch needs
the end-of-sequence result. The second and post-damage branches only advance
playback. The first two use a saved sprite pointer, while the post-damage branch
takes the sprite address from the slot again.
