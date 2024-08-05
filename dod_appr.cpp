#include "raylib.h"
#include "raymath.h"
#include "random"

extern float CAMSPEED;
extern float RAD;
extern int ENTITY_AMOUNT;
extern Vector3 ARENA_ORIGIN;
extern float ARENA_RAD;
extern float ARENA_DIM;
extern Color ARENA_COL;
extern int SPAWN_RATE;

extern size_t DIRECTIONS;
extern float* dirlux;
extern float* dirluy;
extern float* dirluz;
extern Color* pdcolors;
extern double avgframetime;
extern double MAX_FRAMES;
extern void take_inputs(Camera& camera, float dt);

#include "lecs.h"
using namespace ls::lecs;

struct position { Vector3 p; };
struct velocity { Vector3 v; };
struct cube_dim {float w,h,l; };

void in_bounds_condes(const ecsid* entity, size_t size, std::tuple<sim*>& tup, position* positions)
{
  std::vector<ecsid> r {};
  r.reserve(size/4);
  for(size_t k = 0; k < size; k++)
  {
    if(positions[k].p.x <= -RAD || positions[k].p.x >= RAD ||
           positions[k].p.y <= -RAD || positions[k].p.y >= RAD ||
           positions[k].p.z <= -RAD || positions[k].p.z >= RAD)
    {
      r.push_back(entity[k]);
    }
  }
  sim* s = std::get<0>(tup);
  for(const ecsid id : r)
  {
    s->erase(id);
  }
}
void in_bounds_static(const ecsid* entity, size_t size, position* positions, velocity* velocities)
{
  for(size_t k = 0; k < size; k++)
  {
    if(positions[k].p.x <= -RAD || positions[k].p.x >= RAD) velocities[k].v.x *= -1;
    if(positions[k].p.y <= -RAD || positions[k].p.y >= RAD) velocities[k].v.y *= -1;
    if(positions[k].p.z <= -RAD || positions[k].p.z >= RAD) velocities[k].v.z *= -1;
  }
}
void move(const ecsid* entity, size_t size, std::tuple<float>& tuple, position* p, velocity* v)
{
  float dt = std::get<0>(tuple);
  for(size_t i = 0; i < size; i++)
  {
    p[i].p = Vector3Add(p[i].p, Vector3Scale(v[i].v, dt));
  }
}
void draw(const ecsid* entity, size_t size, position* p, cube_dim* cd, Color* c)
{
  for(size_t i = 0; i < size; i++)
  {
    DrawCube(p[i].p, cd[i].w, cd[i].h, cd[i].l, c[i]);
  }
}
void morph_color(const ecsid* entity, size_t size, Color* c)
{
  static std::random_device device;
  static std::default_random_engine engine(device());
  static std::uniform_int_distribution<> color_distr(0, UINT32_MAX);
  Color col = GetColor(color_distr(engine)| 0xff);
  for(size_t k = 0; k < size; k++)
  {
    c[k] = col;
  }
}
void input_func(ecsid entity, std::tuple<float>& tuple, Camera* c)
{
  take_inputs(*c, std::get<0>(tuple));
};

