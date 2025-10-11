/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersections_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 18:40:52 by anemet            #+#    #+#             */
/*   Updated: 2025/10/11 07:21:30 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

// calculate color of the checker pattern
// the floor() function replaced with `round()` because of "dust" artifacts
static t_color	get_pattern_color(t_hit_record *rec, t_object *obj)
{
	double	scaled_x;
	double	scaled_y;
	double	scaled_z;
	int		sum;

	scaled_x = round(rec->p.x * obj->pattern_scale);
	scaled_y = round(rec->p.y * obj->pattern_scale);
	scaled_z = round(rec->p.z * obj->pattern_scale);
	sum = (int)(scaled_x + scaled_y + scaled_z);
	if (sum % 2 == 0)
		return (obj->color);
	else
		return (obj->color2);
}

/* hit_sphere()
	Tests for a ray-sphere intersection
	Input:
		*sp:	the sphere object
		*ray:	the ray to test
		t_max	the max distance to consider for an intersection
		*rec	the hit record to populate if an intersection is found
	Return 1 if valid intersection is found, 0 otherwise

	This function solves the quadratic equations `at^2 + bt + c = 0`
	which arises from substituting the ray equation into the sphere equation
	- ray equation P(t) = O + Dt  // O: ray origin, D: ray direction
	- sphere equation (P - C) * (P - C) = r^2. // P is on sphere with center C
																	and radius r
	(O + Dt - C) * (O + Dt - C) = r^2
		a = D^2 = 1.0 // D is normalized
		b = 2 * D * (O - C)
		c = (O - C) * (O - C) - r^2
	The discriminant of this equation (b^2 - 4*a*c)
	determines if there are 0, 1, or 2 intersections. We find the smallest
	positive root `t` that is within our accepted range [0.001, t_max].

	- the normal of the sphere is pointing outwards from its center
	- a ray hitting the sphere from outside will have an angle > 90 to the normal
		so the dot product will be negative
	- when the ray hits the sphere from inside, the ray has an angle < 90 to the
		normal, so the dot product will become positive => we're reversing the
		normal (flipping the point inwards). This guarantees the normal always
		opposes the ray's direction, which is the standard rendering convention
*/
int	hit_sphere(t_sphere *sp, t_ray *ray, double t_max, t_hit_record *rec)
{
	t_vec3	oc;
	double	a;
	double	b;
	double	c;
	double	discriminant;

	oc = vec3_sub(ray->origin, sp->center);
	a = vec3_length_squared(ray->direction);
	b = 2 * vec3_dot(ray->direction, oc);
	c = vec3_length_squared(oc) - sp->radius * sp->radius;
	discriminant = b * b - 4 * a * c;
	if (discriminant < 0)
		return (0);
	rec->t = (-b - sqrt(discriminant)) / (2.0 * a);
	if (rec->t <= 0.001 || rec->t >= t_max)
		rec->t = (-b + sqrt(discriminant)) / (2.0 * a);
	if (rec->t <= 0.001 || rec->t >= t_max)
		return (0);
	rec->p = vec3_add(ray->origin, vec3_mul(ray->direction, rec->t));
	rec->normal = vec3_normalize(vec3_sub(rec->p, sp->center));
	if (vec3_dot(ray->direction, rec->normal) > 0.0)
		rec->normal = vec3_mul(rec->normal, -1);
	return (1);
}

