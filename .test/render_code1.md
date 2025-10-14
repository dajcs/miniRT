

This implementation covers the mandatory parts: ray generation, intersection with spheres and planes, and a lighting model with ambient light, diffuse light, and hard shadows. The cylinder intersection is left as a placeholder, as it is significantly more complex and should be tackled after the sphere and plane are working perfectly.

### 1. Ray Utilities (`src/render/ray.c`)

This small utility file provides functions for creating and manipulating rays.

```c
#include "minirt.h"

/*
** Creates a new ray with the given origin and direction.
*/
t_ray	create_ray(t_point3 origin, t_vec3 direction)
{
	t_ray	r;

	r.origin = origin;
	r.direction = direction;
	return (r);
}

/*
** Calculates the position along the ray at a distance 't'.
** This is the classic ray equation: P(t) = origin + t * direction.
*/
t_point3	ray_at(t_ray r, double t)
{
	t_vec3	scaled_direction;

	scaled_direction = vec3_mul(r.direction, t);
	return (vec3_add(r.origin, scaled_direction));
}
```

### 2. Intersections (`src/render/intersections.c`)

This file contains the logic for determining if a ray hits an object.

```c
#include "minirt.h"

// Fills the hit record with intersection data.
static void	fill_hit_record(t_hit_record *rec, t_ray *r, double t, t_object *obj)
{
	rec->t = t;
	rec->p = ray_at(*r, t);
	rec->color = obj->color;
	if (obj->type == SPHERE)
		rec->normal = vec3_normalize(vec3_sub(rec->p, \
			((t_sphere *)obj->shape_data)->center));
	else if (obj->type == PLANE)
		rec->normal = ((t_plane *)obj->shape_data)->normal;
	// If the ray hits from inside, the normal should point inwards.
	if (vec3_dot(r->direction, rec->normal) > 0.0)
		rec->normal = vec3_mul(rec->normal, -1);
}

// Solves the quadratic equation for ray-sphere intersection.
static int	solve_sphere_intersection(t_sphere *sp, t_ray *r, t_hit_record *rec)
{
	t_vec3	oc;
	double	a;
	double	half_b;
	double	c;
	double	discriminant;

	oc = vec3_sub(r->origin, sp->center);
	a = vec3_length_squared(r->direction);
	half_b = vec3_dot(oc, r->direction);
	c = vec3_length_squared(oc) - sp->radius * sp->radius;
	discriminant = half_b * half_b - a * c;
	if (discriminant < 0)
		return (0);
	// Find the nearest root that lies in an acceptable range.
	// For now, we only need the first intersection.
	rec->t = (-half_b - sqrt(discriminant)) / a;
	if (rec->t < 1e-4) // Use a small epsilon to avoid self-intersection
		rec->t = (-half_b + sqrt(discriminant)) / a;
	if (rec->t < 1e-4)
		return (0);
	return (1);
}

// Calculates intersection with a plane.
static int	hit_plane(t_plane *pl, t_ray *r, t_hit_record *rec)
{
	double	denominator;
	double	t;

	denominator = vec3_dot(pl->normal, r->direction);
	// If denominator is near zero, the ray is parallel to the plane.
	if (fabs(denominator) < 1e-6)
		return (0);
	t = vec3_dot(vec3_sub(pl->point, r->origin), pl->normal) / denominator;
	// We only care about intersections in front of the camera.
	if (t <= 1e-4)
		return (0);
	rec->t = t;
	return (1);
}

/*
** Iterates through all objects in the scene to find the closest intersection
** along the ray. Returns 1 if an object is hit, 0 otherwise.
*/
int	hit_objects(t_object *objects, t_ray *ray, t_hit_record *rec)
{
	t_hit_record	temp_rec;
	int				hit_anything;
	double			closest_so_far;

	hit_anything = 0;
	closest_so_far = INFINITY;
	while (objects)
	{
		if (objects->type == SPHERE)
		{
			if (solve_sphere_intersection(objects->shape_data, ray, &temp_rec))
			{
				if (temp_rec.t < closest_so_far)
				{
					closest_so_far = temp_rec.t;
					*rec = temp_rec;
					fill_hit_record(rec, ray, closest_so_far, objects);
					hit_anything = 1;
				}
			}
		}
		// Add else if for PLANE and CYLINDER here
		objects = objects->next;
	}
	return (hit_anything);
}
```

### 3. Lighting (`src/render/lighting.c`)

This file calculates the final color of a point based on the lights in the scene.

