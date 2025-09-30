
# MiniRT Project Collaboration Plan

## 1. Overview

The project is divided in two main componenets that can be developed in parallel by two students:
- Student A: Parser, Scene Management, Window/Display
- Student B: Ray Tracing Engine, Math Library, Lighting

- Repo Structure:

```css
miniRT/
├── src/
│   ├── parser/       # Student A
│   ├── screen/      # Student A
│   ├── render/       # Student B
│   ├── math/         # Student B
│   ├── main.c
│   └── ...
├── include/
│   ├── minirt.h
│   ├── parser.h
│   ├── render.h
│   └── ...
├── libft/
├── mlx/
├── scenes/
│   ├── sphere.rt
│   ├── invalid.rt
│   └── ...
├── Makefile
```

- `make all` build `miniRT`
- `make parser` build only parser part
- `make render` build only render part


## 2. Shared Initial Setup 

```c

// minirt.h 
typedef struct s_vec3
{
    double x;
    double y;
    double z;
}               t_vec3;

typedef struct s_rgb
{
    int r;
    int g;
    int b;
}               t_rgb;

typedef struct s_ray
{
    t_vec3 origin;
    t_vec3 direction;
}               t_ray;

typedef struct s_camera
{
    t_vec3 position;
    t_vec3 orientation;
    double fov;
}               t_camera;

typedef struct s_light
{
    t_vec3 position;
    double brightness;
    t_rgb color;         // for bonus later
}               t_light;

typedef struct s_sphere
{
    t_vec3 center;
    double diameter;
    t_rgb color;
}               t_sphere;

typedef struct s_plane
{
    t_vec3 point;
    t_vec3 normal;
    t_rgb color;
}               t_plane;

typedef struct s_cylinder
{
    t_vec3 center;
    t_vec3 axis;
    double diameter;
    double height;
    t_rgb color;
}               t_cylinder;

typedef struct s_scene
{
    t_camera camera;
    t_light *lights;
    int light_count;
    t_rgb ambient_light;
    double ambient_ratio;
    void **objects; // array of object pointers
    int object_types; // array of object type identifiers
    int object_count;
}               t_scene;
```

## 3. Student A: Parser & Display System

1. Scene File Parser (parser.c, parser_utils.c)
2. Window Management (window.c)
3. Main Program Loop (main.c)
4. Error Handling (errors.c)

### Task A1: Basic File Reading

```c
// parser.c
t_scnene *parse_scene(char *filename);
int validate_extension(char *filename);
char **read_file_lines(char *fd);
```

### Task A2: Element Parsers

```c
// parser_elements.c
int parse_ambient(char *line, t_scene *scene);
int parse_camera(char *line, t_scene *scene);
int parse_light(char *line, t_scene *scene);
int parse_sphere(char *line, t_scene *scene);
int parse_plane(char *line, t_scene *scene);
int parse_cylinder(char *line, t_scene *scene);
```

### Task A3: Validation Functions

```c
// parser_validation.c
int validate_rgb(char *str, t_rgb *color);
int validate_coordinates(char *str, t_vec3 *vec);
int validate_normalized_vector(char *str, t_vec3 *vec);
int validate_ratio(char *str, double *ratio);
int validate_fov(char *str, double *fov);
```

### Task A4: MinilibX Setup

```c
// window.c
void *init_mlx(t_mlx *mlx, int width, int height);
void create_image(t_mlx *mlx);
void put_pixel(t_mlx *mlx, int x, int y, int color);
int handle_keypress(int keycode, t_mlx *mlx);
int handle_close(t_mlx *mlx);
```

### Task A5: Main Program Loop

```c
// main.c
int main(int argc, char **argv);
void render_scene(t_mlx *mlx, t_scene *scene);
void cleanup(t_mlx *mlx, t_scene *scene);
```

### Testing Methods for Student A

1. **Parser Tests**: 
  - Create various `.rt` files with valid and invalid data.
  - Write unit tests for each parsing function to ensure correct parsing and error handling.
2. **Display Tests**:
  - Create a mock render function that displays solid colors or patterns.
  - Verify window creation and image rendering using MinilibX.
  - Test keypress and window close events to ensure proper cleanup.

## 4. Student B: Ray Tracing Engine & Math Library

1. Math Library (vec3_ops.c, vec3_utils.c)
2. Ray-Object Intersections (intersections.c)
3. Lighting Calculations (lighting.c)
4. Ray Tracing Engine (raytracer.c)

### Task B1: Math Library

```c
// vec3_ops.c
t_vec3 vec3_add(t_vec3 a, t_vec3 b);
t_vec3 vec3_sub(t_vec3 a, t_vec3 b);
t_vec3 vec3_mul(t_vec3 v, double scalar);
double vec3_dot(t_vec3 a, t_vec3 b);
t_vec3 vec3_cross(t_vec3 a, t_vec3 b);
double vec3_length(t_vec3 v);
t_vec3 vec3_normalize(t_vec3 v);
```

### Task B2: Ray Generation

```c
// camera.c
t_ray generate_ray(t_camera *camera, double u, double v);
void setup_viewport(t_camera *camera, int img_width, int img_height);
```

### Task B3: Intersections & Ligting

```c
// intersections.c
typedef struct s_hit
{
    double t;         // distance along the ray
    t_vec3 point;    // hit (intersection) point
    t_vec3 normal;   // surface normal at hit point
    t_rgb color;     // color of the object hit
    int object_id; // which object was hit
    }   t_hit;

int intersect_sphere(t_ray *ray, t_sphere *sphere, t_hit *hit);
int intersect_plane(t_ray *ray, t_plane *plane, t_hit *hit);
int intersect_cylinder(t_ray *ray, t_cylinder *cylinder, t_hit *hit);
t_hit find_closest_hit(t_ray *ray, t_scene *scene);
```

### Task B4: Lighting Model

```c
// lighting.c
t_rgb calculate_lighting(t_hit *hit, t_scene *scene);
int is_in_shadow(t_vec3 point, t_light *light, t_scene *scene);
t_rgb apply_ambient_light(t_rgb color, double ratio, t_rgb ambient);
t_rgb apply_diffuse_light(t_rgb color, double intensity);
```

### Task B5: Ray Tracing Engine

```c
// raytracer.c
t_rgb trace_ray(t_ray *ray, t_scene *scene, int depth);
int *render_pixel(int x, int y, t_scene *scene, int width, int height);
```

### Testing Methods for Student B

1. **Math Library Tests**: 
  - Write unit tests for each vector operation to ensure mathematical correctness.
2. **Intersection Tests**:
  - Create test cases with known intersection points for spheres, planes, and cylinders.
  - Verify that the intersection functions return correct hit information.
3. **Visual Tests**:
  - Generate test images to file (PPM format) to verify correct rendering of scenes.
  
## 5. Testing Scenes

1. **Test 1: Single Sphere**

```c
A 0.2 255,255,255
C 0,0,0 0,0,1 70
L 0,10,0 0.7 255,255,255
sp 0,0,20 10 255,0,0
```

2. **Test 2: Three Objects**

```c
A 0.2 255,255,255
C -50,0,20 0,0,1 70
L -40,0,30 0.7 255,255,255
pl 0,-10,0 0,1,0 0,255,0
sp 0,0,20 20 255,0,0
cy 20,0,20 0,1,0 10 20 0,0,255
```

3. **Test 3: Shadow Test**

```c
A 0.2 255,255,255
C 0,0,-20 0,0,1 70
L 10,10,0 0.7 255,255,255
sp 0,0,10 5 255,0,0
sp 5,0,15 3 0,255,0
pl 0,-5,0 0,1,0 255,255,255