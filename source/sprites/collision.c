#include "source/sprites/collision.h"
#include "source/library/bank_helpers.h"

CODE_BANK(PRG_BANK_SPRITE_COLLISION);
ZEROPAGE_DEF(unsigned char, collisionTemp);

unsigned char test_collision(unsigned char tileId, unsigned char isPlayer) {
    // The top 2 bits of the tile id are the palette color. We don't care about that here, so skip them.

    // I'm a dumbass and keep using the grass tile as water, so manually strip it out
    // HACK: This isn't exactly efficient
    if (tileId == 64 || tileId == 65) {
        return 1;
    }
    collisionTemp = tileId & 0x3f;
    if (collisionTemp >= FIRST_SOLID_SPRITE && collisionTemp <= LAST_SOLID_SPRITE) {
        return 1;
    }
    if (collisionTemp == 57) {
        return 1;
    }
    return 0;
}