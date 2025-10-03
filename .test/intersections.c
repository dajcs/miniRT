/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersections.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 20:41:25 by anemet            #+#    #+#             */
/*   Updated: 2025/10/03 10:48:17 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

/* fill_hit_record()
	Fills the hit record with intersection data.
*/
static void	fill_hit_record(t_hit_record *rec, t_ray *r, double t, t_object *obj)
{
	rec->t = t;
	rec->p = ray_at(*r, t);
	rec->color = obj->color;
	if (obj->type == SPHERE)
		rec->normal = vec3_normalize(vec3_sub(rec->p, ((t_sphere *)obj->shape_data)->center));
	else if (obj->type == PLANE)
		rec->normal = ((t_plane *)obj->shape_data)->normal;

}
