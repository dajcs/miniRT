/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec3_ops2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 19:51:42 by anemet            #+#    #+#             */
/*   Updated: 2025/10/04 19:28:28 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

/* vec3_length_squared(t_vec3 v)
	Returns the squared length of vector v.
	Useful for performance optimization when comparing lengths.
*/
double	vec3_length_squared(t_vec3 v)
{
	double	result;

	result = v.x * v.x + v.y * v.y + v.z * v.z;
	return (result);
}

/* vec3_length(t_vec3 v)
	Returns the length (magnitude) of vector v.
*/
double	vec3_length(t_vec3 v)
{
	double	len_sq;

	len_sq = vec3_length_squared(v);
	return (sqrt(len_sq));
}

/* vec3_normalize(t_vec3 v)
	Returns the unit vector (normalized vector) of v.
	Handles zero-length vectors by returning the vector itself.
*/
t_vec3	vec3_normalize(t_vec3 v)
{
	double	len;

	len = vec3_length(v);
	if (len == 0.0)
		return (v);
	return (vec3_div(v, len));
}

/* vec3_cross(t_vec3 v1, t_vec3 v2)
	Returns the cross product of vectors v1 and v2.
	The result is a vector perpendicular to both v1 and v2.
*/
t_vec3	vec3_cross(t_vec3 v1, t_vec3 v2)
{
	t_vec3	result;

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return (result);
}

/* vec3_mulxyz()
	Component-wise multiplication of two vectors.
	The result is a t_vec3.
	v_new = v1 x v2
*/
t_vec3	vec3_mulxyz(t_vec3 v1, t_vec3 v2)
{
	t_vec3	result;

	result.x = v1.x * v2.x;
	result.y = v1.y * v2.y;
	result.z = v1.z * v2.z;
	return (result);
}
