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
extern bool MORPH;
extern bool LARGE_ONLY;

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
  appearance_component(const Color c, const float w, const float h, const float l, const bool is_large):
  color(c), width(w), height(h), length(l), is_large(is_large) {}
  void draw(const Vector3 position) const
  {
    DrawCube(position, width, height, length, color);
  }
  void set_color(Color c){color = c;}
  bool large() const {return is_large;}
private:
  Color color;
  float width, height, length;
  bool is_large;
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


void oop_condes()
{
  entity** entities = (entity**) malloc(sizeof(entity*) * ENTITY_AMOUNT);
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
      DrawSphere(ARENA_ORIGIN, ARENA_RAD, ARENA_COL);
      take_inputs(camera, dt);
      for(size_t k = 0; k < component_amount; k++)
      {
        entity* entt = entities[k];
        if(!entt->in_bounds_condes())
        {
          const size_t last = component_amount-1;
          const size_t removed_entity = dense[sparse[k]];
          const size_t moved_entity = dense[last];
          dense[k] = moved_entity;
          sparse[moved_entity] = k;
          delete entities[k];
          entities[k] = entities[last];
          --component_amount;
          recvec[removed_entity] = head;
          head = removed_entity;
          ++count;
        }
      }
      for(size_t k = 0; k < component_amount; k++)
      {
        entity* entt = entities[k];
        entt->move(dt);
        entt->draw();
      }
      if(num_of_spawns < 10)
      {
        for(size_t k = 0; k < SPAWN_RATE; k++)
        {
          const auto dx = (float)real_vel_distr(engine);
          const auto dy = (float)real_vel_distr(engine);
          const auto dz = (float)real_vel_distr(engine);
          const Color c = GetColor(color_distr(engine)| 0xff) ;
          size_t id;
          if(count > 0)
          {
            id = head;
            head = recvec[head];
            --count;
          }
          else
          {
            if(entity_count == ENTITY_AMOUNT) return;
            id = entity_count;
            recvec[entity_count] = entity_count;
            ++entity_count;
          }
          sparse[id] = component_amount;
          dense[component_amount] = id;
          entities[component_amount] = new entity{
            new movement_component({0,0,0}, {dx,dy,dz}),
            new appearance_component{c, 10, 10, 10, false}
          };
          component_amount++;
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

void oop_static()
{
  entity** entities = (entity**) malloc(sizeof(entity*) * ENTITY_AMOUNT);
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
  size_t i = 0;
  for(;i < ENTITY_AMOUNT / 2; i++)
  {
    const auto dx = (float)real_vel_distr(engine);
    const auto dy = (float)real_vel_distr(engine);
    const auto dz = (float)real_vel_distr(engine);
    const Color c = GetColor(color_distr(engine)| 0xff);
    entities[i] = new entity{
      new movement_component({0,0,0}, {dx,dy,dz}),
      new appearance_component(c,5, 5, 5, false)
    };
  }
  for(;i < ENTITY_AMOUNT; i++)
  {
    const auto dx = (float)real_vel_distr(engine);
    const auto dy = (float)real_vel_distr(engine);
    const auto dz = (float)real_vel_distr(engine);
    const Color c = GetColor(color_distr(engine)| 0xff);
    entities[i] = new entity{
      new movement_component({0,0,0}, {dx,dy,dz}),
      new appearance_component(c,5, 5, 5, true)
    };
  }


  while(!WindowShouldClose())
  {
    ClearBackground(BLACK);
    const float dt = GetFrameTime();
    BeginDrawing();
    BeginMode3D(camera);
    take_inputs(camera, dt);
    DrawCubeWires(ARENA_ORIGIN, ARENA_DIM, ARENA_DIM, ARENA_DIM, ARENA_COL);
    Color c;
    if(MORPH)
      c = GetColor(color_distr(engine)| 0xff);
    for(size_t k = 0; k < ENTITY_AMOUNT; k++)
    {
      entity* e = entities[k];
      const bool is_large = e->ac->large();
      if(LARGE_ONLY)
        if(!is_large) continue;
      e->in_bounds_static();
      if(MORPH && is_large)
        e->ac->set_color(c);
      e->move(dt);
      e->draw();
    }
    EndMode3D();
    DrawText(TextFormat("FPS: %i", GetFPS()), 0, 0, 20, RED);
    DrawText(TextFormat("Cubes: %i", ENTITY_AMOUNT), 0, 50, 20, RED);
    DrawText(TextFormat("Morph: %i", MORPH ? 1 : 0), 0, 100, 20, RED);
    DrawText(TextFormat("Large Only %i", LARGE_ONLY ? 1 : 0), 0, 150, 20, RED);
    EndDrawing();
  }
}