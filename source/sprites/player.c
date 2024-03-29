#include "source/neslib_asm/neslib.h"
#include "source/sprites/player.h"
#include "source/library/bank_helpers.h"
#include "source/globals.h"
#include "source/map/map.h"
#include "source/configuration/game_states.h"
#include "source/configuration/system_constants.h"
#include "source/sprites/collision.h"
#include "source/sprites/sprite_definitions.h"
#include "source/sprites/map_sprites.h"
#include "source/menus/error.h"
#include "source/graphics/hud.h"
#include "source/graphics/game_text.h"

CODE_BANK(PRG_BANK_PLAYER_SPRITE);

// Some useful global variables
ZEROPAGE_DEF(int, playerXPosition);
ZEROPAGE_DEF(int, playerYPosition);
ZEROPAGE_DEF(int, playerXVelocity);
ZEROPAGE_DEF(int, playerYVelocity);
ZEROPAGE_DEF(unsigned char, playerControlsLockTime);
ZEROPAGE_DEF(unsigned char, playerInvulnerabilityTime);
ZEROPAGE_DEF(unsigned char, playerDirection);

// Huge pile of temporary variables
#define rawXPosition tempChar1
#define rawYPosition tempChar2
#define rawTileId tempChar3
#define collisionTempX tempChar4
#define collisionTempY tempChar5
#define collisionTempXRight tempChar6
#define collisionTempYBottom tempChar7

#define tempSpriteCollisionX tempInt1
#define tempSpriteCollisionY tempInt2

#define collisionTempXInt tempInt3
#define collisionTempYInt tempInt4

 const unsigned char* introductionText = 
                                "Welcome to nes-starter-kit!";
const unsigned char* introCharacter = 
                                "Hello! How are you?           "
                                "                              "
                                "                              "
                                
                                "... wait, why are you not     "
                                "running!? The world is ending!"
                                "                              "
                                
                                "The world is being swallowed  "
                                "up by some unknown power.     "
                                "                              "

                                "We are running out of space to"
                                "live! There must be an escape!"
                                "                              "

                                "Me? I can't run anymore. It's "
                                "ok. I lived a good life...    "
                                "                              "

                                "GO! While you still can!";
const unsigned char* level2Character = 
                                "It never ends! I've been      "
                                "running from the darkness for "
                                "days! I have gone through     "

                                "twenty dimensions, and there  "
                                "is just no end in sight!      "
                                "                              "

                                "I can't run anymore. Hopefully"
                                "the darkness takes the pain in"
                                "my legs away...";

const unsigned char* heartCharacter = 
                                "The thing next me is a        "
                                "health powerup. It restores   "
                                "some of your health. Grab it!";

const unsigned char* keyCharacter = 
                                "Grab the key south of here!   "
                                "                              "
                                "You just might need it...";

const unsigned char* vacuumCharacter = 
                                "The weird thing next to me is "
                                "part of a vacuum stabilizing  "
                                "machine.                      "
                                
                                "If we could put it together,  "
                                "we might be able to reverse   "
                                "everything happening...       "
                                
                                "Sadly this is the only part I "
                                "know of, and it takes many    "
                                "to fix the smallest tear.     "
                                
                                "If you're still running, you  "
                                "should collect these.         "
                                "                              "
                                "Maybe someone can use them,   "
                                "and fix this mess!";

const unsigned char* firstEnemyCharacter = 
                                "Some creatures have gone      "
                                "completely insane after seeing"
                                "the world shrink...           "

                                "They will try to hurt you; you"
                                "must avoid them to survive!   "
                                "                              "
                                
                                "For whatever reason they do   "
                                "not mind me, but I have seen  "
                                "many travellers perist.       "

                                "Be careful!";

const unsigned char* runCharacter = 

                                "RUN!!!!";

const unsigned char* fasterCharacter = 
                                "The darkness seems to be      "
                                "advancing even faster now. Get"
                                "moving!";

const unsigned char* labAheadCharacter = 
                                "I've heard there is a lab full"
                                "of scientists ahead. Rumor is "
                                "they combine robots with forms"

                                "of life.                      "
                                "                              "
                                "                              "

                                "People also say that they have"
                                "the ability to control space  "
                                "and time.                     "
                                
                                "They might be able to stop    "
                                "the oncoming darkness.        "
                                "                              "

                                "I also heard their creations  "
                                "are violent. Be very careful!";

const unsigned char* movedText = 
                                "Hey, you put me on another    "
                                "screen! Cool!";
