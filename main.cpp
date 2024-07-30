#include "raylib.h"
#include "rcamera.h"
#include "lecs.h"
using namespace ls::lecs;

int WIDTH = 800;
int HEIGHT = 800;
float CAMSPEED = 500.0f;
float RAD = 250;
int ENTITY_AMOUNT = 10000;
Vector3 ARENA_ORIGIN = {0,0,0};
float ARENA_RAD = 1;
float ARENA_DIM = RAD * 2;
Color ARENA_COL = RAYWHITE;


extern void oop_main();
extern void dod_main();

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

  CameraYaw(&camera, -GetMouseDelta().x * dt, false);
  CameraPitch(&camera, -GetMouseDelta().y * dt, true, false, false);

  if(IsKeyPressed(KEY_ONE))
  {
    CAMSPEED += 50.0f;
  }
  if(IsKeyPressed(KEY_TWO))
  {
    CAMSPEED -= 50.0f;
  }
}

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
  InitWindow(WIDTH, HEIGHT, "ECS");
  DisableCursor();
  //oop_main();
  dod_main();
  return 1;
}
