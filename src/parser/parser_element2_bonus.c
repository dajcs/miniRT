/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_element2_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 14:26:45 by anemet            #+#    #+#             */
/*   Updated: 2025/10/13 10:45:29 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

// Parses Plane: pl <x,y,z> <nx,ny,nz> <R,G,B> <specular> <shine>
int	parse_plane(char **tokens, t_scene *scene)
{
	t_object	*obj;
	t_plane		*pl;

	if (count_tokens(tokens) < 6)
		return (error_msg("Plane: bonus part requires 5+ parameters"));
	obj = malloc(sizeof(t_object));
	pl = malloc(sizeof(t_plane));
	if (!obj || !pl)
		return (error_msg("Plane: memory allocation failed"));
	if (!parse_vec3(tokens[1], &pl->point, 0))
		return (error_msg("Plane: invalid point coordinates"));
	if (!parse_vec3(tokens[2], &pl->normal, 1)
		|| !validate_norm_vec3(pl->normal))
		return (error_msg("Plane: invalid normal vector"));
	if (!set_material(obj, tokens, 3))
		return (error_msg("Plane: `set_material()` error"));
	obj->type = PLANE;
	obj->shape_data = pl;
	obj->next = scene->objects;
	scene->objects = obj;
	return (1);
}

// Parses Cylinder: cy <x,y,z> <ax,ay,az> <diameter> <height>
// set_material: [color] [specular] [shininess] [checker] [color2] [patter]
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
		return (error_msg("Cylinder: memory allocation failed"));
	if (!parse_vec3(tokens[1], &cy->center, 0))
		return (error_msg("Cylinder: invalid center coordinates"));
	if (!parse_vec3(tokens[2], &cy->axis, 1) || !validate_norm_vec3(cy->axis))
		return (error_msg("Cylinder: invalid axis orientation vector"));
	if (!parse_double(tokens[3], &cy->diameter) || cy->diameter <= 0.0)
		return (error_msg("Cylinder: diameter should be > 0"));
	if (!parse_double(tokens[4], &cy->height) || cy->height <= 0.0)
		return (error_msg("Cylinder: height should be > 0"));
	if (!set_material(obj, tokens, 5))
		return (error_msg("Cylinder: `set_material()` error"));
	cy->center = vec3_sub(cy->center, vec3_mul(cy->axis, cy->height / 2.0));
	obj->type = CYLINDER;
	obj->shape_data = cy;
	obj->next = scene->objects;
	scene->objects = obj;
	return (1);
}

// Parses Cone: co [tip_xyz] [axis_xyz] [half_angle_deg] [height]
// set_material: [color] [specular] [shininess] [checker] [color2] [patter]
// we're considering the base cap middle as cylinder center
// if axis middle should be considered, then the line below should be added
// cy->center = vec3_add(cy->center, vec3_mul(cy->axis, -cy->height / 2.0));
int	parse_cone(char **tokens, t_scene *scene)
{
	t_object	*obj;
	t_cone		*co;

	if (count_tokens(tokens) < 5)
		return (error_msg("Cone: requires 5 parameters"));
	obj = malloc(sizeof(t_object));
	co = malloc(sizeof(t_cone));
	if (!obj || !co)
		return (error_msg("Cone: memory allocation failed"));
	if (!parse_vec3(tokens[1], &co->tip, 0))
		return (error_msg("Cone: invalid tip coordinates"));
	if (!parse_vec3(tokens[2], &co->axis, 1) || !validate_norm_vec3(co->axis))
		return (error_msg("Cone: invalid axis orientation vector"));
	if (!parse_double(tokens[3], &co->angle) || !validate_angle(co))
		return (error_msg("Cone: invalid angle"));
	if (!parse_double(tokens[4], &co->height) || co->height <= 0.0)
		return (error_msg("Cone: height should be > 0"));
	if (!set_material(obj, tokens, 5))
		return (error_msg("Cone: `set_material()` error"));
	obj->type = CONE;
	obj->shape_data = co;
	obj->next = scene->objects;
	scene->objects = obj;
	return (1);
}