const unsigned char* labber =   "Our lab is ahead. GO!! RUN!   ";

const unsigned char* allVacuums =
                                "Oh my goodness, are you ok!?  "
                                "No one has come through that  "
                                "way in centuries.             "
                                
                                "That's where we put our       "
                                "robots that went wrong.       "
                                "Someone made a mistake many   "

                                "years ago, and our docile     "
                                "worker robots became extremely"
                                "aggressive.                   "

                                "The world you came through is "
                                "disappearing, and you're      "
                                "running out of space?         "

                                "I can help with that. We had  "
                                "that problem a few decades ago"
                                "and found a way to stop it.   "

                                "I see you have a dozen pieces "
                                "of the vacuum stabilizing     "
                                "machine.                      "
                                
                                "If you just give me those, I  "
                                "will reverse the damage, and  "
                                "you can head back home.       "

                                "Just head through the door    "
                                "below to get back to your own "
                                "dimension.";
const unsigned char* notAllVacuums = // are created equal??
                                "Oh my goodness, are you ok!?  "
                                "No one has come through that  "
                                "way in centuries.             "
                                
                                "That's where we put our       "
                                "robots that went wrong.       "
                                "Someone made a mistake many   "

                                "years ago, and our docile     "
                                "worker robots became extremely"
                                "aggressive.                   "

                                "The world you came through is "
                                "disappearing, and you're      "
                                "running out of space?         "

                                "Unfortunately, there is not   "
                                "much I can do. We have dealt  "
                                "with that before, but...      "

                                "We need vacuum stabilizer     "
                                "pieces to fix it, and there   "
                                "are no more in this dimension."
                                
                                "You are welcome to stay here  "
                                "with us. The door behind you  "
                                "leads to our living area.     "

                                "We have plenty of food and    "
                                "space.                        "
                                "I'm so sorry for your loss.";

const unsigned char* leveCompleteText = 
                                "Barney moved on to a new      "
                                "dimension...";

// NOTE: This uses tempChar1 through tempChar3; the caller must not use these.
void update_player_sprite() {
    // Calculate the position of the player itself, then use these variables to build it up with 4 8x8 NES sprites.
    rawXPosition = (playerXPosition >> PLAYER_POSITION_SHIFT);
    rawYPosition = (playerYPosition >> PLAYER_POSITION_SHIFT);
    rawTileId = PLAYER_SPRITE_TILE_ID + playerDirection;

    if (playerXVelocity != 0 || playerYVelocity != 0) {
        // Does some math with the current NES frame to add either 2 or 0 to the tile id, animating the sprite.
        rawTileId += ((frameCount >> SPRITE_ANIMATION_SPEED_DIVISOR) & 0x01) << 1;
    }
    
    if (playerInvulnerabilityTime && frameCount & PLAYER_INVULNERABILITY_BLINK_MASK) {
        // If the player is invulnerable, we hide their sprite about half the time to do a flicker animation.
        oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, rawTileId, 0x00, PLAYER_SPRITE_INDEX);
        oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, rawTileId + 1, 0x00, PLAYER_SPRITE_INDEX+4);
        oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, rawTileId + 16, 0x00, PLAYER_SPRITE_INDEX+8);
        oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, rawTileId + 17, 0x00, PLAYER_SPRITE_INDEX+12);

    } else {
        oam_spr(rawXPosition, rawYPosition, rawTileId, 0x00, PLAYER_SPRITE_INDEX);
        oam_spr(rawXPosition + NES_SPRITE_WIDTH, rawYPosition, rawTileId + 1, 0x00, PLAYER_SPRITE_INDEX+4);
        oam_spr(rawXPosition, rawYPosition + NES_SPRITE_HEIGHT, rawTileId + 16, 0x00, PLAYER_SPRITE_INDEX+8);
        oam_spr(rawXPosition + NES_SPRITE_WIDTH, rawYPosition + NES_SPRITE_HEIGHT, rawTileId + 17, 0x00, PLAYER_SPRITE_INDEX+12);
    }

}

