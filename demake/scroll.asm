; Scroll a ROM-resident 64x64 world map through the DMG's 32x32 BG map.
; During visible scan, gather the entering row/column into two WRAM buffers
; in hardware ring order. Only the two unrolled transfers run during VBlank.
projection_word:
    ld l,a
    ld h,0x43
    ld e,[hl]
    ld h,0x70
    ld d,[hl]
    ret

project:
    call surface
    call projection_word
    ld a,e
    ld [projected_h],a
    ld a,d
    ld [projected_h_hi],a
    ld a,b
    call projection_word
    push de
    ld a,c
    call projection_word
    pop hl
    push hl
    push de
    call subtract_de
    ld a,[header+12]
    ld e,a
    ld a,[header+13]
    ld d,a
    add hl,de
    ld a,l
    ld [world_x],a
    ld a,h
    ld [world_x_hi],a
    pop de
    pop hl
    add hl,de
    ld a,[projected_h]
    ld e,a
    ld a,[projected_h_hi]
    ld d,a
    call subtract_de
    ld a,[header+14]
    ld e,a
    ld a,[header+15]
    ld d,a
    add hl,de
    ld a,l
    ld [world_y],a
    ld a,h
    ld [world_y_hi],a
    ret

subtract_de:
    ld a,l
    sub e
    ld l,a
    ld a,h
    sbc d
    ld h,a
    ret

camera:
    ld a,[world_x]
    ld l,a
    ld a,[world_x_hi]
    ld h,a
    ld e,80
    ld bc,352
    call clamp_axis
    ld a,[camera_x_hi]
    ld d,a
    ld a,[camera_x]
    ld e,a
    call follow_axis
    ld a,l
    ld [camera_x],a
    ld a,h
    ld [camera_x_hi],a
    ld a,[world_y]
    ld l,a
    ld a,[world_y_hi]
    ld h,a
    ld e,82
    ld bc,368
    call clamp_axis
    ld a,[camera_y_hi]
    ld d,a
    ld a,[camera_y]
    ld e,a
    call follow_axis
    ld a,l
    ld [camera_y],a
    ld a,h
    ld [camera_y_hi],a
    ret
; Cap normal tracking at seven pixels per frame. A high step or slope can
; shift the projected feet faster than the single entering tile row.
follow_axis:
    ld a,[camera_snap]
    and a
    ret nz
    push hl
    call subtract_de
    bit 7,h
    jr nz,.negative
    ld a,h
    and a
    jr nz,.forward
    ld a,l
    cp 8
    jr c,.near
.forward:
    pop hl
    ld hl,7
    add hl,de
    ret
.negative:
    ld a,h
    cp 255
    jr nz,.back
    ld a,l
    cp 249
    jr nc,.near
.back:
    pop hl
    ld hl,0xfff9
    add hl,de
    ret
.near:
    pop hl
    ret

clamp_axis:
    ld d,0
    call subtract_de
    jr nc,.max
    ld hl,0
    ret
.max:
    ld a,h
    cp b
    ret c
    jr nz,.limit
    ld a,l
    cp c
    ret c
.limit:
    ld h,b
    ld l,c
    ret

screen_xy:
    ld a,[world_x]
    ld hl,camera_x
    sub [hl]
    ld d,a
    ld a,[world_x_hi]
    ld hl,camera_x_hi
    sbc [hl]
    jr z,.right
    cp 255
    jr nz,.skip
    ld a,d
    cp 249
    jr c,.skip
    jr .vertical
.right:
    ld a,d
    cp 168
    jr nc,.skip
.vertical:
    ld a,[world_y]
    ld hl,camera_y
    sub [hl]
    ld e,a
    ld a,[world_y_hi]
    ld hl,camera_y_hi
    sbc [hl]
    jr nz,.skip
    ld a,e
    cp 136
    jr nc,.skip
    and a
    ret
.skip:
    scf
    ret

