/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cone_intersect.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 12:20:16 by anemet            #+#    #+#             */
/*   Updated: 2025/10/13 14:11:44 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

/* The Ray-Cone Intersection Math
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A point P is on an infinite cone if the angle α between the vector from the tip
to P		(V = P - Tip) and the cone's Axis is constant.
This gives the implicit equation:

(V · Axis)² = (V · V) * cos²(α)

Substituting the ray equation P(t) = O + tD gives  V(t) = (O - Tip) + tD.

Let			oc = O - Tip. The equation becomes:

((oc + tD) · A)² = ((oc + tD) · (oc + tD)) * cos²(α)

This expands to the quadratic equation at² + bt + c = 0,
where the coefficients are:

a = (D · A)² - cos²(α)
b = 2 * [ (D · A)(oc · A) - (D · oc) * cos²(α) ]
c = (oc · A)² - (oc · oc) * cos²(α)

*/

/* is_hit_in_bounds()
	Checks if a given intersection point is withing the cone's height
	Input:
		m:	the projected distance of the hit point along the cone's axis
		height:	the cone's height
	Return 1 if the hit is valid, 0 otherwise
*/
static int	is_hit_in_bounds(double m, double height)
{
	return (m >= 0 && m <= height);
}

static void	set_cone_wall_hit(t_hit_info *info, double qt)
{
	info->hit_t = qt;
	info->part = HIT_WALL;
}

/* intersect_cone_wall()
	This function solves the quadratic equation and checks both roots
	Input:
		*co:	t_cone object
		*r:		the ray to test
		*info:	the hit_info struct, updated if a closer hit is found
	Return 1 if there is a valid hit with the cone wall, 0 otherwise
*/
static int	intersect_cone_wall(t_cone *co, t_ray *r, t_hit_info *info)
{
	t_quadratic	q;
	t_vec3		oc;
	double		d_dot_a;
	double		oc_dot_a;

	oc = vec3_sub(r->origin, co->tip);
	d_dot_a = vec3_dot(r->direction, co->axis);
	oc_dot_a = vec3_dot(oc, co->axis);
	q.a = d_dot_a * d_dot_a - co->cos_angle_sq;
	q.b = 2 * (d_dot_a * oc_dot_a - vec3_dot(r->direction, oc)
			* co->cos_angle_sq);
	q.c = oc_dot_a * oc_dot_a - vec3_dot(oc, oc) * co->cos_angle_sq;
	q.discriminant = q.b * q.b - 4 * q.a * q.c;
	if (q.discriminant < 0)
		return (0);
	q.t1 = (-q.b - sqrt(q.discriminant)) / (2 * q.a);
	q.t2 = (-q.b + sqrt(q.discriminant)) / (2 * q.a);
	if (q.t1 > 0.001 && q.t1 < info->hit_t && is_hit_in_bounds(oc_dot_a + q.t1
			* d_dot_a, co->height))
		set_cone_wall_hit(info, q.t1);
	if (q.t2 > 0.001 && q.t2 < info->hit_t && is_hit_in_bounds(oc_dot_a + q.t2
			* d_dot_a, co->height))
		set_cone_wall_hit(info, q.t2);
	return (info->part == HIT_WALL);
}

/* intersect_cone_cap()
	Intersects the ray with the cone's single base cap
	Input:
		*co:	the cone object
		*r:		the ray to test
		*info:	hit_info struct, updated if a closer hit is found.
	Return 1 if a new, closer, valid hit was found on the cap, 0 otherwise

	The function defines the plane on which the cone's circular base lies.
	It uses the existing hit_plane function to test for an intersection.
	If the plane is hit, it then calculates the squared radius of the cap and
	checks if the intersection point is within that radius.

	0. Initialize the hit_info struct (0, t_max, HIT_NONE)
	1. Define the plane for the cone's base cap. The point is at the base,
		and the normal points along the axis.
	2. Check for an intersection with the cap's plane. We only consider hits
		that are closer than any previously found hit.
	3. If plane hit, calculate the squared radius of the cap using a trig. eq:
		radius² = height² * tan²(angle) = height² * (1/cos²(angle) - 1)
	4. Check if the hit point is within the cap's circular boudary.
	5. If hit within radius, update the hit_info struct
*/
static int	intersect_cone_cap(t_cone *co, t_ray *r, t_hit_info *info,
		double t_max)
{
	t_plane			cap;
	t_hit_record	temp_rec;
	double			radius_sq;

	info->hit = 0;
	info->hit_t = t_max;
	info->part = HIT_NONE;
	cap.point = vec3_add(co->tip, vec3_mul(co->axis, co->height));
	cap.normal = co->axis;
	if (hit_plane(&cap, r, info->hit_t, &temp_rec))
	{
		radius_sq = co->height * co->height * (1.0 / co->cos_angle_sq - 1.0);
		if (vec3_length_squared(vec3_sub(temp_rec.p, cap.point)) <= radius_sq)
		{
			info->hit_t = temp_rec.t;
			info->t = temp_rec.t;
			info->p = temp_rec.p;
			info->normal = temp_rec.normal;
			info->hit = 1;
			info->part = HIT_CAP;
			return (1);
		}
	}
	return (0);
}

/* hit_cone()
	Orchestrates the ray & cone intersection process checking the walls
	and the cap, and if valid hit, then it calculates the normal
	Input:
		*co:	the cone object
		*ray:	the ray to test
		t_max:	max t value to have a valid hit
		*rec:	record of the ray-cone intersection
	Return 1 if there is a valid hit with the cone, 0 otherwise

	1. set `info` by the eventual intersection with the infinite cone's walls
	2. set `info` by the eventual intersection with the base cap
	3. If no valid hit => Return
	4. Populate the hit record t, p, n values
	5. Calculate the normal based on what was hit
		- if HIT_WALL, normal is subsequently computed with:
			p_tip = P - tip
			m = p_tip projection on axis
			using: 1 + tan^2(a) = 1 / cos^2(a)
		- if HIT_CAP, the normal is the cone axis
	6. Revert the normal direction to point towards the incoming ray
*/
int	hit_cone(t_cone *co, t_ray *ray, double t_max, t_hit_record *rec)
{
	t_hit_info	info;
	t_vec3		p_tip;
	double		m;
	double		k_inv_cos2;

	(void)intersect_cone_cap(co, ray, &info, t_max);
	(void)intersect_cone_wall(co, ray, &info);
	if (info.hit_t >= t_max || info.part == HIT_NONE)
		return (0);
	rec->t = info.hit_t;
	rec->p = vec3_add(ray->origin, vec3_mul(ray->direction, rec->t));
	if (info.part == HIT_WALL)
	{
		p_tip = vec3_sub(rec->p, co->tip);
		m = vec3_dot(p_tip, co->axis);
		k_inv_cos2 = 1.0 / co->cos_angle_sq;
		rec->normal = vec3_normalize(vec3_sub(p_tip, vec3_mul(co->axis, m
						* k_inv_cos2)));
	}
	else if (info.part == HIT_CAP)
		rec->normal = co->axis;
	if (vec3_dot(ray->direction, rec->normal) > 0.0)
		rec->normal = vec3_mul(rec->normal, -1);
	return (1);
}