```c
#include "minirt.h"

// Applies ambient light contribution.
static t_color	apply_ambient_light(t_scene *scene, t_color object_color)
{
	t_color	ambient_comp;

	// Note: A more correct model would be vec3_multiply(color1, color2)
	// but for simplicity, scaling by ratio is common.
	ambient_comp = vec3_mul(object_color, scene->ambient_ratio);
	return (ambient_comp);
}

// Checks if a point is in shadow by casting a ray to the light source.
static int	is_in_shadow(t_hit_record *rec, t_light *light, t_scene *scene)
{
	t_ray			shadow_ray;
	t_hit_record	shadow_rec;
	double			light_dist;
	t_vec3			light_dir;

	light_dir = vec3_sub(light->position, rec->p);
	light_dist = vec3_length(light_dir);
	shadow_ray = create_ray(rec->p, vec3_normalize(light_dir));
	// Check for any object between the point and the light.
	if (hit_objects(scene->objects, &shadow_ray, &shadow_rec))
	{
		if (shadow_rec.t > 1e-4 && shadow_rec.t < light_dist)
			return (1); // The point is in shadow.
	}
	return (0);
}

/*
** Calculates the final color at an intersection point by summing
** the contributions of ambient and diffuse lighting.
*/
t_color	calculate_lighting(t_hit_record *rec, t_scene *scene)
{
	t_color	final_color;
	t_light	*light;
	double	diffuse_intensity;
	t_vec3	light_dir;

	final_color = apply_ambient_light(scene, rec->color);
	light = scene->lights;
	while (light)
	{
		if (!is_in_shadow(rec, light, scene))
		{
			light_dir = vec3_normalize(vec3_sub(light->position, rec->p));
			// Calculate diffuse lighting (Lambertian reflection)
			diffuse_intensity = vec3_dot(rec->normal, light_dir);
			if (diffuse_intensity < 0)
				diffuse_intensity = 0;
			final_color = vec3_add(final_color, \
				vec3_mul(rec->color, light->ratio * diffuse_intensity));
		}
		light = light->next;
	}
	// Clamp colors to ensure they are in the [0, 1] range.
	final_color.x = fmin(1.0, final_color.x);
	final_color.y = fmin(1.0, final_color.y);
	final_color.z = fmin(1.0, final_color.z);
	return (final_color);
}
```

### 4. Main Renderer (`src/render/renderer.c`)

This is the main engine that ties everything together. It iterates through each pixel, generates a ray, and determines its color.

```c
#include "minirt.h"
#include "window.h" // For my_put_pixel_to_img

// Converts a t_color (doubles from 0-1) to an integer for MiniLibX.
static int	color_to_int(t_color color)
{
	int	r;
	int	g;
	int	b;

	r = (int)(color.x * 255.999);
	g = (int)(color.y * 255.999);
	b = (int)(color.z * 255.999);
	return (r << 16 | g << 8 | b);
}

/*
** Determines the color of a single ray. If it hits an object, calculate
** lighting; otherwise, return a background color (black).
*/
static t_color	ray_color(t_ray r, t_scene *scene)
{
	t_hit_record	rec;

	if (hit_objects(scene->objects, &r, &rec))
	{
		return (calculate_lighting(&rec, scene));
	}
	// Return background color if no object is hit.
	return ((t_color){0, 0, 0});
}

// Generates the primary ray for a given pixel (x, y).
static t_ray	get_ray(t_camera *cam, int x, int y)
{
	double	u;
	double	v;
	t_vec3	pixel_center;
	t_vec3	ray_direction;

	// Convert pixel coordinates to viewport coordinates [-1, 1]
	// This simple method assumes a fixed viewport size and distance.
	// For a proper FOV implementation, this needs to be more complex.
	u = (double)x / (cam->win_width - 1);
	v = (double)y / (cam->win_height - 1);

	// A more advanced camera setup (like in the revised plan) is needed here
	// for proper FOV and orientation. This is a simplified placeholder.
	ray_direction = vec3_add(cam->pixel00_loc, vec3_add(
				vec3_mul(cam->viewport_u, u), vec3_mul(cam->viewport_v, v)));
	ray_direction = vec3_sub(ray_direction, cam->lookfrom);
	return (create_ray(cam->lookfrom, vec3_normalize(ray_direction)));
}

/*
** The main render function. It loops through every pixel, generates a ray,
** finds its color, and puts it on the image buffer.
*/
void	render(t_scene *scene, t_mlx_data *mlx)
{
	int		x;
	int		y;
	t_ray	ray;
	t_color	pixel_color;
	int		color_int;

	// setup_camera(&scene->camera); // IMPORTANT: Call a camera setup function here
	y = 0;
	while (y < scene->win_height)
	{
		x = 0;
		while (x < scene->win_width)
		{
			ray = get_ray(&scene->camera, x, y);
			pixel_color = ray_color(ray, scene);
			color_int = color_to_int(pixel_color);
			my_put_pixel_to_img(mlx, x, y, color_int);
			x++;
		}
		y++;
	}
}
```

**Important Next Step:** The `get_ray` function provided is a simplified placeholder. You will need to implement a `setup_camera` function (as outlined in the revised collaboration plan) that pre-calculates the camera's viewport vectors based on the FOV and orientation. This function should be called once in `render()` before the main loops begin.
