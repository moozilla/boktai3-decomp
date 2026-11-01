# Scene departure display

Tags: 081EDB74 SceneActors_UpdateLeftDeparture

SceneActors_UpdateLeftDeparture matches all 728 bytes. It moves the two
drawable layers left and switches to the destruction callback at negative
screen x. Its callback setter takes the value before its destination, as in
the adjacent destruction transitions.

Tags: 081EEAD8 SceneActors_UpdatePaletteDispersal

SceneActors_UpdatePaletteDispersal matches all 772 bytes. The display first
fades two sprite palettes and four background banks. After a 50-frame wait it
emits five particles per update until it has emitted 25.
