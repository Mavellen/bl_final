#include <vector>

#include "raylib.h"
#include "random"

extern float CAMSPEED;
extern float RAD;
extern int ENTITY_AMOUNT;
extern Vector3 ARENA_ORIGIN;
extern float ARENA_RAD;
extern float ARENA_DIM;
extern Color ARENA_COL;

enum shape_type : unsigned char
{
  CUBE, SPHERE, CYLINDER, CAPSULE,
  NUM
};
class shape
{
protected:
  shape(const Vector3 position, const Vector3 velocity, const Color color, const shape_type type)
    : type(type), color(color), position(position), velocity(velocity) {}
  virtual ~shape() = default;
public:
  virtual void move(float dt) = 0;
  virtual void draw() = 0;
  void in_bounds()
  {
    if(position.x <= -RAD || position.x >= RAD){ velocity.x *= -1; }
    if(position.y <= -RAD || position.y >= RAD){ velocity.y *= -1; }
    if(position.z <= -RAD || position.z >= RAD){ velocity.z *= -1; }
  }
  void set_color(const Color c)
  {
    this->color = c;
  }
  [[nodiscard]] Color* get_color()
  {
    return &color;
  }
  void set_velocity(const Vector3 v)
  {
    this->velocity = v;
  }
  [[nodiscard]] Vector3* get_velocity()
  {
    return &velocity;
  }
  void set_position(const Vector3 p)
  {
    this->position = p;
  }
  [[nodiscard]] Vector3* get_position()
  {
    return &position;
  }
  [[nodiscard]] bool get_alive() const
  {
    return is_alive;
  }
  void set_alive(const bool b)
  {
    is_alive = b;
  }
protected:
  shape_type type;
  bool is_alive = true;
  Color color;
  Vector3 position;
  Vector3 velocity;
};

class cube final : public shape
{
public:
  cube(const Color col, const Vector3 p, const Vector3 v,
    const float width, const float height, const float length)
  : shape(p, v, col, CUBE), width(width), height(height), length(length)
  {}
  ~cube() override = default;
  void move(const float dt) override
  {
    in_bounds();
    position.x += ( velocity.x * dt );
    position.y += ( velocity.y * dt );
    position.z += ( velocity.z * dt );
  }
  void draw() override
  {
    DrawCube(position, width, height, length, color);
  }
private:
  float width;
  float height;
  float length;
};

class sphere final : public shape
{
public:
  sphere(const Color col, const Vector3 p, const Vector3 v, const float radius)
  : shape(p, v, col, SPHERE), radius(radius)
  {}
  ~sphere() override = default;
  void move(const float dt) override
  {
    in_bounds();
    position.x += ( velocity.x * dt );
    position.y += ( velocity.y * dt );
    position.z += ( velocity.z * dt );
  }
  void draw() override
  {
    DrawSphere(position, radius, color);
  }
private:
  float radius;
};

class cylinder final : public shape
{
public:
  cylinder(const Color col, const Vector3 p, const Vector3 v,
    const float radius_top, const float radius_bottom, const float height, const int slices)
  : shape(p, v, col, CYLINDER), radius_top(radius_top), radius_bottom(radius_bottom), height(height), slices(slices)
  {}
  ~cylinder() override = default;
  void move(const float dt) override
  {
    in_bounds();
    position.x += ( velocity.x * dt );
    position.y += ( velocity.y * dt );
    position.z += ( velocity.z * dt );
  }
  void draw() override
  {
    DrawCylinder(position, radius_top, radius_bottom, height, slices, color);
  }
private:
  float radius_top;
  float radius_bottom;
  float height;
  int slices;
};

class capsule final : public shape
{
public:
  capsule(const Color col, const Vector3 startpos, const Vector3 v,
    const Vector3 endpos, const float radius, const int slices, const int rings)
  : shape(startpos, v, col, CAPSULE), endpos(endpos), radius(radius), slices(slices), rings(rings)
  {}
  ~capsule() override = default;
  void move(const float dt) override
  {
    in_bounds();
    position.x += ( velocity.x * dt );
    position.y += ( velocity.y * dt );
    position.z += ( velocity.z * dt );
    endpos.x += ( velocity.x * dt );
    endpos.y += ( velocity.y * dt );
    endpos.z += ( velocity.z * dt );
  }
  void draw() override
  {
    DrawCapsule(position, endpos, radius, slices, rings, color);
  }
private:
  Vector3 endpos;
  float radius;
  int slices;
  int rings;
};

