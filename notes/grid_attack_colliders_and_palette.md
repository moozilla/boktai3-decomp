# Grid attack colliders and palette

Tags: #matching #bosses 0802BAFC

GridAttack_UpdateCollidersAndPalette matches all 1,076 bytes. The actor updates
the damage and body colliders in sequence using a single collider pointer. The
camera enable bit belongs to the tracking requests at offset 0x20. The palette
logic reads the resource index before testing the three blend modes, as in the
Vampire frame update. Keeping the default-palette copy in an inline helper
preserves the argument order for CpuSet.
