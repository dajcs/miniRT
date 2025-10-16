/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_element2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 14:26:45 by anemet            #+#    #+#             */
/*   Updated: 2025/10/16 10:35:16 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

// Parses Plane: pl <x,y,z> <nx,ny,nz> <R,G,B>
int	parse_plane(char **tokens, t_scene *scene)
{
	t_object	*obj;
	t_plane		*pl;

	if (count_tokens(tokens) < 4)
		return (error_msg("Plane: requires 3 parameters"));
	obj = malloc(sizeof(t_object));
	pl = malloc(sizeof(t_plane));
	if (!obj || !pl)
		return (obj_err("Plane: memory allocation failed", obj, pl));
	if (!parse_vec3(tokens[1], &pl->point, 0))
		return (obj_err("Plane: invalid point coordinates", obj, pl));
	if (!parse_vec3(tokens[2], &pl->normal, 1)
		|| !validate_norm_vec3(pl->normal))
		return (obj_err("Plane: invalid normal vector", obj, pl));
	if (!parse_color(tokens[3], &obj->color))
		return (obj_err("Plane: invalid color format", obj, pl));
	obj->type = PLANE;
	obj->shape_data = pl;
	obj->next = scene->objects;
	scene->objects = obj;
	return (1);
}

// Parses Cylinder: cy <x,y,z> <ax,ay,az> <diameter> <height> <R,G,B>
// in rendering we're considering the base cap middle as cylinder center
// parsing cylinder center is corrected with
// cy->center = vec3_sub(cy->center, vec3_mul(cy->axis, cy->height / 2.0));
int	parse_cylinder(char **tokens, t_scene *scene)
{
	t_object	*obj;
	t_cylinder	*cy;

	if (count_tokens(tokens) < 6)
		return (error_msg("Cylinder: requires 5 parameters"));
	obj = malloc(sizeof(t_object));
	cy = malloc(sizeof(t_cylinder));
	if (!obj || !cy)
		return (obj_err("Cylinder: memory allocation failed", obj, cy));
	if (!parse_vec3(tokens[1], &cy->center, 0))
		return (obj_err("Cylinder: invalid center coordinates", obj, cy));
	if (!parse_vec3(tokens[2], &cy->axis, 1) || !validate_norm_vec3(cy->axis))
		return (obj_err("Cylinder: invalid orientation vector", obj, cy));
	if (!parse_double(tokens[3], &cy->diameter) || cy->diameter <= 0.0)
		return (obj_err("Cylinder: invalid diameter", obj, cy));
	if (!parse_double(tokens[4], &cy->height) || cy->height <= 0.0)
		return (obj_err("Cylinder: invalid height", obj, cy));
	if (!parse_color(tokens[5], &obj->color))
		return (obj_err("Cylinder: invalid color format", obj, cy));
	cy->center = vec3_sub(cy->center, vec3_mul(cy->axis, cy->height / 2.0));
	obj->type = CYLINDER;
	obj->shape_data = cy;
	obj->next = scene->objects;
	scene->objects = obj;
	return (1);
}
