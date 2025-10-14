/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 18:30:51 by anemet            #+#    #+#             */
/*   Updated: 2025/10/14 16:04:49 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

/* color_to_int()
	Converts a t_color (doubles from 0-1) to a 32-bit integer color
	Input:
		color:	the t_color to be converted
	Return: an integer in 0x00RRGGBB format for minilibX

	This is a utility function to bridge the gap between the renderer's
	floating-point color representation and the integer-based color system
	that minilibX's image buffer requires.
*/
int	color_to_int(t_color color)
{
	int	r;
	int	g;
	int	b;

	r = (int)(255.999 * color.x);
	g = (int)(255.999 * color.y);
	b = (int)(255.999 * color.z);
	return ((r << 16) | (g << 8) | b);
}

/* ray_color()
	Traces a single ray and determines the color it sees.
	Input:
		*ray:	the ray to trace
		*scene:	the scene containing all objects
	Return: the computed t_color for the ray

	This is the core of the ray tracer. It iterates through every object in the
	scene to find the closest intersection point along the ray.
	If an intersection occurs, it calls calculate_lighting() to shade that point.
	If the ray hits nothing, it returns a background color (black)
*/
static t_color	ray_color(t_ray *ray, t_scene *scene, int depth)
{
	t_hit_record	rec;
	t_hit_record	temp_rec;
	t_object		*obj;
	int				hit_anything;
	double			closest_so_far;
	t_color			local_color;
	t_color			reflected_color;

	// --- BASE CASE: Stop recursion if we've bounced too many times. ---
	if (depth <= 0)
		return ((t_color){0, 0, 0}); // return black

	// find closest hit
	hit_anything = 0;
	closest_so_far = DBL_MAX;
	obj = scene->objects;
	while (obj)
	{
		if (hit_object(obj, ray, closest_so_far, &temp_rec))
		{
			hit_anything = 1;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
		obj = obj->next;
	}

	// if nothing hit, return black
	if (!hit_anything)
		return ((t_color){0, 0, 0});

	// --- STEP 1: Calculate the object's own color (Local Illumination) ---
	// this is the color of the surface itself, based on diffuse and specular
	// lighting.
	local_color = calculate_lighting(&rec, scene);

	// --- STEP 2: Calculate the reflected color (if object is reflective) ---
	if (rec.reflect > 0)
	{
		t_ray	reflection_ray;

		// Calculate the direction of the reflected ray
		reflection_ray.direction = vec3_reflect(ray->direction, rec.normal);
		// The new ray starts at the hit point (with a small offset to avoid self intersection)
		reflection_ray.origin = vec3_add(rec.p, vec3_mul(reflection_ray.direction, 0.001));

		// --- RECURSIVE CALL ---
		// checking what the reflection ray sees by calling this same function
		reflected_color = ray_color(&reflection_ray, scene, depth - 1);
	}
	else
		reflected_color = (t_color){0, 0, 0};  // not reflective / no reflected color

	// --- STEP 3: Combine the local and reflected colors ---
	// The final color is a blend, controlled by the object's reflectivity.
	// A perfect mirror (reflectivity=1) shows only the reflected_color.
	// A normal object (reflectivity=0) shows only the local_color.
	return (vec3_add(vec3_mul(local_color, 1.0 - rec.reflect),
		vec3_mul(reflected_color, rec.reflect)));

}

/* render()
	Input:
		*scene:		the fully parsed scene to be rendered
		*mlx		the minilibX data structure containing the image buffer
	Return: void, fill the buffer in place

	This function iterates through every pixel of the image buffer. For each
	pixel it calls setup_camera() to generate a primary ray. It then calls
	`ray_color()` to find the color for that ray, converts the color to an
	integer, and places it in the image buffer using `my_put_pixel_to_img()`.
*/
void	render(t_scene *scene, t_mlx_data *mlx)
{
	int		x;
	int		y;
	t_ray	r;
	t_color	pixel_color;
	int		color_int;

	setup_camera(&scene->camera, scene->width, scene->height);
	y = 0;
	while (y < scene->height)
	{
		x = 0;
		while (x < scene->width)
		{
			r = get_ray(&scene->camera, x, y);
			pixel_color = ray_color(&r, scene, MAX_DEPTH);
			color_int = color_to_int(pixel_color);
			my_put_pixel_to_img(mlx, x, y, color_int);
			x++;
		}
		y++;
	}
}
