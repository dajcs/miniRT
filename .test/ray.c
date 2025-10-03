/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 20:19:12 by anemet            #+#    #+#             */
/*   Updated: 2025/10/02 20:53:15 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

/* create_ray(t_point3 origin, t_vec3 direction)
	Creates a ray given an origin point and a direction vector.
*/
t_ray	create_ray(t_point3 origin, t_vec3 direction)
{
	t_ray	r;

	r.origin = origin;
	r.direction = direction;
	return (r);
}

/* ray_at(t_ray *ray, double t)
	Computes the point along the ray at distance t.
	P(t) = origin + t * direction
*/
t_point3	ray_at(t_ray r, double t)
{
	t_vec3	scaled_direction;

	scaled_direction = vec3_mul(r.direction, t);
	return (vec3_add(r.origin, scaled_direction));
}
