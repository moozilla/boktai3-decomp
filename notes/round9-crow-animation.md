# Crow profile animation

Tags: 0809EF50 0808E9A0 #crow #slime #depin

`Crow_PlayEntry2` matches its 212 bytes through the same profile-selection
helper as `Slime_PlayWaitAnimation`. The existing turn animation selector
handles the tile restart and mode bits without the private pinned copy.

Tags: 0809F174 #crow #depin

`Crow_RecoverFromDisplacement` also matches without barriers. A halfword
initializer supplies the byte state while the word counter retains its own
zero. Clearing the contact bit through the halfword flag view preserves the
mask and load order.