void handle_player_movement() {
    // Using a variable, so we can change the velocity based on pressing a button, having a special item,
    // or whatever you like!
    int maxVelocity = PLAYER_MAX_VELOCITY;
    lastControllerState = controllerState;
    controllerState = pad_poll(0);

    // If Start is pressed now, and was not pressed before...
    if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
        gameState = GAME_STATE_PAUSED;
        return;
    }
    if (playerControlsLockTime) {
        // If your controls are locked, just tick down the timer until they stop being locked. Don't read player input.
        playerControlsLockTime--;
    } else {
        if (controllerState & PAD_RIGHT && playerXVelocity >= 0) {
            // If you're moving right, and you're not at max, speed up.
            if (playerXVelocity < maxVelocity) {
                playerXVelocity += PLAYER_VELOCITY_ACCEL;
            // If you're over max somehow, we'll slow you down a little.
            } else if (playerXVelocity > maxVelocity) {
                playerXVelocity -= PLAYER_VELOCITY_ACCEL;
            }
        } else if (controllerState & PAD_LEFT && playerXVelocity <= 0) {
            // If you're moving left, and you're not at max, speed up.
            if (ABS(playerXVelocity) < maxVelocity) {
                playerXVelocity -= PLAYER_VELOCITY_ACCEL;
            // If you're over max, slow you down a little...
            } else if (ABS(playerXVelocity) > maxVelocity) { 
                playerXVelocity += PLAYER_VELOCITY_ACCEL;
            }
        } else if (playerXVelocity != 0) {
            // Not pressing anything? Let's slow you back down...
            if (playerXVelocity > 0) {
                playerXVelocity -= PLAYER_VELOCITY_ACCEL;
            } else {
                playerXVelocity += PLAYER_VELOCITY_ACCEL;
            }
        }

        if (controllerState & PAD_UP && playerYVelocity <= 0) {
            if (ABS(playerYVelocity) < maxVelocity) {
                playerYVelocity -= PLAYER_VELOCITY_ACCEL;
            } else if (ABS(playerYVelocity) > maxVelocity) {
                playerYVelocity += PLAYER_VELOCITY_ACCEL;
            }
        } else if (controllerState & PAD_DOWN && playerYVelocity >= 0) {
            if (playerYVelocity < maxVelocity) {
                playerYVelocity += PLAYER_VELOCITY_ACCEL;
            } else if (playerYVelocity > maxVelocity) {
                playerYVelocity -= PLAYER_VELOCITY_ACCEL;
            }
        } else { 
            if (playerYVelocity > 0) {
                playerYVelocity -= PLAYER_VELOCITY_ACCEL;
            } else if (playerYVelocity < 0) {
                playerYVelocity += PLAYER_VELOCITY_ACCEL;
            }
        }
    }

    // While we're at it, tick down the invulnerability timer if needed
    if (playerInvulnerabilityTime) {
        playerInvulnerabilityTime--;
    }

    // This will knock out the player's speed if they hit anything.
    test_player_tile_collision();
    handle_player_sprite_collision();

    rawXPosition = (playerXPosition >> PLAYER_POSITION_SHIFT);
    rawYPosition = (playerYPosition >> PLAYER_POSITION_SHIFT);
        if (rawXPosition > SCREEN_EDGE_RIGHT) {
        // We use sprite direction to determine which direction to scroll in, so be sure this is set properly.
        if (playerInvulnerabilityTime) {
            playerXPosition -= playerXVelocity;
            rawXPosition = (playerXPosition >> PLAYER_POSITION_SHIFT);
        } else {
            playerDirection = SPRITE_DIRECTION_RIGHT;
            gameState = GAME_STATE_SCREEN_SCROLL;
            playerOverworldPosition++;
        }
    } else if (rawXPosition < SCREEN_EDGE_LEFT) {
        if (playerInvulnerabilityTime) {
            playerXPosition -= playerXVelocity;
            rawXPosition = (playerXPosition >> PLAYER_POSITION_SHIFT);
        } else {
            playerDirection = SPRITE_DIRECTION_LEFT;
            gameState = GAME_STATE_SCREEN_SCROLL;
            playerOverworldPosition--;
        }
    } else if (rawYPosition > SCREEN_EDGE_BOTTOM) {
        if (playerInvulnerabilityTime) {
            playerYPosition -= playerYVelocity;
            rawYPosition = (playerYPosition >> PLAYER_POSITION_SHIFT);
        } else {
            playerDirection = SPRITE_DIRECTION_DOWN;
            gameState = GAME_STATE_SCREEN_SCROLL;
            playerOverworldPosition += 8;
        }
    } else if (rawYPosition < SCREEN_EDGE_TOP) {
        if (playerInvulnerabilityTime) {
            playerYPosition -= playerYVelocity;
            rawYPosition = (playerYPosition >> PLAYER_POSITION_SHIFT);
        } else {
            playerDirection = SPRITE_DIRECTION_UP;
            gameState = GAME_STATE_SCREEN_SCROLL;
            playerOverworldPosition -= 8;
        }
    }

}

