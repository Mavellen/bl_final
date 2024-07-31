#include "raylib.h"
#include "random"

extern float CAMSPEED;
extern float RAD;
extern int ENTITY_AMOUNT;
extern Vector3 ARENA_ORIGIN;
extern float ARENA_RAD;
extern float ARENA_DIM;
extern Color ARENA_COL;

extern void take_inputs(Camera& camera, float dt);

#include "lecs.h"
using namespace ls::lecs;

enum shape_type : unsigned char
{
  CUBE, SPHERE, CYLINDER, CAPSULE,
  NUM
};

struct cube{};
struct sphere{};
struct cylinder{};
struct capsule{};

struct position { Vector3 p; };
struct velocity { Vector3 v; };
struct cube_dim {float w,h,l; };
struct sphere_dim {float rad; };
struct cylinder_dim {float rad_t, rad_b, h; int slices;};
struct capsule_dim {Vector3 endpos; float rad; int slices, rings;};

void make_cube(sim& s, ecsid entity, const float x, const float y, const float z,
  const float dx, const float dy, const float dz,
  const float width, const float height, const float length,
  const Color c)
{
  s.add<cube>(entity);
  s.add<position>(entity,Vector3{x,y,z});
  s.add<velocity>(entity,Vector3{dx,dy,dz});
  s.add<cube_dim>(entity,width,height,length);
  s.add<Color>(entity,c.r, c.g, c.b, c.a);
}
void make_sphere(sim& s, ecsid entity, const float x, const float y, const float z,
  const float dx, const float dy, const float dz,
  const float radius, const Color c)
{
  s.add<sphere>(entity);
  s.add<position>(entity,Vector3{x,y,z});
  s.add<velocity>(entity,Vector3{dx,dy,dz});
  s.add<sphere_dim>(entity,radius);
  s.add<Color>(entity, c.r, c.g, c.b, c.a);
}
void make_cylinder(sim& s, ecsid entity, const float x, const float y, const float z,
  const float dx, const float dy, const float dz,
  const float rad_top, const float rad_bot, const float height, const int slices,
  const Color c)
{
  s.add<cylinder>(entity);
  s.add<position>(entity,Vector3{x,y,z});
  s.add<velocity>(entity,Vector3{dx,dy,dz});
  s.add<cylinder_dim>(entity,rad_top,rad_bot,height,slices);
  s.add<Color>(entity, c.r, c.g, c.b, c.a);
}
void make_capsule(sim& s, ecsid entity, const float x, const float y, const float z,
  const float dx, const float dy, const float dz,
  const float ex, const float ey, const float ez, const float radius, const int slices, const int rings,
  const Color c)
{
  s.add<capsule>(entity);
  s.add<position>(entity,Vector3{x,y,z});
  s.add<velocity>(entity,Vector3{dx,dy,dz});
  s.add<capsule_dim>(entity,Vector3{ex,ey,ez},radius,slices,rings);
  s.add<Color>(entity, c.r,c.g,c.b,c.a);
}

void make_rand_shapes(sim& s, int amount)
{
  static std::random_device device;
  static std::default_random_engine engine(device());
  static std::uniform_int_distribution<> color_distr(0, UINT32_MAX);
  static std::uniform_int_distribution<> int_distr(0, NUM-1);
  static std::uniform_real_distribution<> real_pos_distr(-100, 100);
  static std::uniform_real_distribution<> real_vel_distr(-10, 10);
  static std::uniform_real_distribution<> real_size_cube(0, 10);
  static std::uniform_real_distribution<> real_size_sphere(0, 5);
  do
  {
    ecsid entity = s.entity();
    const auto x = (float)real_pos_distr(engine);
    const auto y = (float)real_pos_distr(engine);
    const auto z = (float)real_pos_distr(engine);
    const auto dx = (float)real_vel_distr(engine);
    const auto dy = (float)real_vel_distr(engine);
    const auto dz = (float)real_vel_distr(engine);
    const Color c = GetColor(color_distr(engine)| 0xff) ;
    switch (/*int_distr(engine)*/3)
    {
    case 0:
      {
        const auto wid = 10 + (float)real_size_cube(engine);
        const auto hei = 10 + (float)real_size_cube(engine);
        const auto len = 10 + (float)real_size_cube(engine);
        make_cube(s,entity,x,y,z,dx,dy,dz,wid,hei,len,c);
        break;
      }
    case 1:
      {
        const float radius = 5 + (float)real_size_sphere(engine);
        make_sphere(s,entity,x,y,y,dx,dy,dz,radius,c);
        break;
      }
    case 2:
      {
        const float rad_top = 5 + (float)real_size_sphere(engine);
        const float rad_bot = 5 + (float)real_size_sphere(engine);
        const float hei = 10 + (float)real_size_cube(engine);
        const int slices = 10;
        make_cylinder(s,entity,x,y,z,dx,dy,dz,rad_top,rad_bot,hei,slices,c);
        break;
      }
    case 3:
      {
        const float endx = x + 5 + (float)real_size_sphere(engine);
        const float endy = y + 5 + (float)real_size_sphere(engine);
        const float endz = z + 5 + (float)real_size_sphere(engine);
        const float radius = 5 + (float)real_size_sphere(engine);
        const int slices = 10;
        const int rings = 5;
        make_capsule(s,entity,x,y,z,dx,dy,dz,endx,endy,endz,radius,slices,rings,c);
        break;
      }
    default:
      break;
    }
    --amount;
  }while(amount > 0);
}

