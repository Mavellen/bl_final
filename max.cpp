
#include <random>

#include "raylib.h"
#include "raymath.h"

#include <immintrin.h>


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

void max_condes_raw()
{
  const auto positions = new Vector3[ENTITY_AMOUNT];
  const auto velocities = new Vector3[ENTITY_AMOUNT];
  const auto colors = new Color[ENTITY_AMOUNT];
  const auto widths = new float[ENTITY_AMOUNT];
  const auto heights = new float[ENTITY_AMOUNT];
  const auto lengths = new float[ENTITY_AMOUNT];

  const auto dense = new size_t[ENTITY_AMOUNT];
  const auto sparse = new size_t[ENTITY_AMOUNT];
  size_t active = 0;
  size_t head = 0;
  size_t count = 0;

  const auto entities = new size_t[ENTITY_AMOUNT];
  size_t entity_count = 0;

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
    for(auto k = 0; k < active; k++)
    {
      Vector3& p = positions[k];
      if(p.x <= -RAD || p.x >= RAD ||
         p.y <= -RAD || p.y >= RAD ||
         p.z <= -RAD || p.z >= RAD)
      {
        const size_t last = active-1;
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
        -- active;

        entities[removed_entity] = head;
        head = removed_entity;
        ++ count;
        continue;
      }
      p = Vector3Add(p, Vector3Scale(velocities[k], dt));
    }
    if(num_of_spawns < 10)
    {
      for(size_t k = 0; k < SPAWN_RATE; k++)
      {
        size_t id;
        if(count > 0)
        {
          id = head;
          head = entities[head];
          --count;
        }
        else
        {
          if(entity_count == ENTITY_AMOUNT) break;;
          id = entity_count;
          entities[entity_count] = entity_count;
          ++entity_count;
        }
        sparse[id] = active;
        dense[active] = id;
        positions[active] = {0,0,0};
        velocities[active] = {
          dirlux[id % DIRECTIONS],
          dirluy[id % DIRECTIONS],
          dirluz[id % DIRECTIONS]
        };
        colors[active] = pdcolors[id % DIRECTIONS];
        widths[active] = 10;
        heights[active] = 10;
        lengths[active] = 10;
        ++active;
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
    if(iterations >= MAX_FRAMES) break;
  }
  avgframetime = total_frametime / iterations;
}

void max_static_raw()
{
  const auto positions = new Vector3[ENTITY_AMOUNT];
  const auto velocities = new Vector3[ENTITY_AMOUNT];
  const auto colors = new Color[ENTITY_AMOUNT];
  const auto widths = new float[ENTITY_AMOUNT];
  const auto heights = new float[ENTITY_AMOUNT];
  const auto lengths = new float[ENTITY_AMOUNT];

  for(size_t i = 0; i < ENTITY_AMOUNT; i++)
  {
    positions[i] = {0,0,0};
    velocities[i] = {
      dirlux[i % DIRECTIONS],
      dirluy[i % DIRECTIONS],
      dirluz[i % DIRECTIONS]
    };
    colors[i] = pdcolors[i % DIRECTIONS];
    widths[i] = 10;
    heights[i] = 10;
    lengths[i] = 10;
  }

  double iterations = 0;
  double total_frametime = 0;

  while(true)
  {
    if(IsKeyDown(KEY_Q)) break;
    ++iterations;
    const float dt = GetFrameTime();
    total_frametime += dt;
    for(size_t k = 0; k < ENTITY_AMOUNT; k++)
    {
      Vector3& p = positions[k];
      Vector3& v = velocities[k];
      if(p.x <= -RAD || p.x >= RAD) v.x *= -1;
      if(p.y <= -RAD || p.y >= RAD) v.y *= -1;
      if(p.z <= -RAD || p.z >= RAD) v.z *= -1;
      p = Vector3Add(p, Vector3Scale(v, dt));
    }
    // __m128 delta = _mm_set1_ps(dt);
    // for(size_t k = 0; k < ENTITY_AMOUNT; k+=4)
    // {
    //
    //   __m128 addx_data = _mm_set1_ps(0.0f);
    //   __m128 addy_data = _mm_set1_ps(0.0f);
    //   __m128 addz_data = _mm_set1_ps(0.0f);
    //   __m128 scalex_data = _mm_set1_ps(0.0f);
    //   __m128 scaley_data = _mm_set1_ps(0.0f);
    //   __m128 scalez_data = _mm_set1_ps(0.0f);
    //   for(size_t i = 0; i < 4; i++)
    //   {
    //     Vector3& p = positions[k + i];
    //     Vector3& v = velocities[k + i];
    //     if(p.x <= -RAD || p.x >= RAD) v.x *= -1;
    //     if(p.y <= -RAD || p.y >= RAD) v.y *= -1;
    //     if(p.z <= -RAD || p.z >= RAD) v.z *= -1;
    //     ((float*)&addx_data)[i] = p.x;
    //     ((float*)&addy_data)[i] = p.y;
    //     ((float*)&addz_data)[i] = p.z;
    //     ((float*)&scalex_data)[i] = v.x;
    //     ((float*)&scaley_data)[i] = v.y;
    //     ((float*)&scalez_data)[i] = v.z;
    //   }
    //
    //   addx_data = _mm_add_ps(_mm_mul_ps(delta, scalex_data), addx_data);
    //   addy_data = _mm_add_ps(_mm_mul_ps(delta, scaley_data), addy_data);
    //   addz_data = _mm_add_ps(_mm_mul_ps(delta, scalez_data), addz_data);
    //
    //   for(size_t i = 0; i < 4; i++)
    //   {
    //     Vector3& p = positions[k + i];
    //     p.x = ((float*)&addx_data)[i];
    //     p.y = ((float*)&addy_data)[i];
    //     p.z = ((float*)&addz_data)[i];
    //   }
    // }
    ClearBackground(BLACK);
    BeginDrawing();
    DrawText(TextFormat("FPS: %i", GetFPS()), 0, 0, 20, RED);
    DrawText(TextFormat("Cubes: %i", ENTITY_AMOUNT), 0, 50, 20, RED);
    EndDrawing();
    if(iterations >= MAX_FRAMES) break;
  }
  avgframetime = total_frametime / iterations;
}

void max_condes()
{
  const auto positions = new Vector3[ENTITY_AMOUNT];
  const auto velocities = new Vector3[ENTITY_AMOUNT];
  const auto colors = new Color[ENTITY_AMOUNT];
  const auto widths = new float[ENTITY_AMOUNT];
  const auto heights = new float[ENTITY_AMOUNT];
  const auto lengths = new float[ENTITY_AMOUNT];

  const auto dense = new size_t[ENTITY_AMOUNT];
  const auto sparse = new size_t[ENTITY_AMOUNT];
  size_t active = 0;
  size_t head = 0;
  size_t count = 0;

  const auto entities = new size_t[ENTITY_AMOUNT];
  size_t entity_count = 0;

  Camera camera {
          { .0f, 5.0f, -600.0f},
          { .0f, 5.0f, .0f},
          { .0f, 1.0f, .0f},
          60,
          CAMERA_PERSPECTIVE
        };


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

    BeginDrawing();
    BeginMode3D(camera);
    {
      DrawCubeWires(ARENA_ORIGIN, ARENA_DIM, ARENA_DIM, ARENA_DIM, ARENA_COL);
      take_inputs(camera, dt);
      for(size_t k = 0; k < active; k++)
      {
        Vector3& p = positions[k];
        if(p.x <= -RAD || p.x >= RAD ||
         p.y <= -RAD || p.y >= RAD ||
         p.z <= -RAD || p.z >= RAD)
        {
          const size_t last = active-1;
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
          --active;

          entities[removed_entity] = head;
          head = removed_entity;
          ++count;
          continue;
        }
        p = Vector3Add(p, Vector3Scale(velocities[k], dt));
        DrawCube(p, widths[k], heights[k], lengths[k], colors[k]);
      }
      if(num_of_spawns < 10)
      {
        for(size_t k = 0; k < SPAWN_RATE; k++)
        {
          size_t id;
          if(count > 0)
          {
            id = head;
            head = entities[head];
            --count;
          }
          else
          {
            if(entity_count == ENTITY_AMOUNT) break;;
            id = entity_count;
            entities[entity_count] = entity_count;
            ++entity_count;
          }
          sparse[id] = active;
          dense[active] = id;
          positions[active] = {0,0,0};
          velocities[active] = {
            dirlux[id % DIRECTIONS],
            dirluy[id % DIRECTIONS],
            dirluz[id % DIRECTIONS]
          };
          colors[active] = pdcolors[id % DIRECTIONS];
          widths[active] = 10;
          heights[active] = 10;
          lengths[active] = 10;
          ++active;
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
    if(iterations >= MAX_FRAMES) break;
  }
  avgframetime = total_frametime / iterations;
}

void max_static()
{
  const auto positions = new Vector3[ENTITY_AMOUNT];
  const auto velocities = new Vector3[ENTITY_AMOUNT];
  const auto colors = new Color[ENTITY_AMOUNT];
  const auto widths = new float[ENTITY_AMOUNT];
  const auto heights = new float[ENTITY_AMOUNT];
  const auto lengths = new float[ENTITY_AMOUNT];

  for(size_t i = 0; i < ENTITY_AMOUNT; i++)
  {
    positions[i] = {0,0,0};
    velocities[i] = {
      dirlux[i % DIRECTIONS],
      dirluy[i % DIRECTIONS],
      dirluz[i % DIRECTIONS]
    };
    colors[i] = pdcolors[i % DIRECTIONS];
    widths[i] = 10;
    heights[i] = 10;
    lengths[i] = 10;
  }

  Camera camera {
            { .0f, 5.0f, -600.0f},
            { .0f, 5.0f, .0f},
            { .0f, 1.0f, .0f},
            60,
            CAMERA_PERSPECTIVE
          };

  double iterations = 0;
  double total_frametime = 0;

  while(!WindowShouldClose())
  {
    ClearBackground(BLACK);
    ++iterations;
    const float dt = GetFrameTime();
    total_frametime += dt;
    BeginDrawing();
    BeginMode3D(camera);
    take_inputs(camera, dt);
    DrawCubeWires(ARENA_ORIGIN, ARENA_DIM, ARENA_DIM, ARENA_DIM, ARENA_COL);
    for(size_t k = 0; k < ENTITY_AMOUNT; k++)
    {
      Vector3& p = positions[k];
      Vector3& v = velocities[k];
      if(p.x <= -RAD || p.x >= RAD) v.x *= -1;
      if(p.y <= -RAD || p.y >= RAD) v.y *= -1;
      if(p.z <= -RAD || p.z >= RAD) v.z *= -1;
      p = Vector3Add(p, Vector3Scale(v, dt));
      DrawCube(p, widths[k], heights[k], lengths[k], colors[k]);
    }

    EndMode3D();
    DrawText(TextFormat("FPS: %i", GetFPS()), 0, 0, 20, RED);
    DrawText(TextFormat("Cubes: %i", ENTITY_AMOUNT), 0, 50, 20, RED);
    EndDrawing();
    if(iterations >= MAX_FRAMES) break;
  }
  avgframetime = total_frametime / iterations;
}