void test_player_tile_collision() {

	if (playerYVelocity != 0) {
        collisionTempYInt = playerYPosition + PLAYER_Y_OFFSET_EXTENDED + playerYVelocity;
        collisionTempXInt = playerXPosition + PLAYER_X_OFFSET_EXTENDED;

		collisionTempY = ((collisionTempYInt) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
		collisionTempX = ((collisionTempXInt) >> PLAYER_POSITION_SHIFT);

        collisionTempYInt += PLAYER_HEIGHT_EXTENDED;
        collisionTempXInt += PLAYER_WIDTH_EXTENDED;

        collisionTempYBottom = ((collisionTempYInt) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
        collisionTempXRight = (collisionTempXInt) >> PLAYER_POSITION_SHIFT;
        
        // Due to how we are calculating the sprite's position, there is a slight chance we can exceed the area that our
        // map takes up near screen edges. To compensate for this, we clamp the Y position of our character to the 
        // window between 0 and 192 pixels, which we can safely test collision within.

        // If collisionTempY is > 240, it can be assumed we calculated a position less than zero, and rolled over to 255
        if (collisionTempY > 240) {
            collisionTempY = 0;
        }
        // The lowest spot we can test collision is at pixel 192 (the 12th 16x16 tile). If we're past that, bump ourselves back.
        if (collisionTempYBottom > 190) { 
            collisionTempYBottom = 190;
        }

		if (playerYVelocity < 0) {
            // We're going up - test the top left, and top right
			if (test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempY)], 1) || test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempXRight, collisionTempY)], 1)) {
                playerYVelocity = 0;
                playerControlsLockTime = 0;
            }
            if (!playerControlsLockTime) {
                playerDirection = SPRITE_DIRECTION_UP;
            }
		} else {
            // Okay, we're going down - test the bottom left and bottom right
			if (test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempYBottom)], 1) || test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempXRight, collisionTempYBottom)], 1)) {
                playerYVelocity = 0;
                playerControlsLockTime = 0;

			}
            if (!playerControlsLockTime) {
                playerDirection = SPRITE_DIRECTION_DOWN;
            }
		}
    }

	if (playerXVelocity != 0) {
        collisionTempXInt = playerXPosition + PLAYER_X_OFFSET_EXTENDED + playerXVelocity;
        collisionTempYInt = playerYPosition + PLAYER_Y_OFFSET_EXTENDED + playerYVelocity;
		collisionTempX = (collisionTempXInt) >> PLAYER_POSITION_SHIFT;
		collisionTempY = ((collisionTempYInt) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;

        collisionTempXInt += PLAYER_WIDTH_EXTENDED;
        collisionTempYInt += PLAYER_HEIGHT_EXTENDED;

        collisionTempYBottom = ((collisionTempYInt) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
        collisionTempXRight = ((collisionTempXInt) >> PLAYER_POSITION_SHIFT);


        // Depending on how far to the left/right the player is, there's a chance part of our bounding box falls into
        // the next column of tiles on the opposite side of the screen. This if statement prevents those collisions.
        if (collisionTempX > 2 && collisionTempX < 238) {
            if (playerXVelocity < 0) {
                // Okay, we're moving left. Need to test the top-left and bottom-left
                if (test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempY)], 1) || test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempYBottom)], 1)) {
                    playerXVelocity = 0;
                    playerControlsLockTime = 0;

                }
                if (!playerControlsLockTime) {
                    playerDirection = SPRITE_DIRECTION_LEFT;
                }
            } else {
                // Going right - need to test top-right and bottom-right
                if (test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempXRight, collisionTempY)], 1) || test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempXRight, collisionTempYBottom)], 1)) {
                    playerXVelocity = 0;
                    playerControlsLockTime = 0;

                }
                if (!playerControlsLockTime) {
                    playerDirection = SPRITE_DIRECTION_RIGHT;
                }
            }
        }
	}

    playerXPosition += playerXVelocity;
    playerYPosition += playerYVelocity;

}

