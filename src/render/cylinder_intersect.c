/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder_intersect.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 17:19:51 by anemet            #+#    #+#             */
/*   Updated: 2025/10/05 21:15:57 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

// calculates the normal at the hit point P on the cylinder wall
void	get_wall_normal(t_cylinder *cy, t_hit_info *info)
{
	t_vec3	oc;
	double	m;

	oc = vec3_sub(info->p, cy->center);
	m = vec3_dot(oc, cy->axis);
	info->normal = vec3_normalize(vec3_sub(oc, vec3_mul(cy->axis, m)));
}

/* check_wall_hit()
	Checks a single potential intersection `t` with the cylinder wall
	Input:
		*cy:	t_cylinder
		*ray:	t_ray
		*info:	t_hit_info
	Return 1 if the hit is valid and closer than t_max, otherwise 0

	The function verifies if a hit point on the infinite cylinder's wall falls
	within the finite height of our cylinder object. It calculates the
	projection of the hit point onto the cylinder's axis to see if it's between
	the base(0) and the top (cy->height)
*/
int	check_wall_hit(t_cylinder *cy, t_ray *ray, t_hit_info *info)
{
	t_vec3	oc;
	double	m;

	info->p = vec3_add(ray->origin, vec3_mul(ray->direction, info->t));
	oc = vec3_sub(info->p, cy->center);
	m = vec3_dot(oc, cy->axis);
	if (m >= 0 && m <= cy->height)
	{
		get_wall_normal(cy, info);
		info->hit = 1;
		return (1);
	}
	return (0);
}

/* intersect_cylinder_wall()
	Solves the quadratic equation for ray -> cylinder wall intersection
	Input:
		*cy:	t_cylinder
		*r:		t_ray
		*info:	t_hit_info
	Return 1 if cylinder wall is hit, 0 otherwise

	The function calculates the coefficients a, b, and c for the quadratic
	equation that describes the ray-cylinder intersection. It then solves for
	t1 and t2. It checks both solutions to find the smallest positive `t` that
	is within the cylinder's height bounds;
*/
/* Mathematical Concept:

A point P is on an infinite cylinder's wall if its perpendicular distance to
the cylinder's central axis is equal to the radius r.

=> The squared length of the vector from P to the axis, perpendicular to the
axis is r².

Let:
	r: Ray to be tested ((O, D))
	cy: Cylinder (center C, axis A, radius r_cy)
	oc: Vector from ray origin to cylinder center (O - C)

	a = (D · D) - (D · A)²   // D · D == 1, if (D · A)² == 1 => D || with A
code:
	q.a = vec3_dot(r->dir, r->dir) - pow(vec3_dot(r->dir, cy->axis), 2);

	b = 2 * [ (D · oc) - (D · A) * (oc · A) ]
code:
	q.b = 2 * (vec3_dot(r->dir, oc) - (vec3_dot(r->dir, cy->axis) *
													vec3_dot(oc, cy->axis)));

	c = (oc · oc) - (oc · A)² - r_cy²
code:
	qc = vec3_dot(oc, oc) - pow(vec3_dot(oc, cy->axis), 2) -
								(cy->diameter / 2.0) * (cy->diameter / 2.0);
*/
int	intersect_cylinder_wall(t_cylinder *cy, t_ray *r, t_hit_info *info)
{
	t_quadratic	q;
	t_vec3		oc;

	oc = vec3_sub(r->origin, cy->center);
	q.a = vec3_dot(r->direction, r->direction) - pow(vec3_dot(r->direction,
				cy->axis), 2);
	q.b = 2 * (vec3_dot(r->direction, oc) - vec3_dot(r->direction, cy->axis)
			* vec3_dot(oc, cy->axis));
	q.c = vec3_dot(oc, oc) - pow(vec3_dot(oc, cy->axis), 2) - (cy->diameter
			/ 2.0) * (cy->diameter / 2.0);
	q.discriminant = q.b * q.b - 4 * q.a * q.c;
	if (q.discriminant < 0)
		return (0);
	q.t1 = (-q.b - sqrt(q.discriminant)) / (2 * q.a);
	q.t2 = (-q.b + sqrt(q.discriminant)) / (2 * q.a);
	info->t = q.t1;
	if (info->t > 0.001 && info->t < info->hit_t && check_wall_hit(cy, r, info))
		return (1);
	info->t = q.t2;
	if (info->t > 0.001 && info->t < info->hit_t && check_wall_hit(cy, r, info))
		return (1);
	return (0);
}

/* intersect_cylinder_caps()
	Intersects the ray with the cylinder's top and bottom caps
	Input:
		*cy:	t_cylinder
		*r:		t_ray
		*info:	t_hit_info
	Return 1 if a valid, closer hit on a cap is found, 0 otherwise

	The cylinder caps are circles that lie on planes. This function performs two
	ray-plane intersections. If a plane is hit, it then checks if the hit point
	is withing the circle's radius. It pudates the hit_info struct if a closer
	intersection is found on either cap.
*/
int	intersect_cylinder_caps(t_cylinder *cy, t_ray *r, t_hit_info *info)
{
	t_plane	cap;
	double	dist_sq;

	cap.point = cy->center;
	cap.normal = vec3_mul(cy->axis, -1);
	if (hit_plane(&cap, r, info->hit_t, (void *)info) && info->t < info->hit_t)
	{
		dist_sq = vec3_length_squared(vec3_sub(info->p, cy->center));
		if (dist_sq <= pow(cy->diameter / 2.0, 2))
			info->hit = 1;
	}
	cap.point = vec3_add(cy->center, vec3_mul(cy->axis, cy->height));
	cap.normal = cy->axis;
	if (hit_plane(&cap, r, info->hit_t, (void *)info) && info->t < info->hit_t)
	{
		dist_sq = vec3_length_squared(vec3_sub(info->p, cap.point));
		if (dist_sq <= pow(cy->diameter / 2.0, 2))
			info->hit = 1;
	}
	return (info->hit);
}
