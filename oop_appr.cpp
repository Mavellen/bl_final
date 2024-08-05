#include <vector>
#include "raylib.h"
#include "raymath.h"
#include "random"

class world;
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

class movement_component
{
public:
  movement_component(const Vector3 p, const Vector3 v):
  position(p), velocity(v) {}
  [[nodiscard]] bool in_bounds_condes() const
  {
    if(position.x <= -RAD || position.x >= RAD ||
    position.y <= -RAD || position.y >= RAD ||
    position.z <= -RAD || position.z >= RAD)
    {
      return false;
    }
    return true;
  }
  void in_bounds_static()
  {
    if(position.x <= -RAD || position.x >= RAD) velocity.x *= -1;
    if(position.y <= -RAD || position.y >= RAD) velocity.y *= -1;
    if(position.z <= -RAD || position.z >= RAD) velocity.z *= -1;
  }
  void move(const float dt)
  {
    position = Vector3Add(position, Vector3Scale(velocity, dt));
  }
  [[nodiscard]] Vector3 get_position() const {return position;}
private:
  Vector3 position, velocity;
};
class appearance_component
{
public:
  appearance_component(const Color c, const float w, const float h, const float l):
  color(c), width(w), height(h), length(l){}
  void draw(const Vector3 position) const
  {
    DrawCube(position, width, height, length, color);
  }
  void set_color(Color c){color = c;}
private:
  Color color;
  float width, height, length;
};
class entity
{
public:
  entity(movement_component* mc, appearance_component* ac):
  mc(mc), ac(ac) {}
  [[nodiscard]] bool in_bounds_condes() const
  {
    return mc->in_bounds_condes();
  }
  void in_bounds_static() const
  {
    mc->in_bounds_static();
  }
  void move(const float dt) const
  {
    mc->move(dt);
  }
  void draw() const
  {
    ac->draw(mc->get_position());
  }
  movement_component* mc;
  appearance_component* ac;
};

void oop_condes_raw()
{
  const auto agents = new entity*[ENTITY_AMOUNT];
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

    for(size_t k = 0; k < active; k++)
    {
      const entity* entt = agents[k];
      if(!entt->in_bounds_condes())
      {
        const size_t last = active-1;
        const size_t removed_entity = dense[sparse[k]];
        const size_t moved_entity = dense[last];
        dense[k] = moved_entity;
        sparse[moved_entity] = k;
        delete agents[k];
        agents[k] = agents[last];
        entities[k] = entities[last];
        --active;
        entities[removed_entity] = head;
        head = removed_entity;
        ++count;
        continue;
      }
      entt->move(dt);
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
          if(entity_count == ENTITY_AMOUNT) return;
          id = entity_count;
          entities[entity_count] = entity_count;
          ++entity_count;
        }
        sparse[id] = active;
        dense[active] = id;
        agents[active] = new entity{
          new movement_component(
            {0,0,0},
            {
            dirlux[id % DIRECTIONS],
            dirluy[id % DIRECTIONS],
            dirluz[id % DIRECTIONS]
            }),
          new appearance_component{pdcolors[id % DIRECTIONS], 10, 10, 10}
        };
        active++;
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

void oop_static_raw()
{
  const auto agents = new entity*[ENTITY_AMOUNT];

  for(size_t i = 0; i < ENTITY_AMOUNT; i++)
  {
    agents[i] = new entity{
      new movement_component(
        {0,0,0},
        {
        dirlux[i % DIRECTIONS],
        dirluy[i % DIRECTIONS],
        dirluz[i % DIRECTIONS]
        }),
      new appearance_component{pdcolors[i % DIRECTIONS], 10, 10, 10}
    };
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
      const entity* e = agents[k];
      e->in_bounds_static();
      e->move(dt);
    }
    ClearBackground(BLACK);
    BeginDrawing();
    DrawText(TextFormat("FPS: %i", GetFPS()), 0, 0, 20, RED);
    DrawText(TextFormat("Cubes: %i", ENTITY_AMOUNT), 0, 50, 20, RED);
    EndDrawing();
    if(iterations >= MAX_FRAMES) break;
  }
  avgframetime = total_frametime / iterations;
}

void oop_condes()
{
  const auto agents = new entity*[ENTITY_AMOUNT];
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
        const entity* entt = agents[k];
        if(!entt->in_bounds_condes())
        {
          const size_t last = active-1;
          const size_t removed_entity = dense[sparse[k]];
          const size_t moved_entity = dense[last];
          dense[k] = moved_entity;
          sparse[moved_entity] = k;
          delete agents[k];
          agents[k] = agents[last];
          entities[k] = entities[last];
          --active;
          entities[removed_entity] = head;
          head = removed_entity;
          ++count;
          continue;
        }
        entt->move(dt);
        entt->draw();
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
            if(entity_count == ENTITY_AMOUNT) return;
            id = entity_count;
            entities[entity_count] = entity_count;
            ++entity_count;
          }
          sparse[id] = active;
          dense[active] = id;
          agents[active] = new entity{
            new movement_component(
              {0,0,0},
              {
              dirlux[k % DIRECTIONS],
              dirluy[k % DIRECTIONS],
              dirluz[k % DIRECTIONS]
              }),
            new appearance_component{pdcolors[k % DIRECTIONS], 10, 10, 10}
          };
          active++;
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

void oop_static()
{
  Camera camera {
              { .0f, 5.0f, -600.0f},
              { .0f, 5.0f, .0f},
              { .0f, 1.0f, .0f},
              60,
              CAMERA_PERSPECTIVE
            };

  const auto agents = new entity*[ENTITY_AMOUNT];

  for(size_t i = 0; i < ENTITY_AMOUNT; i++)
  {
    agents[i] = new entity{
      new movement_component(
        {0,0,0},
        {
        dirlux[i % DIRECTIONS],
        dirluy[i % DIRECTIONS],
        dirluz[i % DIRECTIONS]
        }),
      new appearance_component{pdcolors[i % DIRECTIONS], 10, 10, 10}
    };
  }

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
      const entity* e = agents[k];
      e->in_bounds_static();
      e->move(dt);
      e->draw();
    }
    EndMode3D();
    DrawText(TextFormat("FPS: %i", GetFPS()), 0, 0, 20, RED);
    DrawText(TextFormat("Cubes: %i", ENTITY_AMOUNT), 0, 50, 20, RED);
    EndDrawing();
    if(iterations >= MAX_FRAMES) break;
  }
  avgframetime = total_frametime / iterations;
}