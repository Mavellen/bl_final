#include "raylib.h"
#include "rcamera.h"
#include "lecs.h"
using namespace ls::lecs;

int WIDTH = 800;
int HEIGHT = 800;
float CAMSPEED = 500.0f;
float RAD = 250;
int ENTITY_AMOUNT = 1000000;
Vector3 ARENA_ORIGIN = {0,0,0};
float ARENA_RAD = 1;
float ARENA_DIM = RAD * 2;
Color ARENA_COL = RAYWHITE;
int SPAWN_RATE = 500;
bool MORPH = false;
bool LARGE_ONLY = false;


extern void oop_condes();
extern void oop_static();
extern void dod_condes();
extern void dod_static();
extern void max_condes();
extern void max_static();

void take_inputs(Camera& camera, const float dt)
{
  if(IsKeyDown(KEY_W)){
    CameraMoveForward(&camera, CAMSPEED * dt, false);
  }
  else if(IsKeyDown(KEY_S)){
    CameraMoveForward(&camera, -CAMSPEED * dt, false);
  }

  if(IsKeyDown(KEY_D)){
    CameraMoveRight(&camera, CAMSPEED * dt, true);
  }
  else if(IsKeyDown(KEY_A)){
    CameraMoveRight(&camera, -CAMSPEED * dt, true);
  }

  if(IsKeyPressed(KEY_ONE))
    MORPH = !MORPH;
  if(IsKeyPressed(KEY_TWO))
    LARGE_ONLY = !LARGE_ONLY;

  CameraYaw(&camera, -GetMouseDelta().x * dt, false);
  CameraPitch(&camera, -GetMouseDelta().y * dt, true, false, false);
}

#undef CONDES
/*
 * Systems
 * - move and draw all entities on screen, regardless of type
 * - change data of specific type
 * - draw only specific type
 * - deconstruct all entites of a type and re-construct them each frame
 *
 *
 */
int main()
{
  InitWindow(WIDTH, HEIGHT, "ECSTEST");
  DisableCursor();
#ifdef CONDES
  //max_condes();
  //oop_condes();
  dod_condes();
#else
  //max_static();
  //oop_static();
  dod_static();
#endif
  CloseWindow();
  return 1;
}
