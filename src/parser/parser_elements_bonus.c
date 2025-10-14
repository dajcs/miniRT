/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_elements_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 13:24:03 by anemet            #+#    #+#             */
/*   Updated: 2025/10/14 15:05:54 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

// Parses Ambient light: A <ratio> <R,G,B>
int	parse_ambient(char **tokens, t_scene *scene)
{
	double	ratio;
	t_color	color;

	if (count_tokens(tokens) < 3)
		return (error_msg("Ambient light: requires 2 parameters"));
	if (scene->has_ambient)
		return (error_msg("Ambient light: declared more than once"));
	if (!parse_double(tokens[1], &ratio) || !validate_ratio(ratio))
		return (error_msg("Ambient light: invalid ratio"));
	if (!parse_color(tokens[2], &color))
		return (error_msg("Ambient light: invalid color format"));
	scene->ambient_ratio = ratio;
	scene->ambient_light = color;
	scene->has_ambient = 1;
	return (1);
}

// Parses Camera: C <x,y,z> <nx,ny,nz> <fov>
int	parse_camera(char **tokens, t_scene *scene)
{
	t_point3	origin;
	t_vec3		orientation;
	double		fov;

	if (count_tokens(tokens) < 4)
		return (error_msg("Camera: requires 3 parameters"));
	if (scene->has_camera)
		return (error_msg("Camera: declared more than once"));
	if (!parse_vec3(tokens[1], &origin, 0))
		return (error_msg("Camera: invalid origin coordinates"));
	if (!parse_vec3(tokens[2], &orientation, 1)
		|| !validate_norm_vec3(orientation))
		return (error_msg("Camera: invalid orientation vector"));
	if (!parse_double(tokens[3], &fov) || !validate_fov(fov))
		return (error_msg("Camera: invalid FOV"));
	scene->camera.origin = origin;
	scene->camera.orientation = orientation;
	scene->camera.fov = fov;
	scene->has_camera = 1;
	return (1);
}

// Parses Light: L <x,y,z> <ratio> <R,G,B> (Color is ignored for mandatory part)
int	parse_light(char **tokens, t_scene *scene)
{
	t_light		*light;
	t_point3	pos;
	double		ratio;
	t_color		color;

	if (count_tokens(tokens) < 4)
		return (error_msg("Light: requires 3 parameters"));
	light = malloc(sizeof(t_light));
	if (!light)
		return (error_msg("Light: memory allocation failed"));
	if (!parse_vec3(tokens[1], &pos, 0))
		return (error_msg("Light: invalid position coordinates"));
	if (!parse_double(tokens[2], &ratio) || !validate_ratio(ratio))
		return (error_msg("Light: invalid brightness ratio"));
	if (!parse_color(tokens[3], &color))
		return (error_msg("Light: invalid color format"));
	light->position = pos;
	light->ratio = ratio;
	light->color = color;
	light->next = scene->lights;
	scene->lights = light;
	return (1);
}

// parsing the material propoerties of the objects
// <color> <specular> <shininess> <checker flag> <color2> <pattern_scale>
int	set_material(t_object *obj, char **tokens, int i)
{
	if (!parse_color(tokens[i++], &obj->color))
		return (error_msg("Invalid color format"));
	if (!parse_double(tokens[i++], &obj->speci) || !validate_ratio(obj->speci))
		return (error_msg("Specular intensity must be in [0,1]"));
	if (!parse_double(tokens[i++], &obj->shine) || obj->shine < 1.0)
		return (error_msg("Shininess must be >= 1.0"));
	if (!parse_int(tokens[i++], &obj->checker) || !obj->checker)
		return (1);
	if (obj->checker == CHECKER)
	{
		if (!parse_color(tokens[i++], &obj->color2))
			return (error_msg("Invalid 2nd color format"));
		if (!parse_double(tokens[i], &obj->pattern_scale)
			|| obj->pattern_scale <= 0)
			return (error_msg("Pattern scale should be > 0"));
	}
	else if (obj->checker == MIRROR)
	{
		if (!parse_double(tokens[i++], &obj->reflect)
			|| !validate_ratio(obj->reflect))
			return (error_msg("Reflectivity must be in [0,1]"));
	}
	return (1);
}

// Parses Sphere: sp <x,y,z> <diameter> <R,G,B> <specular intens.> <shininess>
int	parse_sphere(char **tokens, t_scene *scene)
{
	t_object	*obj;
	t_sphere	*sp;

	if (count_tokens(tokens) < 6)
		return (error_msg("Sphere: bonus part requires 5+ parameters"));
	obj = malloc(sizeof(t_object));
	sp = malloc(sizeof(t_sphere));
	if (!obj || !sp)
		return (error_msg("Sphere: memory allocation failed"));
	if (!parse_vec3(tokens[1], &sp->center, 0))
		return (error_msg("Sphere: invalid center coordinates"));
	if (!parse_double(tokens[2], &sp->radius) || sp->radius <= 0)
		return (error_msg("Sphere: invalid diameter"));
	if (!set_material(obj, tokens, 3))
		return (error_msg("Sphere: `set_material()` error"));
	sp->radius /= 2.0;
	obj->type = SPHERE;
	obj->shape_data = sp;
	obj->next = scene->objects;
	scene->objects = obj;
	return (1);
}