void dod_condes_raw()
{
  sim s;

  const auto in_bounds_q = s.query();
  in_bounds_q->all_of<position>();
  in_bounds_q->update();

  const auto mov_q = s.query();
  mov_q->all_of<position,velocity>();
  mov_q->update();

  auto sim_tuple = std::make_tuple(&s);

  double iterations = 0;
  double total_frametime = 0;

  float combined = 0;
  int num_of_spawns = 0;
  while(true)
  {
    if(IsKeyDown(KEY_Q)) break;
    ++iterations;
    const float dt = GetFrameTime();
    combined += dt;
    total_frametime += dt;

    auto tuple = std::make_tuple(dt);

    in_bounds_q->batch(in_bounds_condes, sim_tuple);
    mov_q->batch(move, tuple);
    if(num_of_spawns < 10)
    {
      for(size_t k = 0; k < SPAWN_RATE; k++)
      {
        const ecsid id = s.entity();
        s.add<position>(id, (Vector3){0, 0, 0});
        s.add<velocity>(id, (Vector3){dirlux[id % DIRECTIONS],
        dirluy[id % DIRECTIONS],
        dirluz[id % DIRECTIONS]});
        const auto& [r, g, b, a] = pdcolors[id % DIRECTIONS];
        s.add<Color>(id, r, g, b, a);
        s.add<cube_dim>(id, 10, 10, 10);
      }
      ++num_of_spawns;
    }
    if(combined >= 1)
    {
      num_of_spawns=0;
      combined=0;
    }
    ClearBackground(BLACK);
    BeginDrawing();
    DrawText(TextFormat("FPS: %i", GetFPS()), 0, 0, 20, RED);
    DrawText(TextFormat("Cubes: %i", s.alive_entites()), 0, 50, 20, RED);
    EndDrawing();
    if(iterations >= MAX_FRAMES) break;
  }
  avgframetime = total_frametime / iterations;
}

void dod_static_raw()
{
  sim s;

  for(size_t i = 0; i < ENTITY_AMOUNT; i++)
  {
    const ecsid id = s.entity();
    s.add<position>(id, (Vector3){0, 0, 0});
    s.add<velocity>(id, (Vector3){dirlux[id % DIRECTIONS],
    dirluy[id % DIRECTIONS],
    dirluz[id % DIRECTIONS]});
    const auto& [r, g, b, a] = pdcolors[id % DIRECTIONS];
    s.add<Color>(id, r, g, b, a);
    s.add<cube_dim>(id, 10, 10, 10);
  }

  const auto in_bounds_all_q = s.query();
  in_bounds_all_q->all_of<position,velocity>();
  in_bounds_all_q->update();

  const auto mov_all_q = s.query();
  mov_all_q->all_of<position,velocity>();
  mov_all_q->update();

  double iterations = 0;
  double total_frametime = 0;

  while(true)
  {
    if(IsKeyDown(KEY_Q)) break;
    ++iterations;
    const float dt = GetFrameTime();
    total_frametime += dt;
    auto tuple = std::make_tuple(dt);
    in_bounds_all_q->batch(in_bounds_static);
    mov_all_q->batch(move, tuple);
    ClearBackground(BLACK);
    BeginDrawing();
    DrawText(TextFormat("FPS: %i", GetFPS()), 0, 0, 20, RED);
    DrawText(TextFormat("Cubes: %i", s.alive_entites()), 0, 50, 20, RED);
    EndDrawing();
    if(iterations >= MAX_FRAMES) break;
  }
  avgframetime = total_frametime / iterations;
}

