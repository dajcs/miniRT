/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_element2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 14:26:45 by anemet            #+#    #+#             */
/*   Updated: 2025/10/05 21:36:14 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

// Parses Plane: pl <x,y,z> <nx,ny,nz> <R,G,B>
int	parse_plane(char **tokens, t_scene *scene)
{
	t_object	*obj;
	t_plane		*pl;

	if (count_tokens(tokens) != 4)
		return (error_msg("Plane: requires 3 parameters"));
	obj = malloc(sizeof(t_object));
	pl = malloc(sizeof(t_plane));
	if (!obj || !pl)
		return (error_msg("Plane: memory allocation failed"));
	if (!parse_vec3(tokens[1], &pl->point))
		return (error_msg("Plane: invalid point coordinates"));
	if (!parse_vec3(tokens[2], &pl->normal) || !validate_norm_vec3(pl->normal))
		return (error_msg("Plane: invalid normal vector"));
	if (!parse_color(tokens[3], &obj->color))
		return (error_msg("Plane: invalid color format"));
	obj->type = PLANE;
	obj->shape_data = pl;
	obj->next = scene->objects;
	scene->objects = obj;
	return (1);
}

// Parses Cylinder: cy <x,y,z> <nx,ny,nz> <diameter> <height> <R,G,B>
int	parse_cylinder(char **tokens, t_scene *scene)
{
	t_object	*obj;
	t_cylinder	*cy;

	if (count_tokens(tokens) != 6)
		return (error_msg("Cylinder: requires 5 parameters"));
	obj = malloc(sizeof(t_object));
	cy = malloc(sizeof(t_cylinder));
	if (!obj || !cy)
		return (error_msg("Cylinder: memory allocation failed"));
	if (!parse_vec3(tokens[1], &cy->center))
		return (error_msg("Cylinder: invalid center coordinates"));
	if (!parse_vec3(tokens[2], &cy->axis) || !validate_norm_vec3(cy->axis))
		return (error_msg("Cylinder: invalid orientation vector"));
	if (!parse_double(tokens[3], &cy->diameter) || cy->diameter <= 0.0)
		return (error_msg("Cylinder: invalid diameter"));
	if (!parse_double(tokens[4], &cy->height) || cy->height <= 0.0)
		return (error_msg("Cylinder: invalid height"));
	if (!parse_color(tokens[5], &obj->color))
		return (error_msg("Cylinder: invalid color format"));
	obj->type = CYLINDER;
	obj->shape_data = cy;
	obj->next = scene->objects;
	scene->objects = obj;
	return (1);
}
