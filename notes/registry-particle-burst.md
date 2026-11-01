# Registry update and contact particles

Tags: #scene-actors #matching 08070FA8

The registry updater remains guarded. After a destroy callback frees its context, the ROM
loads the next node's actor directly. The draft now follows that edge and uses the shared
game-state region, solar and flag fields. Three draws per particle supply its X, Y and Z
velocity. Pointer lifetimes around the callback loop and palette branch still differ.