void dod_condes()
{
  sim s;

  const ecsid camera = s.entity();
  s.add<Camera>(camera, Vector3{ .0f, 5.0f, -600.0f},
    Vector3{.0f, 5.0f, .0f}, Vector3{.0f, 1.0f, .0f}, 60, CAMERA_PERSPECTIVE);
  auto cam = s.get<Camera>(camera);

  const auto in_bounds_q = s.query();
  in_bounds_q->all_of<position>();
  in_bounds_q->update();

  const auto mov_q = s.query();
  mov_q->all_of<position,velocity>();
  mov_q->update();

  const auto draw_q = s.query();
  draw_q->all_of<position,cube_dim,Color>();
  draw_q->update();

  const auto cam_q = s.query();
  cam_q->all_of<Camera>();
  cam_q->update();

  auto sim_tuple = std::make_tuple(&s);

  double iterations = 0;
  double total_frametime = 0;

  float combined = 0;
  int num_of_spawns = 0;
  while(!WindowShouldClose())
  {
    ClearBackground(BLACK);
    ++iterations;
    const float dt = GetFrameTime();
    combined += dt;
    total_frametime += dt;
    auto tuple = std::make_tuple(dt);
    BeginDrawing();
    BeginMode3D(*cam);
    {
      DrawCubeWires(ARENA_ORIGIN, ARENA_DIM, ARENA_DIM, ARENA_DIM, ARENA_COL);
      cam_q->each(input_func, tuple);
      in_bounds_q->batch(in_bounds_condes, sim_tuple);
      mov_q->batch(move, tuple);
      draw_q->batch(draw);
      if(num_of_spawns < 10)
      {
        for(size_t k = 0; k < SPAWN_RATE; k++)
        {
          const ecsid id = s.entity();
          s.add<position>(id, (Vector3){0, 0, 0});
          s.add<velocity>(id, (Vector3){dirlux[id % DIRECTIONS],
          dirluy[id % DIRECTIONS],
          dirluz[id % DIRECTIONS]});
          const auto& [r, g, b, a] = pdcolors[id % DIRECTIONS];
          s.add<Color>(id, r, g, b, a);
          s.add<cube_dim>(id, 10, 10, 10);
        }
        ++num_of_spawns;
      }
      if(combined >= 1)
      {
        num_of_spawns=0;
        combined=0;
      }
    }
    EndMode3D();
    DrawText(TextFormat("FPS: %i", GetFPS()), 0, 0, 20, RED);
    DrawText(TextFormat("Cubes: %i", s.alive_entites()-1), 0, 50, 20, RED);
    EndDrawing();
    if(iterations >= MAX_FRAMES) break;
  }
  avgframetime = total_frametime / iterations;
}

void dod_static()
{
  sim s;

  const ecsid camera = s.entity();
  s.add<Camera>(camera, Vector3{ .0f, 5.0f, -600.0f},
    Vector3{.0f, 5.0f, .0f}, Vector3{.0f, 1.0f, .0f}, 60, CAMERA_PERSPECTIVE);
  auto cam = s.get<Camera>(camera);

  for(size_t i = 0; i < ENTITY_AMOUNT; i++)
  {
    const ecsid id = s.entity();
    s.add<position>(id, (Vector3){0, 0, 0});
    s.add<velocity>(id, (Vector3){dirlux[id % DIRECTIONS],
    dirluy[id % DIRECTIONS],
    dirluz[id % DIRECTIONS]});
    const auto& [r, g, b, a] = pdcolors[id % DIRECTIONS];
    s.add<Color>(id, r, g, b, a);
    s.add<cube_dim>(id, 10, 10, 10);
  }

  const auto in_bounds_all_q = s.query();
  in_bounds_all_q->all_of<position,velocity>();
  in_bounds_all_q->update();

  const auto mov_all_q = s.query();
  mov_all_q->all_of<position,velocity>();
  mov_all_q->update();

  const auto draw_all_q = s.query();
  draw_all_q->all_of<position,cube_dim,Color>();
  draw_all_q->update();

  const auto cam_q = s.query();
  cam_q->all_of<Camera>();
  cam_q->update();

  double iterations = 0;
  double total_frametime = 0;

  while(!WindowShouldClose())
  {
    ClearBackground(BLACK);
    ++iterations;
    const float dt = GetFrameTime();
    total_frametime += dt;
    auto tuple = std::make_tuple(dt);
    BeginDrawing();
    BeginMode3D(*cam);
    {
      DrawCubeWires(ARENA_ORIGIN, ARENA_DIM, ARENA_DIM, ARENA_DIM, ARENA_COL);
      cam_q->each(input_func, tuple);
      in_bounds_all_q->batch(in_bounds_static);
      mov_all_q->batch(move, tuple);
      draw_all_q->batch(draw);
    }
    EndMode3D();
    DrawText(TextFormat("FPS: %i", GetFPS()), 0, 0, 20, RED);
    DrawText(TextFormat("Cubes: %i", s.alive_entites()-1), 0, 50, 20, RED);
    EndDrawing();
    if(iterations >= MAX_FRAMES) break;
  }
  avgframetime = total_frametime / iterations;
}