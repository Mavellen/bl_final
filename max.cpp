#include <complex>
#include <random>
#include <valarray>

#include "raylib.h"
#include "raymath.h"
#include "../../stuff/clion/CLion 2024.1.2/bin/mingw/x86_64-w64-mingw32/include/stdlib.h"

extern float CAMSPEED;
extern float RAD;
extern int ENTITY_AMOUNT;
extern Vector3 ARENA_ORIGIN;
extern float ARENA_RAD;
extern float ARENA_DIM;
extern Color ARENA_COL;
extern int SPAWN_RATE;
extern bool MORPH;
extern bool LARGE_ONLY;
extern void take_inputs(Camera& camera, float dt);
extern void take_inputs();

void max_condes_raw()
{
  Vector3* positions = (Vector3*)malloc(sizeof(Vector3) * ENTITY_AMOUNT);
  Vector3* velocities = (Vector3*)malloc(sizeof(Vector3) * ENTITY_AMOUNT);
  Color* colors = (Color*)malloc(sizeof(Color) * ENTITY_AMOUNT);
  float* widths = (float*)malloc(sizeof(float) * ENTITY_AMOUNT);
  float* heights = (float*)malloc(sizeof(float) * ENTITY_AMOUNT);
  float* lengths = (float*)malloc(sizeof(float) * ENTITY_AMOUNT);
  size_t* dense = (size_t*)malloc(sizeof(size_t) * ENTITY_AMOUNT);
  size_t component_amount = 0;

  size_t* sparse = (size_t*)malloc(sizeof(size_t) * ENTITY_AMOUNT);
  size_t head = 100;
  size_t count = 0;

  size_t* recvec = (size_t*)malloc(sizeof(size_t) * ENTITY_AMOUNT);
  size_t entity_count = 0;

  static std::random_device device;
  static std::default_random_engine engine(device());
  static std::uniform_real_distribution<> real_vel_distr(-20, 20);
  static std::uniform_int_distribution<> color_distr(0, UINT32_MAX);

  float combined = 0;
  int num_of_spawns = 0;
  while(true)
  {
    if(IsKeyDown(KEY_Q)) break;
    const float dt = GetFrameTime();
    combined += dt;
    for(size_t k = 0; k < component_amount; k++)
      {
        if(positions[k].x <= -RAD || positions[k].x >= RAD ||
           positions[k].y <= -RAD || positions[k].y >= RAD ||
           positions[k].z <= -RAD || positions[k].z >= RAD)
        {
          const size_t last = component_amount-1;
          const size_t removed_entity = dense[sparse[k]];
          const size_t moved_entity = dense[last];
          dense[k] = moved_entity;
          sparse[moved_entity] = k;
          positions[k] = positions[last];
          velocities[k] = velocities[last];
          colors[k] = colors[last];
          heights[k] = heights[last];
          widths[k] = widths[last];
          lengths[k] = lengths[last];
          --component_amount;

          recvec[removed_entity] = head;
          head = removed_entity;
          ++count;
        }
      }
    for(size_t k = 0; k < component_amount; k++)
    {
      positions[k] = Vector3Add(positions[k], Vector3Scale(velocities[k], dt));
    }
    if(num_of_spawns < 10)
    {
      for(size_t k = 0; k < SPAWN_RATE; k++)
      {
        const auto dx = (float)real_vel_distr(engine);
        const auto dy = (float)real_vel_distr(engine);
        const auto dz = (float)real_vel_distr(engine);
        const Color c = GetColor(color_distr(engine)| 0xff);
        size_t id;

        if(count > 0)
        {
          id = head;
          head = recvec[head];
          --count;
        }
        else
        {
          if(entity_count == ENTITY_AMOUNT) break;;
          id = entity_count;
          recvec[entity_count] = entity_count;
          ++entity_count;
        }
        sparse[id] = component_amount;
        dense[component_amount] = id;
        positions[component_amount] = {0,0,0};
        velocities[component_amount] = {dx,dy,dz};
        colors[component_amount] = c;
        widths[component_amount] = 10;
        heights[component_amount] = 10;
        lengths[component_amount] = 10;
        ++component_amount;
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
    DrawText(TextFormat("Cubes: %i", entity_count-count), 0, 50, 20, RED);
    EndDrawing();
  }
}

void max_static_raw()
{
  Vector3* positions_small = (Vector3*)malloc(sizeof(Vector3) * ENTITY_AMOUNT / 2);
  Vector3* velocities_small = (Vector3*)malloc(sizeof(Vector3) * ENTITY_AMOUNT / 2);
  Color* colors_small = (Color*)malloc(sizeof(Color) * ENTITY_AMOUNT / 2);
  float* widths_small = (float*)malloc(sizeof(float) * ENTITY_AMOUNT / 2);
  float* heights_small = (float*)malloc(sizeof(float) * ENTITY_AMOUNT / 2);
  float* lengths_small = (float*)malloc(sizeof(float) * ENTITY_AMOUNT / 2);
  Vector3* positions_large = (Vector3*)malloc(sizeof(Vector3) * ENTITY_AMOUNT / 2);
  Vector3* velocities_large = (Vector3*)malloc(sizeof(Vector3) * ENTITY_AMOUNT / 2);
  Color* colors_large = (Color*)malloc(sizeof(Color) * ENTITY_AMOUNT / 2);
  float* widths_large = (float*)malloc(sizeof(float) * ENTITY_AMOUNT / 2);
  float* heights_large = (float*)malloc(sizeof(float) * ENTITY_AMOUNT / 2);
  float* lengths_large = (float*)malloc(sizeof(float) * ENTITY_AMOUNT / 2);
  static std::random_device device;
  static std::default_random_engine engine(device());
  static std::uniform_real_distribution<> real_vel_distr(-20, 20);
  static std::uniform_int_distribution<> color_distr(0, UINT32_MAX);
  for(size_t i = 0; i < ENTITY_AMOUNT / 2; i++)
  {
    const auto dx = (float)real_vel_distr(engine);
    const auto dy = (float)real_vel_distr(engine);
    const auto dz = (float)real_vel_distr(engine);
    const Color c = GetColor(color_distr(engine)| 0xff);
    positions_small[i] = {0,0,0};
    velocities_small[i] = {dx,dy,dz};
    colors_small[i] = c;
    widths_small[i] = 5;
    heights_small[i] = 5;
    lengths_small[i] = 5;
  }
  for(size_t i = 0; i < ENTITY_AMOUNT / 2; i++)
  {
    const auto dx = (float)real_vel_distr(engine);
    const auto dy = (float)real_vel_distr(engine);
    const auto dz = (float)real_vel_distr(engine);
    const Color c = GetColor(color_distr(engine)| 0xff);
    positions_large[i] = {0,0,0};
    velocities_large[i] = {dx,dy,dz};
    colors_large[i] = c;
    widths_large[i] = 10;
    heights_large[i] = 10;
    lengths_large[i] = 10;
  }
  while(true)
  {
    take_inputs();
    if(IsKeyDown(KEY_Q)) break;
    const float dt = GetFrameTime();
    if(!LARGE_ONLY)
      for(size_t k = 0; k < ENTITY_AMOUNT / 2; k++)
      {
        if(positions_small[k].x <= -RAD || positions_small[k].x >= RAD) velocities_small[k].x *= -1;
        if(positions_small[k].y <= -RAD || positions_small[k].y >= RAD) velocities_small[k].y *= -1;
        if(positions_small[k].z <= -RAD || positions_small[k].z >= RAD) velocities_small[k].z *= -1;
        positions_small[k] = Vector3Add(positions_small[k], Vector3Scale(velocities_small[k], dt));
      }
    for(size_t k = 0; k < ENTITY_AMOUNT / 2; k++)
    {
      if(positions_large[k].x <= -RAD || positions_large[k].x >= RAD) velocities_large[k].x *= -1;
      if(positions_large[k].y <= -RAD || positions_large[k].y >= RAD) velocities_large[k].y *= -1;
      if(positions_large[k].z <= -RAD || positions_large[k].z >= RAD) velocities_large[k].z *= -1;
      positions_large[k] = Vector3Add(positions_large[k], Vector3Scale(velocities_large[k], dt));
    }
    if(MORPH)
    {
      const Color c = GetColor(color_distr(engine)| 0xff);
      for(size_t k = 0; k < ENTITY_AMOUNT / 2; k++)
      {
        colors_large[k] = c;
      }
    }
    ClearBackground(BLACK);
    BeginDrawing();
    DrawText(TextFormat("FPS: %i", GetFPS()), 0, 0, 20, RED);
    DrawText(TextFormat("Cubes: %i", ENTITY_AMOUNT), 0, 50, 20, RED);
    DrawText(TextFormat("Morph: %i", MORPH ? 1 : 0), 0, 100, 20, RED);
    DrawText(TextFormat("Large Only %i", LARGE_ONLY ? 1 : 0), 0, 150, 20, RED);
    EndDrawing();
  }
}

void max_condes()
{
  Vector3* positions = (Vector3*)malloc(sizeof(Vector3) * ENTITY_AMOUNT);
  Vector3* velocities = (Vector3*)malloc(sizeof(Vector3) * ENTITY_AMOUNT);
  Color* colors = (Color*)malloc(sizeof(Color) * ENTITY_AMOUNT);
  float* widths = (float*)malloc(sizeof(float) * ENTITY_AMOUNT);
  float* heights = (float*)malloc(sizeof(float) * ENTITY_AMOUNT);
  float* lengths = (float*)malloc(sizeof(float) * ENTITY_AMOUNT);
  size_t* dense = (size_t*)malloc(sizeof(size_t) * ENTITY_AMOUNT);
  size_t component_amount = 0;

  size_t* sparse = (size_t*)malloc(sizeof(size_t) * ENTITY_AMOUNT);
  size_t head = 100;
  size_t count = 0;

  size_t* recvec = (size_t*)malloc(sizeof(size_t) * ENTITY_AMOUNT);
  size_t entity_count = 0;

  Camera camera {
          { .0f, 5.0f, -600.0f},
          { .0f, 5.0f, .0f},
          { .0f, 1.0f, .0f},
          60,
          CAMERA_PERSPECTIVE
        };
  static std::random_device device;
  static std::default_random_engine engine(device());
  static std::uniform_real_distribution<> real_vel_distr(-20, 20);
  static std::uniform_int_distribution<> color_distr(0, UINT32_MAX);

  float combined = 0;
  int num_of_spawns = 0;
  while(!WindowShouldClose())
  {
    ClearBackground(BLACK);
    const float dt = GetFrameTime();
    combined += dt;

    BeginDrawing();
    BeginMode3D(camera);
    {
      DrawCubeWires(ARENA_ORIGIN, ARENA_DIM, ARENA_DIM, ARENA_DIM, ARENA_COL);
      take_inputs(camera, dt);
      for(size_t k = 0; k < component_amount; k++)
      {
        if(positions[k].x <= -RAD || positions[k].x >= RAD ||
           positions[k].y <= -RAD || positions[k].y >= RAD ||
           positions[k].z <= -RAD || positions[k].z >= RAD)
        {
          const size_t last = component_amount-1;
          const size_t removed_entity = dense[sparse[k]];
          const size_t moved_entity = dense[last];
          dense[k] = moved_entity;
          sparse[moved_entity] = k;
          positions[k] = positions[last];
          velocities[k] = velocities[last];
          colors[k] = colors[last];
          heights[k] = heights[last];
          widths[k] = widths[last];
          lengths[k] = lengths[last];
          --component_amount;

          recvec[removed_entity] = head;
          head = removed_entity;
          ++count;
        }
      }
      for(size_t k = 0; k < component_amount; k++)
      {
        positions[k] = Vector3Add(positions[k], Vector3Scale(velocities[k], dt));
      }
      for(size_t k = 0; k < component_amount; k++)
      {
        DrawCube(positions[k], widths[k], heights[k], lengths[k], colors[k]);
      }
      if(num_of_spawns < 10)
      {
        for(size_t k = 0; k < SPAWN_RATE; k++)
        {
          const auto dx = (float)real_vel_distr(engine);
          const auto dy = (float)real_vel_distr(engine);
          const auto dz = (float)real_vel_distr(engine);
          const Color c = GetColor(color_distr(engine)| 0xff);
          size_t id;

          if(count > 0)
          {
            id = head;
            head = recvec[head];
            --count;
          }
          else
          {
            if(entity_count == ENTITY_AMOUNT) break;;
            id = entity_count;
            recvec[entity_count] = entity_count;
            ++entity_count;
          }
          sparse[id] = component_amount;
          dense[component_amount] = id;
          positions[component_amount] = {0,0,0};
          velocities[component_amount] = {dx,dy,dz};
          colors[component_amount] = c;
          widths[component_amount] = 10;
          heights[component_amount] = 10;
          lengths[component_amount] = 10;
          ++component_amount;
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
    DrawText(TextFormat("Cubes: %i", entity_count-count), 0, 50, 20, RED);
    EndDrawing();
  }
}

void max_static()
{
  Vector3* positions_small = (Vector3*)malloc(sizeof(Vector3) * ENTITY_AMOUNT / 2);
  Vector3* velocities_small = (Vector3*)malloc(sizeof(Vector3) * ENTITY_AMOUNT / 2);
  Color* colors_small = (Color*)malloc(sizeof(Color) * ENTITY_AMOUNT / 2);
  float* widths_small = (float*)malloc(sizeof(float) * ENTITY_AMOUNT / 2);
  float* heights_small = (float*)malloc(sizeof(float) * ENTITY_AMOUNT / 2);
  float* lengths_small = (float*)malloc(sizeof(float) * ENTITY_AMOUNT / 2);
  Vector3* positions_large = (Vector3*)malloc(sizeof(Vector3) * ENTITY_AMOUNT / 2);
  Vector3* velocities_large = (Vector3*)malloc(sizeof(Vector3) * ENTITY_AMOUNT / 2);
  Color* colors_large = (Color*)malloc(sizeof(Color) * ENTITY_AMOUNT / 2);
  float* widths_large = (float*)malloc(sizeof(float) * ENTITY_AMOUNT / 2);
  float* heights_large = (float*)malloc(sizeof(float) * ENTITY_AMOUNT / 2);
  float* lengths_large = (float*)malloc(sizeof(float) * ENTITY_AMOUNT / 2);

  Camera camera {
            { .0f, 5.0f, -600.0f},
            { .0f, 5.0f, .0f},
            { .0f, 1.0f, .0f},
            60,
            CAMERA_PERSPECTIVE
          };
  static std::random_device device;
  static std::default_random_engine engine(device());
  static std::uniform_real_distribution<> real_vel_distr(-20, 20);
  static std::uniform_int_distribution<> color_distr(0, UINT32_MAX);


  for(size_t i = 0; i < ENTITY_AMOUNT / 2; i++)
  {
    const auto dx = (float)real_vel_distr(engine);
    const auto dy = (float)real_vel_distr(engine);
    const auto dz = (float)real_vel_distr(engine);
    const Color c = GetColor(color_distr(engine)| 0xff);
    positions_small[i] = {0,0,0};
    velocities_small[i] = {dx,dy,dz};
    colors_small[i] = c;
    widths_small[i] = 5;
    heights_small[i] = 5;
    lengths_small[i] = 5;
  }
  for(size_t i = 0; i < ENTITY_AMOUNT / 2; i++)
  {
    const auto dx = (float)real_vel_distr(engine);
    const auto dy = (float)real_vel_distr(engine);
    const auto dz = (float)real_vel_distr(engine);
    const Color c = GetColor(color_distr(engine)| 0xff);
    positions_large[i] = {0,0,0};
    velocities_large[i] = {dx,dy,dz};
    colors_large[i] = c;
    widths_large[i] = 10;
    heights_large[i] = 10;
    lengths_large[i] = 10;
  }

  while(!WindowShouldClose())
  {
    ClearBackground(BLACK);
    const float dt = GetFrameTime();
    BeginDrawing();
    BeginMode3D(camera);
    take_inputs(camera, dt);
    DrawCubeWires(ARENA_ORIGIN, ARENA_DIM, ARENA_DIM, ARENA_DIM, ARENA_COL);
    if(!LARGE_ONLY)
      for(size_t k = 0; k < ENTITY_AMOUNT / 2; k++)
      {
        if(positions_small[k].x <= -RAD || positions_small[k].x >= RAD) velocities_small[k].x *= -1;
        if(positions_small[k].y <= -RAD || positions_small[k].y >= RAD) velocities_small[k].y *= -1;
        if(positions_small[k].z <= -RAD || positions_small[k].z >= RAD) velocities_small[k].z *= -1;
        positions_small[k] = Vector3Add(positions_small[k], Vector3Scale(velocities_small[k], dt));
        DrawCube(positions_small[k], widths_small[k], heights_small[k], lengths_small[k], colors_small[k]);
      }
    for(size_t k = 0; k < ENTITY_AMOUNT / 2; k++)
    {
      if(positions_large[k].x <= -RAD || positions_large[k].x >= RAD) velocities_large[k].x *= -1;
      if(positions_large[k].y <= -RAD || positions_large[k].y >= RAD) velocities_large[k].y *= -1;
      if(positions_large[k].z <= -RAD || positions_large[k].z >= RAD) velocities_large[k].z *= -1;
      positions_large[k] = Vector3Add(positions_large[k], Vector3Scale(velocities_large[k], dt));
      DrawCube(positions_large[k], widths_large[k], heights_large[k], lengths_large[k], colors_large[k]);
    }
    if(MORPH)
    {
      const Color c = GetColor(color_distr(engine)| 0xff);
      for(size_t k = 0; k < ENTITY_AMOUNT / 2; k++)
      {
        colors_large[k] = c;
      }
    }
    EndMode3D();
    DrawText(TextFormat("FPS: %i", GetFPS()), 0, 0, 20, RED);
    DrawText(TextFormat("Cubes: %i", ENTITY_AMOUNT), 0, 50, 20, RED);
    DrawText(TextFormat("Morph: %i", MORPH ? 1 : 0), 0, 100, 20, RED);
    DrawText(TextFormat("Large Only %i", LARGE_ONLY ? 1 : 0), 0, 150, 20, RED);
    EndDrawing();
  }
}

