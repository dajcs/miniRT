/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 18:30:51 by anemet            #+#    #+#             */
/*   Updated: 2025/10/15 14:38:52 by anemet           ###   ########.fr       */
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

/* hit_anything()
	Iterate through all objects of the scene to determine if ray hits anything
	Input:
		*ray:	the ray we're testing
		*scene:	the scene with the objects
		*rec:	the hit_record which should be set if anything is hit
	Return 1 if something is hit, 0 otherwise

	1. Initiate hit=0, starting obj, closest_so_far=DBL_MAX
	2. Iterate through all objects
	3.	if hit_object:
			- update closest_so_far
			- copy temp_rec data into callers record *rec
			- set hit to 1
	4. Return hit.
*/
static int	hit_anything(t_ray *ray, t_scene *scene, t_hit_record *rec)
{
	t_hit_record	temp_rec;
	t_object		*obj;
	double			closest_so_far;
	int				hit;

	hit = 0;
	obj = scene->objects;
	closest_so_far = DBL_MAX;
	while (obj)
	{
		if (hit_object(obj, ray, closest_so_far, &temp_rec))
		{
			closest_so_far = temp_rec.t;
			*rec = temp_rec;
			hit = 1;
		}
		obj = obj->next;
	}
	return (hit);
}

/* ray_color()
	Recursive: traces a ray and eventual reflections and determines the
				resulting color.
	Input:
		*ray:	the ray to trace
		*scene:	the scene containing all objects
		depth:	recursion depth
	Return: the computed t_color for the ray

	This is the core of the ray tracer. It iterates through every object in the
	scene to find the closest intersection point along the ray.
	If the ray hits nothing, it returns a background color (black)
	If an intersection occurs,
		it calls calculate_lighting() to shade that point.
	If the object is reflective: recursive calling itself to determine the
	reflected color contribution.

	0. BASE CASE: Stop recursion if we've bounced too many times, return black
	1. Find closest hit with hit_anything()
		If nothing hit, return black
		If something hit, the hit_record rec is set
	2. STEP 1: Calculate the object's own color (Local Illumination)
		this is the color of the surface itself, based on diffuse and specular
		ligthing
	3. STEP 2: If object is reflective: calculate the reflected color
		- calculate the direction of the reflected ray with vec3_reflect()
		- new ray origin at hit point + small offset to avoid self intersection
		- RECURSIVE CALL: checking what the reflection ray sees by calling this
			same function with the new `reflection_ray` and `depth - 1`
		If the object is not reflective return black (no reflected color)
	4. STEP 3: Combine the local and reflected colors
		The final color is a blend, controlled by the object's reflectivity.
		A perfect mirror (reflectivity=1) shows only the reflected_color.
		A normal object (reflectivity=0) shows only the local_color.
*/
static t_color	ray_color(t_ray *ray, t_scene *scene, int depth)
{
	t_hit_record	rec;
	t_color			local_color;
	t_color			reflected_color;
	t_ray			reflection_ray;

	if (depth <= 0)
		return ((t_color){0, 0, 0});
	if (!hit_anything(ray, scene, &rec))
		return ((t_color){0, 0, 0});
	local_color = calculate_lighting(&rec, scene);
	if (rec.reflect > 0)
	{
		reflection_ray.direction = vec3_reflect(ray->direction, rec.normal);
		reflection_ray.origin = vec3_add(rec.p,
				vec3_mul(reflection_ray.direction, 0.001));
		reflected_color = ray_color(&reflection_ray, scene, depth - 1);
	}
	else
		reflected_color = (t_color){0, 0, 0};
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