#define currentMapSpriteIndex tempChar1
void handle_player_sprite_collision() {
    // We store the last sprite hit when we update the sprites in `map_sprites.c`, so here all we have to do is react to it.
    if (lastPlayerSpriteCollisionId != NO_SPRITE_HIT) {
        currentMapSpriteIndex = lastPlayerSpriteCollisionId<<MAP_SPRITE_DATA_SHIFT;
        switch (currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE]) {
            case SPRITE_TYPE_HEALTH:
                // This if statement ensures that we don't remove hearts if you don't need them yet.
                if (playerHealth < playerMaxHealth) {
                    playerHealth += currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_HEALTH];
                    if (playerHealth > playerMaxHealth) {
                        playerHealth = playerMaxHealth;
                    }
                    // Hide the sprite now that it has been taken.
                    currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE] = SPRITE_TYPE_OFFSCREEN;

                    // Play the heart sound!
                    sfx_play(SFX_HEART, SFX_CHANNEL_3);

                    // Mark the sprite as collected, so we can't get it again.
                    currentMapSpritePersistance[playerOverworldPosition] |= bitToByte[lastPlayerSpriteCollisionId];
                }
                break;
            case SPRITE_TYPE_VACUUM_PART:
                ++playerVacuumCount;
                currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE] = SPRITE_TYPE_OFFSCREEN;

                sfx_play(SFX_VACUUM, SFX_CHANNEL_2);

                // Mark the sprite as collected, so we can't get it again.
                currentMapSpritePersistance[playerOverworldPosition] |= bitToByte[lastPlayerSpriteCollisionId];
                break;
            case SPRITE_TYPE_KEY:
                if (playerKeyCount < MAX_KEY_COUNT) {
                    playerKeyCount++;
                    currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE] = SPRITE_TYPE_OFFSCREEN;

                    sfx_play(SFX_KEY, SFX_CHANNEL_3);

                    // Mark the sprite as collected, so we can't get it again.
                    currentMapSpritePersistance[playerOverworldPosition] |= bitToByte[lastPlayerSpriteCollisionId];
                }
                break;
            case SPRITE_TYPE_REGULAR_ENEMY:
            case SPRITE_TYPE_INVULNERABLE_ENEMY:

                if (playerInvulnerabilityTime) {
                    return;
                }
                playerHealth -= currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_DAMAGE]; 
                // Since playerHealth is unsigned, we need to check for wraparound damage. 
                // NOTE: If something manages to do more than 16 damage at once, this might fail.
                if (playerHealth == 0 || playerHealth > 240) {
                    gameState = GAME_STATE_GAME_OVER;
                    music_stop();
                    sfx_play(SFX_GAMEOVER, SFX_CHANNEL_1);
                    return;
                }
                // Knock the player back
                playerControlsLockTime = PLAYER_DAMAGE_CONTROL_LOCK_TIME;
                playerInvulnerabilityTime = PLAYER_DAMAGE_INVULNERABILITY_TIME;
                if (playerDirection == SPRITE_DIRECTION_LEFT) {
                    // Punt them back in the opposite direction
                    playerXVelocity = PLAYER_MAX_VELOCITY;
                    // Reverse their velocity in the other direction, too.
                    playerYVelocity = 0 - playerYVelocity;
                } else if (playerDirection == SPRITE_DIRECTION_RIGHT) {
                    playerXVelocity = 0-PLAYER_MAX_VELOCITY;
                    playerYVelocity = 0 - playerYVelocity;
                } else if (playerDirection == SPRITE_DIRECTION_DOWN) {
                    playerYVelocity = 0-PLAYER_MAX_VELOCITY;
                    playerXVelocity = 0 - playerXVelocity;
                } else { // Make being thrown downward into a catch-all, in case your direction isn't set or something.
                    playerYVelocity = PLAYER_MAX_VELOCITY;
                    playerXVelocity = 0 - playerXVelocity;
                }
                sfx_play(SFX_HURT, SFX_CHANNEL_2);

                
                break;
            case SPRITE_TYPE_DOOR: 
                // Doors without locks are very simple - they just open! Hide the sprite until the user comes back...
                // note that we intentionally *don't* store this state, so it comes back next time.
                currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE] = SPRITE_TYPE_OFFSCREEN;
                sfx_play(SFX_DOOR, SFX_CHANNEL_4);
                break;
            case SPRITE_TYPE_LOCKED_DOOR:
                // First off, do you have a key? If so, let's just make this go away...
                if (playerKeyCount > 0) {
                    playerKeyCount--;
                    currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE] = SPRITE_TYPE_OFFSCREEN;

                    // Mark the door as gone, so it doesn't come back.
                    currentMapSpritePersistance[playerOverworldPosition] |= bitToByte[lastPlayerSpriteCollisionId];
                    sfx_play(SFX_DOOR, SFX_CHANNEL_4);

                    break;
                }
                // So you don't have a key...
                // Okay, we collided with a door before we calculated the player's movement. After being moved, does the 
                // new player position also collide? If so, stop it. Else, let it go.

                // Calculate position...
                tempSpriteCollisionX = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X + 1]) << 8));
                tempSpriteCollisionY = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y + 1]) << 8));

                // Are we colliding?
                // NOTE: We take a bit of a shortcut here and assume all doors are 16x16 (the hard-coded 16 value below)
                if (
                    playerXPosition < tempSpriteCollisionX + (16 << PLAYER_POSITION_SHIFT) &&
                    playerXPosition + PLAYER_WIDTH_EXTENDED > tempSpriteCollisionX &&
                    playerYPosition < tempSpriteCollisionY + (16 << PLAYER_POSITION_SHIFT) &&
                    playerYPosition + PLAYER_HEIGHT_EXTENDED > tempSpriteCollisionY
                ) {
                    playerXPosition -= playerXVelocity;
                    playerYPosition -= playerYVelocity;
                    playerControlsLockTime = 0;
                }
                break;
            case SPRITE_TYPE_ENDGAME:
                gameState = GAME_STATE_CREDITS;
                break;
            case SPRITE_TYPE_END_MAP:
                ++worldNum;
                trigger_game_text(leveCompleteText);
                gameState = GAME_STATE_LEVEL_COMPLETE;
                music_pause(1);
                sfx_play(SFX_WIN, SFX_CHANNEL_1);
                break;
            case SPRITE_TYPE_NPC:
                // Okay, we collided with this NPC before we calculated the player's movement. After being moved, does the 
                // new player position also collide? If so, stop it. Else, let it go.

                // Calculate position...
                tempSpriteCollisionX = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X + 1]) << 8));
                tempSpriteCollisionY = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y + 1]) << 8));
                // Are we colliding?
                // NOTE: We take a bit of a shortcut here and assume all NPCs are 16x16 (the hard-coded 16 value below)
                if (
                    playerXPosition < tempSpriteCollisionX + (16 << PLAYER_POSITION_SHIFT) &&
                    playerXPosition + PLAYER_WIDTH_EXTENDED > tempSpriteCollisionX &&
                    playerYPosition < tempSpriteCollisionY + (16 << PLAYER_POSITION_SHIFT) &&
                    playerYPosition + PLAYER_HEIGHT_EXTENDED > tempSpriteCollisionY
                ) {
                    playerXPosition -= playerXVelocity;
                    playerYPosition -= playerYVelocity;
                    playerControlsLockTime = 0;
                }
                
                if (controllerState & PAD_A && !(lastControllerState & PAD_A)) {
                    // Show the text for the player on the first screen
                    
                    if (worldNum == 0 && playerOverworldPosition == 8) {
                        trigger_game_text(introCharacter);
                    } else if (worldNum == 0 && playerOverworldPosition == 9) {
                        trigger_game_text(heartCharacter);
                    } else if (worldNum == 1 && playerOverworldPosition == 40) {
                        trigger_game_text(level2Character);
                    } else if (worldNum == 1 && playerOverworldPosition == 41) {
                        trigger_game_text(firstEnemyCharacter);
                    } else if (worldNum == 2 && playerOverworldPosition == 8) {
                        trigger_game_text(runCharacter);
                    } else if (worldNum == 2) {
                        trigger_game_text(vacuumCharacter);
                    } else if (worldNum == 3) {
                        trigger_game_text(keyCharacter);
                    } else if (worldNum == 5) {
                        trigger_game_text(fasterCharacter);
                    } else if (worldNum == 7) {
                        trigger_game_text(labAheadCharacter);
                    } else if (worldNum == 13) {
                        trigger_game_text(labber);
                    } else if (worldNum == 14) {
                        if (playerVacuumCount >= ALL_VACUUMS) {
                            trigger_game_text(allVacuums);
                        } else {
                            trigger_game_text(notAllVacuums);
                        }
                    }
                    /*
                    if (playerOverworldPosition == 0) {
                        trigger_game_text(introductionText);
                    } else {
                        // If it's on another screen, show some different text :)
                        trigger_game_text(movedText);
                    }
                    */
                }
                break;


        }

    }
}
