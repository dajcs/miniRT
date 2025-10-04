/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 18:30:51 by anemet            #+#    #+#             */
/*   Updated: 2025/10/04 19:24:05 by anemet           ###   ########.fr       */
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
t_color	ray_color(t_ray *ray, t_scene *scene)
{
	t_hit_record	rec;
	t_hit_record	temp_rec;
	t_object		*obj;
	int				hit_anything;
	double			closest_so_far;

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
	if (hit_anything)
		return (calculate_lighting(&rec, scene));
	return ((t_color){0, 0, 0});
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
			pixel_color = ray_color(&r, scene);
			color_int = color_to_int(pixel_color);
			my_put_pixel_to_img(mlx, x, y, color_int);
			x++;
		}
		y++;
	}
}
