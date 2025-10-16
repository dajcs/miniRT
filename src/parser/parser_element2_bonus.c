/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_element2_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 14:26:45 by anemet            #+#    #+#             */
/*   Updated: 2025/10/16 11:05:26 by anemet           ###   ########.fr       */
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
		return (obj_err("Plane: memory allocation failed", obj, pl));
	if (!parse_vec3(tokens[1], &pl->point, 0))
		return (obj_err("Plane: invalid point coordinates", obj, pl));
	if (!parse_vec3(tokens[2], &pl->normal, 1)
		|| !validate_norm_vec3(pl->normal))
		return (obj_err("Plane: invalid normal vector", obj, pl));
	if (!set_material(obj, tokens, 3))
		return (obj_err("Plane: `set_material()` error", obj, pl));
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
		return (obj_err("Cylinder: memory allocation failed", obj, cy));
	if (!parse_vec3(tokens[1], &cy->center, 0))
		return (obj_err("Cylinder: invalid center coordinates", obj, cy));
	if (!parse_vec3(tokens[2], &cy->axis, 1) || !validate_norm_vec3(cy->axis))
		return (obj_err("Cylinder: invalid axis orientation vector", obj, cy));
	if (!parse_double(tokens[3], &cy->diameter) || cy->diameter <= 0.0)
		return (obj_err("Cylinder: diameter should be > 0", obj, cy));
	if (!parse_double(tokens[4], &cy->height) || cy->height <= 0.0)
		return (obj_err("Cylinder: height should be > 0", obj, cy));
	if (!set_material(obj, tokens, 5))
		return (obj_err("Cylinder: `set_material()` error", obj, cy));
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
		return (obj_err("Cone: memory allocation failed", obj, co));
	if (!parse_vec3(tokens[1], &co->tip, 0))
		return (obj_err("Cone: invalid tip coordinates", obj, co));
	if (!parse_vec3(tokens[2], &co->axis, 1) || !validate_norm_vec3(co->axis))
		return (obj_err("Cone: invalid axis orientation vector", obj, co));
	if (!parse_double(tokens[3], &co->angle) || !validate_angle(co))
		return (obj_err("Cone: invalid angle", obj, co));
	if (!parse_double(tokens[4], &co->height) || co->height <= 0.0)
		return (obj_err("Cone: height should be > 0", obj, co));
	if (!set_material(obj, tokens, 5))
		return (obj_err("Cone: `set_material()` error", obj, co));
	obj->type = CONE;
	obj->shape_data = co;
	obj->next = scene->objects;
	scene->objects = obj;
	return (1);
}