/* hit_plane()
	Tests for a ray-plane intersection.
	Input:
		*pl:	the plane object, containing a point P₀ and a normal vector n

	Equation: A point P is on a plane defined by a point P₀ on the plane
	and a normal vector n if the vector from P₀ to P is perpendicular to n.
	Mathematically: (P - P₀) · n = 0.

	How to Solve:
	1. Substitute the ray equation P(t): (O + Dt - P₀) · n = 0.
	2. This is a simple linear equation. Solve for t:
		t = ((P₀ - O) · n) / (D · n)
	3. Edge Case: If the denominator (D · n) is zero, the ray is parallel
		to the plane and will never intersect (or is inside the plane,
		which can be ignored).
	4. If a t > 0 exists, that's the intersection distance.
*/
/*
	Formula: t = ((P₀ - O) · n) / (D · n)
	Where:
		P₀: is a point on the plane (pl->point)
		O : is the ray's origin (ray->origin)
		n : is the plane's normal vector (pl->normal)
		D : is the ray's direction (ray->direction)
	1. Check if denominator (D · n) is close to zero (ray is parallel).
		(the denominator is the cosine of the angle between the ray and n)
		if denominator ~ 0 => no hit, return 0
	2. Calculate the numerator = ((P₀ - O) · n)
	3. Calculate t = numerator / denominator.
	4. If t is outside the valid range [0.001, t_max], return 0
		t > 0.001: The hit must be in front of the ray's origin. The small
						epsilon prevents "shadow acne" or self-intersection
		t < t_max: The hit must be closer than any other object already found
	5. A valid intersection was found, we can populate the hit record
	6. Ensure the normal points against the incident ray.
		If the ray hits the "back" of the plane, flip the hit record normal
*/
int	hit_plane(t_plane *pl, t_ray *ray, double t_max, t_hit_record *rec)
{
	double	denominator;
	double	numerator;
	double	t;

	denominator = vec3_dot(ray->direction, pl->normal);
	if (fabs(denominator) < 1e-6)
		return (0);
	numerator = vec3_dot(vec3_sub(pl->point, ray->origin), pl->normal);
	t = numerator / denominator;
	if (t <= 0.001 || t >= t_max)
		return (0);
	rec->t = t;
	rec->p = vec3_add(ray->origin, vec3_mul(ray->direction, t));
	rec->normal = pl->normal;
	if (vec3_dot(ray->direction, rec->normal) > 0.0)
		rec->normal = vec3_mul(rec->normal, -1);
	return (1);
}

/* hit_cylinder()
	The main entry point for ray-cylinder intersection test
	Input:
		*cy:	t_cylinder, the cylinder object
		*ray:	t_ray, the ray to test
		t_max:	the maximum distance for a valid intersection
		*rec:	t_hit_record, the hit_record to populate on closest valid hit
	Return 1 if the cylinder was hit, 0 otherwise

	The function orchestrates the cylinder intersection process.
	It keeps track of potential new hits with cylinder in info (t_hit_info)
	Starts by setting info.hit = 0 (no valid hit with cylinder yet) and
	info.hit_t = t_max // using hit_t to track the closest hit so far
	It first checks for hits on the infinite wall, then on the end caps.
	It keeps track of the closest valid intersection found and, if any hit
	occured, it populates the final hit_record with the t_hit_info values.
*/
int	hit_cylinder(t_cylinder *cy, t_ray *ray, double t_max, t_hit_record *rec)
{
	t_hit_info	info;

	info.hit = 0;
	info.hit_t = t_max;
	intersect_cylinder_wall(cy, ray, &info);
	intersect_cylinder_caps(cy, ray, &info);
	if (info.hit)
	{
		rec->t = info.t;
		rec->p = info.p;
		rec->normal = info.normal;
		if (vec3_dot(ray->direction, rec->normal) > 0.0)
			rec->normal = vec3_mul(rec->normal, -1);
		return (1);
	}
	return (0);
}

/* hit_object()
	A dispatcher function that calls the correct intersection routine
	Input:
		*obj:	a pointer to the generic object to test
		*ray:	the ray to test against the object
		t_max:	the maximum valid distance for an intersection
		*rec:	the hit_record to populate on a successful hit
	Return 1 if the object was hit, 0 otherwise

	This function acts as a router.
	Checks the `type` of the object and calls the appropriate `hit_...` function
*/
int	hit_object(t_object *obj, t_ray *ray, double t_max, t_hit_record *rec)
{
	int	hit;

	hit = 0;
	if (obj->type == SPHERE)
		hit = hit_sphere(obj->shape_data, ray, t_max, rec);
	else if (obj->type == PLANE)
		hit = hit_plane(obj->shape_data, ray, t_max, rec);
	else if (obj->type == CYLINDER)
		hit = hit_cylinder(obj->shape_data, ray, t_max, rec);
	if (hit)
	{
		rec->color = obj->color;
		rec->speci = obj->speci;
		rec->shine = obj->shine;
		if (obj->checker)
			rec->color = get_pattern_color(rec, obj);
	}
	return (hit);
}