void dod_main()
{
  sim s;
  auto in_bounds = +[](const ecsid* entity, size_t size, position* p, velocity* v)
  {
    for(size_t i = 0; i < size; i++)
    {
      if(p[i].p.x <= -RAD || p[i].p.x >= RAD){ v[i].v.x *= -1; }
      if(p[i].p.y <= -RAD || p[i].p.y >= RAD){ v[i].v.y *= -1; }
      if(p[i].p.z <= -RAD || p[i].p.z >= RAD){ v[i].v.z *= -1; }
    }
  };
  auto move = +[](const ecsid* entity, size_t size, std::tuple<float>& tuple, position* p, velocity* v)
  {
    float dt = std::get<0>(tuple);
    for(size_t i = 0; i < size; i++)
    {
      p[i].p.x += ( v[i].v.x * dt );
      p[i].p.y += ( v[i].v.y * dt );
      p[i].p.z += ( v[i].v.z * dt );
    }
  };
  auto move_capsules = +[](const ecsid* entity, size_t size, std::tuple<float>& tuple, position* p, velocity* v, capsule_dim* cd)
  {
    float dt = std::get<0>(tuple);
    for(size_t i = 0; i < size; i++)
    {
      p[i].p.x += ( v[i].v.x * dt );
      p[i].p.y += ( v[i].v.y * dt );
      p[i].p.z += ( v[i].v.z * dt );
      cd[i].endpos.x += ( v[i].v.x * dt );
      cd[i].endpos.y += ( v[i].v.y * dt );
      cd[i].endpos.z += ( v[i].v.z * dt );
    }
  };
  auto draw_cubes = +[](const ecsid* entity, size_t size, position* p, cube_dim* cd, Color* c)
  {
    for(size_t i = 0; i < size; i++)
    {
      DrawRectangle(p[i].p.x, p[i].p.y, cd->w, cd->h, *c);
      //DrawCube(p[i].p, cd[i].w, cd[i].h, cd[i].l, c[i]);
    }
  };
  auto draw_spheres = +[](const ecsid* entity, size_t size, position* p, sphere_dim* sd, Color* c)
  {
    for(size_t i = 0; i < size; i++)
    {
      DrawCircle(p[i].p.x, p[i].p.y, sd->rad, *c);
      //DrawSphere(p[i].p, sd[i].rad, c[i]);
    }
  };
  auto draw_cylinders = +[](const ecsid* entity, size_t size, position* p, cylinder_dim* cd, Color* c)
  {
    for(size_t i = 0; i < size; i++)
    {
      //DrawCylinder(p[i].p, cd[i].rad_t, cd[i].rad_b, cd[i].h, cd[i].slices, c[i]);
    }
  };
  auto draw_capsules = +[](const ecsid* entity, size_t size, position* p, capsule_dim* cd, Color* c)
  {
    for(size_t i = 0; i < size; i++)
    {
      //DrawCapsule(p[i].p, cd[i].endpos, cd[i].rad, cd[i].slices, cd[i].rings, c[i]);
    }
  };
  auto input_func = +[](ecsid entity, std::tuple<float>& tuple, Camera* c)
  {
    take_inputs(*c, std::get<0>(tuple));
  };

  const ecsid camera = s.entity();
  s.add<Camera>(camera, Vector3{ .0f, 5.0f, -600.0f},
    Vector3{.0f, 5.0f, .0f}, Vector3{.0f, 1.0f, .0f}, 60, CAMERA_PERSPECTIVE);
  auto cam = s.get<Camera>(camera);

  make_rand_shapes(s, ENTITY_AMOUNT);

  const auto in_bounds_q = s.query();
  const auto move_q = s.query();
  const auto move_capsules_q = s.query();
  const auto drawc_q = s.query();
  const auto draws_q = s.query();
  const auto drawcyl_q = s.query();
  const auto drawcap_q = s.query();
  const auto cam_q = s.query();
  in_bounds_q->all_of<position,velocity>();
  move_q->all_of<position,velocity>();
  move_q->none_of<capsule>();
  move_capsules_q->all_of<position, velocity, capsule_dim>();
  drawc_q->all_of<position,cube_dim,Color>();
  draws_q->all_of<position,sphere_dim,Color>();
  drawcyl_q->all_of<position,cylinder_dim,Color>();
  drawcap_q->all_of<position,capsule_dim,Color>();
  cam_q->all_of<Camera>();
  in_bounds_q->update();
  move_q->update();
  move_capsules_q->update();
  drawc_q->update();
  draws_q->update();
  drawcyl_q->update();
  drawcap_q->update();
  cam_q->update();

  while(!WindowShouldClose())
  {
    ClearBackground(BLACK);
    const float dt = GetFrameTime();
    auto tuple = std::make_tuple(dt);
    BeginDrawing();
    //BeginMode3D(*cam);
    {
      //DrawCubeWires(ARENA_ORIGIN, ARENA_DIM, ARENA_DIM, ARENA_DIM, ARENA_COL);
      //DrawSphere(ARENA_ORIGIN, ARENA_RAD, ARENA_COL);
      //cam_q->each(input_func, tuple);
      in_bounds_q->batch(in_bounds);
      move_q->batch(move, tuple);
      move_capsules_q->batch(move_capsules, tuple);
      drawc_q->batch(draw_cubes);
      draws_q->batch(draw_spheres);
      drawcyl_q->batch(draw_cylinders);
      drawcap_q->batch(draw_capsules);
    }
    //EndMode3D();
    DrawText(TextFormat("FPS: %i", GetFPS()), 0, 0, 20, RED);
    EndDrawing();
  }
}