class world
{
public:
  void update(const float dt)
  {
    for (shape* shape : shapes)
    {
      shape->move(dt);
      shape->draw();
    }
  }
  void add_shape(shape* shape){ shapes.push_back(shape); }
private:
  std::vector<shape*> shapes;
};

cube* make_cube(const float x, const float y, const float z,
  const float dx, const float dy, const float dz,
  const float width, const float height, const float length,
  const Color c)
{
  return new cube(c, {x,y,z}, {dx, dy, dz}, width, height, length);
}
sphere* make_sphere(const float x, const float y, const float z,
  const float dx, const float dy, const float dz,
  const float radius, const Color c)
{
  return new sphere(c, {x,y,z}, {dx,dy,dz}, radius);
}
cylinder* make_cylinder(const float x, const float y, const float z,
  const float dx, const float dy, const float dz,
  const float rad_top, const float rad_bot, const float height, const int slices,
  const Color c)
{
  return new cylinder(c, {x,y,z}, {dx,dy,dz}, rad_top, rad_bot, height, slices);
}
capsule* make_capsule(const float x, const float y, const float z,
  const float dx, const float dy, const float dz,
  const float ex, const float ey, const float ez, const float radius, const int slices, const int rings,
  const Color c)
{
  return new capsule(c, {x,y,z}, {dx,dy,dz}, {ex,ey,ez}, radius, slices, rings);
}

void make_rand_shapes(world& w, int amount)
{
  static std::random_device device;
  static std::default_random_engine engine(device());
  static std::uniform_int_distribution<> color_distr(0, UINT32_MAX);
  static std::uniform_int_distribution<> int_distr(0, NUM);
  static std::uniform_real_distribution<> real_pos_distr(-100, 100);
  static std::uniform_real_distribution<> real_vel_distr(-10, 10);
  static std::uniform_real_distribution<> real_size_cube(0, 10);
  static std::uniform_real_distribution<> real_size_sphere(0, 5);
  do
  {
    const auto x = (float)real_pos_distr(engine);
    const auto y = (float)real_pos_distr(engine);
    const auto z = (float)real_pos_distr(engine);
    const auto dx = (float)real_vel_distr(engine);
    const auto dy = (float)real_vel_distr(engine);
    const auto dz = (float)real_vel_distr(engine);
    const Color c = GetColor(color_distr(engine)| 0xff) ;
    switch (int_distr(engine))
    {
    case 0:
      {
        const auto wid = 10 + (float)real_size_cube(engine);
        const auto hei = 10 + (float)real_size_cube(engine);
        const auto len = 10 + (float)real_size_cube(engine);
        w.add_shape(make_cube(x,y,z,dx,dy,dz,wid,hei,len,c));
        break;
      }
    case 1:
      {
        const float radius = 5 + (float)real_size_sphere(engine);
        w.add_shape(make_sphere(x,y,y,dx,dy,dz,radius,c));
        break;
      }
    case 2:
      {
        const float rad_top = 5 + (float)real_size_sphere(engine);
        const float rad_bot = 5 + (float)real_size_sphere(engine);
        const float hei = 10 + (float)real_size_cube(engine);
        const int slices = 10;
        w.add_shape(make_cylinder(x,y,z,dx,dy,dz,rad_top,rad_bot,hei,slices,c));
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
        w.add_shape(make_capsule(x,y,z,dx,dy,dz,endx,endy,endz,radius,slices,rings,c));
        break;
      }
    default:
      break;
    }
    --amount;
  }while(amount > 0);
}

extern void take_inputs(Camera& camera, float dt);

void oop_main()
{
  world w;
  Camera camera {
    { .0f, 5.0f, -600.0f},
    { .0f, 5.0f, .0f},
    { .0f, 1.0f, .0f},
    60,
    CAMERA_PERSPECTIVE
  };

  make_rand_shapes(w, ENTITY_AMOUNT);

  constexpr Vector3 cp {0, 0, 0};
  float dim = RAD * 2;
  constexpr Color c {RAYWHITE};

  while(!WindowShouldClose())
  {
    ClearBackground(BLACK);
    const float dt = GetFrameTime();
    BeginDrawing();
      BeginMode3D(camera);
    {
      DrawCubeWires(ARENA_ORIGIN, ARENA_DIM, ARENA_DIM, ARENA_DIM, ARENA_COL);
      DrawSphere(ARENA_ORIGIN, ARENA_RAD, ARENA_COL);
      //DrawGrid(2000, 10.0f);
      take_inputs(camera, dt);
      w.update(dt);
    }
      EndMode3D();
    DrawText(TextFormat("FPS: %i", GetFPS()), 0, 0, 20, RED);
    EndDrawing();
  }
}