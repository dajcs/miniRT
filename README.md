# miniRT
miniRT (mini Ray-Tracing) project @ school 42

### RT Algorithms

- Path Tracing (Hollywood movies)
- Ray Casting (simplest, 1 ray - miniRT mandatory part)
- Recursive Ray Tracing (ray reflections - miniRT bonus part)
- Distributed Ray Tracing

### Resources

- [Ray Tracing in 2 minutes](https://www.youtube.com/watch?v=oCsgTrGLDiI)
- [Ray Tracing in One Weekend (book)](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
- [Ray Tracing: The Next Week (book)](https://raytracing.github.io/books/RayTracingTheNextWeek.html)
- [Ray Tracing: The Rest of Your Life (book)](https://raytracing.github.io/books/RayTracingTheRestOfYourLife.html)
- [Building a miniRT — 42 Project Part 1 (medium)](https://medium.com/@iremoztimur/building-a-minirt-42-project-part-1-ae7a00aebdb9)
- [Building a miniRT — 42 Project Part 2 (medium)](https://medium.com/@iremoztimur/minirt-shading-algorithms-part-2-6c46e6d81fbd)
- [Raycasting Algorithms Part 1](https://www.youtube.com/watch?v=ebzlMOw79Yw)
<!---
- [Computerphile: Recursive Ray Tracing](https://www.youtube.com/watch?v=nOCPpT-Sn0A)
- [Computerphile: Path Tracing](https://www.youtube.com/watch?v=3OKj0SQ_UTw)
- [Gemini 2.5 Pro](https://aistudio.google.com/prompts/1vRVehQahgudJQ6T0p-WscGrNWDqiiD2H)
-->
- [Cem Yuksel - Intro to CG: Chapter 19. Ray Tracing](https://www.youtube.com/watch?v=gGKup9tUSrU&list=PLplnkTzzqsZTfYh4UbhLGpI5kGd5oW_Hh&index=20)


## What is doing our miniRT?

- Parsing a simple scene description file (.rt) and creating a 3D scene with:
  - One ambient light
  - One camera
  - Multiple light sources
  - Multiple objects (spheres, planes, cylinders)
  - We're placing these items in the 3D scene with coordinates:
```

    +Y (Up)
          |
          |
          |
          +----------------> +X (Right)
         /
        /
       /
      +Z (Towards you)

```

- Rendering the scene using Ray Tracing:
  - We put an imaginary rectangle (called viewport) in front of the camera.
  - For each pixel in the viewport, we generate a ray from the camera through that pixel.
  - We check if that ray intersects with any object in the scene.
  - If doesn't hit any object, the pixel will be colored with a background color (black).
  - If it hits an object, that pixel's color is determined by the Phong illumination model:
  	1. The object's color in ambient light
	2. The object's color in diffuse light
	    - From the hit point, we generate a ray to each light source.
	    - If that ray hits another object before reaching the light source, that light is blocked (in shadow).
	    - If the ray reaches the light source, we calculate the diffuse contribution based on the angle between the light direction and the surface normal.
	3. The object's color in specular light (bonus part)
	    - We calculate the reflection direction of the light ray on the surface.
	    - We calculate the specular contribution based on the angle between the reflection direction and the view direction.
  - We combine the ambient + diffuse + specular contributions to get the final color of the pixel.

---

### Bonus part

Implemented functions:
- multiple light sources with color
- specular reflection (the 3rd component of the Phong model)
- checkerboard pattern basic implementation ([improved version](.test/checker_dust_uv_solution.md))
- one other second degree object: cone
- ray reflections with recursive ray tracing

Possible additions:
- [bump map textures](.test/bump_map_texture.md) (to visualize objects with uneven surfaces)
- [refraction](.test/refraction.md) (to visualize transparent/semi-transparent objects)

Quiz:
- Is `reflectivity` a combination of `specular` + `shininess`? ( [answer](.test/reflectivity_specular_shininess.md) )

## miniRT Project Collaboration Plan for 2 Students

### 1. Overview & Philosophy

This plan refines the initial component division.
- **Student A** will focus on the **"front-end"** – parsing the scene description and getting it to the screen.
- **Student B** will own the **"back-end"** – the core rendering logic that turns the scene data into pixels.

This clean separation minimizes dependencies and allows for independent testing.

**Key Principle:** "Code to an interface." Student A and Student B should agree on the exact definitions of the shared data structures in `minirt.h` *first*. As long as those structures don't change, each student can work on their part with confidence.

### 2. Refined Shared Data Structures (`include/minirt.h`)

Here are some suggested improvements to the initial structures (changed in the final version).

```c
#ifndef MINIRT_H
# define MINIRT_H


// Using t_vec3 for points, vectors, and colors for simplicity
typedef struct s_vec3
{
	double			x;
	double			y;
	double			z;
}					t_vec3;

typedef t_vec3	t_point3;	// alias for 3D point
typedef t_vec3	t_color;	// alias for RGB color (0-1.0 range)

// A ray is defined by an origin and a direction vector
typedef struct s_ray
{
	t_point3		origin;
	t_vec3			direction;
}					t_ray;

// Camera definition. Pre-calculating viewport values is highly recommended
typedef struct s_camera
{
	t_point3		origin;	// The camera's position (parsed 'C' coordinates)
	t_vec3			orientation;	// The point the camera is looking at
	double			fov;	// Horizontal field of view in degrees
	// --- Pre-calculated values for rendering ---
	t_vec3			u;		// Camera coordinate system basis vectors
	t_vec3			v;		// Camera coordinate system basis vectors
	t_vec3			w;		// Camera coordinate system basis vectors
	double			viewport_height;
	double			viewport_width;
	t_point3		viewport_u;	// Horizontal vector of the viewport
	t_point3		viewport_v;	// Vertical vector of the viewport
	t_point3		pixel00_loc;	// Location of the top-left pixel
}					t_camera;

// Use a linked list for lights and objects for easier parsing
typedef struct s_light
{
	t_point3		position; // Parsed 'L' coordinates
	double			ratio;	// Light brightness ratio (0.0 - 1.0)
	t_color			color;	// Parsed 'L' color (0-255 range)
	struct s_light	*next;
}					t_light;

// Generic object structure with type and data pointer
// This simplifies storage and traversal
typedef enum e_obj_type
{
	SPHERE,
	PLANE,
	CYLINDER
}					t_obj_type;

typedef struct s_object
{
	t_obj_type		type;
	t_color			color;	// Parsed object color (0-255 range)
	void			*shape_data; // Pointer to one of the structs below
	struct s_object	*next;
}					t_object;

typedef struct s_sphere
{
	t_point3		center; // Parsed 'sp' coordinates
	double			radius;	// Parsed 'sp' diameter / 2
}					t_sphere;

typedef struct s_plane
{
	t_point3		point; // A point on the plane (parsed 'pl' coordinates)
	t_vec3			normal;	// Normal vector (parsed 'pl' orientation vector,
					// (should be normalized)
}					t_plane;

typedef struct s_cylinder
{
	t_point3		center; // Center of the cylinder base (parsed 'cy' coords)
	t_vec3			axis;	// Axis vector (parsed 'cy' orientation vector,
							// should be normalized)
	double			diameter; // Parsed 'cy' diameter
	double			height;	// Parsed 'cy' height
}					t_cylinder;

// The main scene structure
typedef struct s_scene
{
	int				width;
	int				height;
	t_color			ambient_light;	// Ambient light color (0-255 range)
	double			ambient_ratio;	// Ambient light ratio (0.0 - 1.0)
	int				has_ambient;	// Flag to ensure only one ambient light
	t_camera		camera;
	int				has_camera;		// Flag to ensure only one camera
	t_light			*lights;		// Linked list of lights
	t_object		*objects;		// Linked list of objects
}					t_scene;

// Record of a ray-object intersection
typedef struct s_hit_record
{
	t_point3		p;		// Point of intersection
	t_vec3			normal;	// Surface normal at the intersection
	t_color			color;	// Color of the object hit
	double			t;		// 'time' or distance along the ray
	int				front_face;	// 1 if ray hits from outside, 0 if from inside
}					t_hit_record;

// --- Window management ---

// Holds all data related to the MiniLibX window and image buffer
typedef struct s_mlx_data
{
	void			*mlx_ptr;	// Pointer to the MLX instance
	void			*win_ptr;	// Pointer to the window
	void			*img_ptr;	// Pointer to the image buffer
	char			*addr;		// Address of the image data
	int				bits_per_pixel; // Bits per pixel in the image
	int				line_length;	// Length of a line in bytes
	int				endian;	// Endianness of the image data
}					t_mlx_data;

// A master struct to hold pointers to all major components of the program
typedef struct s_program_data
{
	t_scene			*scene;
	t_mlx_data		*mlx;
}					t_program_data;

#endif
```

### 3. Student A: Scene Parsing & Display System

**Goal:** Read a `.rt` file, validate it, populate the `t_scene` struct, and set up a window to display the final image.

#### **Module 1: Parser (`src/parser/`)**

*   **`parser.c`**: Main parsing logic.
    *   `t_scene *parse_scene(const char *filename)`: Orchestrates the parsing process. It opens the file, reads it line-by-line, and calls the appropriate sub-parser for each element type.
    *   `void free_scene(t_scene *scene)`: Essential for clean exit. Frees all allocated memory (objects, lights, etc.).

*   **`parser_utils.c`**: Helper functions for parsing individual data types. These are crucial for robust error checking.
    *   `int parse_vec3(char *str, t_vec3 *vec)`: Parses "x,y,z" into a `t_vec3`. Returns success/failure.
    *   `int parse_color(char *str, t_color *color)`: Parses "R,G,B" and normalizes values to the `[0, 1]` range (e.g., `255` -> `1.0`). This simplifies lighting calculations for Student B.
    *   `double ft_atof(char *str)`: A robust `atof` that can handle parsing errors. `libft` might need this.

*   **`parser_elements.c`**: Functions to parse each specific scene element.
    *   `void parse_ambient(char **tokens, t_scene *scene)`
    *   `void parse_camera(char **tokens, t_scene *scene)`
    *   `void parse_light(char **tokens, t_scene *scene)`
    *   `void add_object(t_scene *scene, t_object *new_obj)`: A helper to add a new object to the front of the `objects` linked list.
    *   `void parse_sphere(char **tokens, t_scene *scene)`: Allocates `t_object` and `t_sphere`, populates them, and calls `add_object`.
    *   `void parse_plane(char **tokens, t_scene *scene)`
    *   `void parse_cylinder(char **tokens, t_scene *scene)`

#### **Module 2: Window Management (`src/window/`)**

*   **`window.c`**: Interfacing with `minilibX`.
    *   `t_mlx_data *init_window(int width, int height, char *title)`: Initializes MLX, creates a window and an image buffer.
    *   `void my_put_pixel_to_img(t_mlx_data *data, int x, int y, int color)`: Puts a single pixel into the image buffer.
    *   `int color_to_int(t_color color)`: Converts your `t_color` (with `double`s from 0-1) back to an integer format that MLX can display.

*   **`hooks.c`**: Event handling.
    *   `int key_hook(int keycode, t_program_data *data)`: Handles `Esc` key press.
    *   `int close_hook(t_program_data *data)`: Handles clicking the red cross.
    *   `void setup_hooks(t_program_data *data)`: Registers the hook functions with MLX.

#### **Module 3: Main (`src/main.c`)**

*   `main()`:
    1.  Validate command-line arguments (`argc`, `argv`, file extension).
    2.  Call `parse_scene()` to populate the `t_scene` struct.
    3.  If parsing succeeds, call `init_window()`.
    4.  Call the main `render()` function (from Student B's part).
    5.  Call `mlx_put_image_to_window()`.
    6.  Call `setup_hooks()`.
    7.  Call `mlx_loop()`.

**Testing Strategy for Student A:**

1.  **Unit Test Parsers:** Create a suite of `.rt` files: one correct, and many incorrect ones (wrong number of args, values out of range, bad formatting) to test every error path.
2.  **Mock Renderer:** Write a temporary `render()` function that fills the screen with a single color or a gradient. This allows you to test the entire pipeline from file parsing to window display without needing Student B's code.

### 4. Student B: Ray Tracing Engine

**Goal:** Implement the mathematical and rendering logic to compute the color of each pixel based on the scene description.

#### **Module 1: Math Library (`src/math/`)**

This is the foundation. It must be flawless.

*   **`vec3_ops1.c`**: Basic operations.
    *   `t_vec3 vec3_add(t_vec3 v1, t_vec3 v2)`
    *   `t_vec3 vec3_sub(t_vec3 v1, t_vec3 v2)`
    *   `t_vec3 vec3_mul(t_vec3 v, double t)` (scalar multiplication)
    *   `t_vec3 vec3_div(t_vec3 v, double t)` (scalar division)
    *   `double vec3_dot(t_vec3 v1, t_vec3 v2)`

*   **`vec3_ops2.c`**: Advanced operations.
    *   `double vec3_length_squared(t_vec3 v)`: (Faster than `length` as it avoids `sqrt`)
    *   `double vec3_length(t_vec3 v)`
    *   `t_vec3 vec3_normalize(t_vec3 v)`
    *   `t_vec3 vec3_cross(t_vec3 u, t_vec3 v)`

#### **Module 2: Ray Tracing (`src/render/`)**

*   **`ray.c`**:
    *   `t_ray create_ray(t_point3 origin, t_vec3 direction)`
    *   `t_point3 ray_at(t_ray r, double t)`: Calculates the point along the ray at distance `t`.

*   **`intersections.c`**:
    *   `int hit_object(t_object *obj, t_ray *ray, double t_min, double t_max, t_hit_record *rec)`: A dispatcher function that calls the correct intersection function below based on `obj->type`.
    *   `int hit_sphere(t_sphere *sp, t_ray *ray, ...)`: Implements the ray-sphere intersection formula.
    *   `int hit_plane(t_plane *pl, t_ray *ray, ...)`
    *   `int hit_cylinder(t_cylinder *cy, t_ray *ray, ...)`: This is the most complex one. Break it into parts: intersecting with the infinite cylinder walls, and intersecting with the top/bottom caps (which are planes).

*   **`camera.c`**:
    *   `void setup_camera(t_camera *cam, int img_width, int img_height)`: This is a crucial function. It takes the parsed FOV and orientation vectors and pre-calculates the `viewport_*` and basis vectors (`u,v,w`) needed for rendering. This should be called once after parsing.

*   **`renderer.c`**: The main engine.
    *   `t_color ray_color(t_ray r, t_scene *scene)`: This is the core recursive (or in this case, iterative for the mandatory part) function.
        1.  Initialize a `t_hit_record`.
        2.  Loop through all `objects` in the scene, calling `hit_object` for each one to find the *closest* intersection.
        3.  If there is a hit:
            a. Call `calculate_lighting()` to get the final color.
            b. Return that color.
        4.  If there is no hit, return a background color (e.g., black or a simple sky gradient).
    *   `void render(t_scene *scene, t_mlx_data *mlx)`: The top-level rendering function called by `main`.
        1.  It loops through every pixel (`y` from 0 to height, `x` from 0 to width).
        2.  For each pixel, it calculates the ray direction using the pre-calculated camera values.
        3.  It calls `ray_color()` to get the color for that ray.
        4.  It converts the `t_color` to an `int` and calls `my_put_pixel_to_img()`.

*   **`lighting.c`**:
    *   `t_color calculate_lighting(t_hit_record *rec, t_scene *scene)`:
        1.  Start with the ambient light: `final_color = vec3_mul(scene->ambient_light, scene->ambient_ratio)`.
        2.  Loop through every `light` in the scene:
            a. Calculate the direction vector from the hit point (`rec->p`) to the light's position.
            b. **Shadows:** Create a "shadow ray" from the hit point towards the light. Call the intersection logic again to see if any other object is between the point and the light. If so, `continue` to the next light.
            c. **Diffuse:** Calculate the dot product of the light vector and the surface normal (`rec->normal`). If it's positive, add the diffuse contribution to `final_color`. The contribution is `light_color * light_ratio * dot_product * object_color`.
        3.  Return the clamped `final_color` (ensure RGB values don't exceed 1.0).

**Testing Strategy for Student B:**

1.  **Unit Test Math:** Write a separate `main` to test all vector functions with known inputs and outputs.
2.  **Test Intersections:** Write tests that create specific rays and objects and verify that `hit_object` returns the correct intersection point `t` and normal vector.
3.  **PPM Image Output:** Instead of relying on `minilibX`, create a function that writes the rendered image to a `.ppm` file. This is a simple text-based image format. It allows you to test the entire rendering engine on your machine without needing Student A's part to be complete. You can view the `.ppm` files with a simple image viewer.


### [Start with the Rendering Code](.test/render_code1.md)

