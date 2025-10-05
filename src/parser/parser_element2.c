/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_element2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 14:26:45 by anemet            #+#    #+#             */
/*   Updated: 2025/10/05 12:18:55 by anemet           ###   ########.fr       */
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
	if (!parse_vec3(tokens[2], &pl->normal))
		return (error_msg("Plane: invalid normal vector"));
	if (!parse_color(tokens[3], &obj->color))
		return (error_msg("Plane: invalid color format"));
	obj->type = PLANE;
	obj->shape_data = pl;
	obj->next = scene->objects;
	scene->objects = obj;
	return (1);
}

// TODO: Implementation of parse_cylinder like parse_sphere()
// Parses Cylinder: cy <x,y,z> <nx,ny,nz> <diameter> <height> <R,G,B>
int	parse_cylinder(char **tokens, t_scene *scene)
{
	(void)tokens;
	(void)scene;
	return (0);
}
