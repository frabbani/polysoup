#pragma once

#define GRAVITY -10
#define GROUNDSLOPE 45

#define MOVETYPE_SLIDE  0
#define MOVETYPE_BOUNCE 1 //TODO: PolySoup support

#include "map.h"
#include "camera.h"

typedef struct actor_s {
  float radius;
  float yaw;
  float yawspeed;
  float movespeed;
  float jumpaccel;
  float zvel;
  camera_t transform;
  int32 movetype;
} actor_t;

extern int32 actors_pause_movement;

void actor_move( actor_t *actor, float move, float side, float turn, float jump );
