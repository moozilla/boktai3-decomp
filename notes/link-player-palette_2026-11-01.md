# Link player palette

Tags: 08239848 LinkPlayer_BlendPalette

LinkPlayer_BlendPalette matches all 472 bytes. The blend in upstream commit
`f4b5b3329` also matches with the shared LinkPlayerFade layout used by its
caller. Passing the source color by address allows agbcc to extract the red
channel directly from the loaded halfword in both loops. The primary form
preserves palette entries 5, 6 and 13.
