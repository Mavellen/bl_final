#include "raylib.h"
#include "rcamera.h"
#include "lecs.h"
using namespace ls::lecs;

int WIDTH = 800;
int HEIGHT = 800;
float CAMSPEED = 500.0f;
float RAD = 250;

Vector3 ARENA_ORIGIN = {0,0,0};
float ARENA_RAD = 1;
float ARENA_DIM = RAD * 2;
Color ARENA_COL = RAYWHITE;

size_t DIRECTIONS = 100;
auto dirlux = new float[DIRECTIONS];
auto dirluy = new float[DIRECTIONS];
auto dirluz = new float[DIRECTIONS];
auto pdcolors = new Color[DIRECTIONS];

double MAX_FRAMES = 3600;
double avgframetime = 0;


extern void oop_condes();
extern void oop_static();
extern void oop_condes_raw();
extern void oop_static_raw();
extern void dod_condes();
extern void dod_static();
extern void dod_condes_raw();
extern void dod_static_raw();
extern void max_condes();
extern void max_static();
extern void max_condes_raw();
extern void max_static_raw();

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
}

int ENTITY_AMOUNT = 1000000;
int SPAWN_RATE = 500;

int main()
{
  static std::random_device device;
  static std::default_random_engine engine(device());
  static std::uniform_real_distribution<> real_vel_distr(-20, 20);
  static std::uniform_int_distribution<> color_distr(0, UINT32_MAX);

  for(size_t i = 0; i < DIRECTIONS; i++)
  {
    dirlux[i] = (float)real_vel_distr(engine);
    dirluy[i] = (float)real_vel_distr(engine);
    dirluz[i] = (float)real_vel_distr(engine);
    pdcolors[i] = GetColor(color_distr(engine) | 0xff);
  }

  InitWindow(WIDTH, HEIGHT, "ECSTEST");
  DisableCursor();
  //max_condes_raw();
  //oop_condes_raw();
  //dod_condes_raw();

  //max_condes();
  //oop_condes();
  //dod_condes();

  //max_static_raw();
  //oop_static_raw();
  dod_static_raw();

  //max_static();
  //oop_static();
  //dod_static();

  CloseWindow();
  printf("AVGFT: %f", avgframetime);
  return 1;
}