tile_origin:
    ld a,[camera_x_hi]
    ld b,a
    ld a,[camera_x]
    call divide8
    ld [tile_x],a
    ld a,[camera_y_hi]
    ld b,a
    ld a,[camera_y]
    call divide8
    ld [tile_y],a
    ret
divide8:
    rrca
    rrca
    rrca
    and 31
    bit 0,b
    ret z
    or 32
    ret

; (B,C) are world tile X,Y, masked to a 64x64 ROM plane. Preserve BC.
world_tile:
    ld a,c
    rrca
    rrca
    and 15
    add 0x60
    ld h,a
    ld a,c
    rrca
    rrca
    and 0xc0
    ld l,a
    ld a,b
    and 63
    or l
    ld l,a
    ld a,[hl]
    ret

gather_row:
    ; C = entering world row. Save the corresponding VRAM destination.
    ld a,c
    rrca
    rrca
    rrca
    and 3
    add 0x98
    ld [row_hi],a
    ld a,c
    rrca
    rrca
    rrca
    and 0xe0
    ld [row_lo],a
    ld a,[tile_x]
    ld b,a
    ld d,0xc4
    ld a,32
.loop:
    push af
    ld a,b
    and 31
    ld e,a
    call world_tile
    ld [de],a
    inc b
    pop af
    dec a
    jr nz,.loop
    ld a,1
    ld [row_pending],a
    ret

gather_col:
    ; B = entering world column.
    ld a,b
    and 31
    ld [col_x],a
    ld a,[tile_y]
    ld c,a
    ld d,0xc5
    ld a,32
.loop:
    push af
    ld a,c
    and 31
    ld e,a
    call world_tile
    ld [de],a
    inc c
    pop af
    dec a
    jr nz,.loop
    ld a,1
    ld [col_pending],a
    ret

initial_map:
    call tile_origin
    ld a,[tile_y]
    ld c,a
    ld b,32
.loop:
    push bc
    call gather_row
    call flush_row
    pop bc
    inc c
    dec b
    jr nz,.loop
    xor a
    ld [row_pending],a
    ld [col_pending],a
    jp remember_tiles

prepare_map:
    call tile_origin
    ld a,[tile_x]
    ld hl,cached_x
    cp [hl]
    jr z,.y
    jr c,.col
    add 20
.col:
    ld b,a
    call gather_col
.y:
    ld a,[tile_y]
    ld hl,cached_y
    cp [hl]
    jr z,remember_tiles
    jr c,.row
    add 18
.row:
    ld c,a
    call gather_row
remember_tiles:
    ld a,[tile_x]
    ld [cached_x],a
    ld a,[tile_y]
    ld [cached_y],a
    ret

flush_map:
    ld a,[row_pending]
    and a
    call nz,flush_row
    ld a,[col_pending]
    and a
    call nz,flush_col
    xor a
    ld [row_pending],a
    ld [col_pending],a
    ret

flush_row:
    ld a,[row_hi]
    ld h,a
    ld a,[row_lo]
    ld l,a
    ld de,0xc400
    ld b,8
.loop:
    ld a,[de]
    inc e
    ld [hl+],a
    ld a,[de]
    inc e
    ld [hl+],a
    ld a,[de]
    inc e
    ld [hl+],a
    ld a,[de]
    inc e
    ld [hl+],a
    dec b
    jr nz,.loop
    ret
flush_col:
    ld a,[col_x]
    ld l,a
    ld h,0x98
    ld de,0xc500
    ld bc,32
.loop:
    ld a,[de]
    inc e
    ld [hl],a
    add hl,bc
    ld a,[de]
    inc e
    ld [hl],a
    add hl,bc
    ld a,[de]
    inc e
    ld [hl],a
    add hl,bc
    ld a,[de]
    inc e
    ld [hl],a
    add hl,bc
    ld a,e
    cp 32
    jr nz,.loop
    ret
