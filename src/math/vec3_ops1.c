/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec3_ops1.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 19:30:27 by anemet            #+#    #+#             */
/*   Updated: 2025/10/04 19:27:52 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

/* vec3_add()
	Adds two vectors component-wise.
	v = v1 + v2
*/
t_vec3	vec3_add(t_vec3 v1, t_vec3 v2)
{
	t_vec3	result;

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return (result);
}

/* vec3_sub()
	Subtracts two vectors component-wise.
	v = v1 - v2
*/
t_vec3	vec3_sub(t_vec3 v1, t_vec3 v2)
{
	t_vec3	result;

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return (result);
}

/* vec3_mul()
	Multiplies a vector by a scalar value t
	v_new = v * t
*/
t_vec3	vec3_mul(t_vec3 v, double t)
{
	t_vec3	result;

	result.x = v.x * t;
	result.y = v.y * t;
	result.z = v.z * t;
	return (result);
}

/* vec3_div()
	Divides a vector by a scalar value t
	Handles division by zero by returning a zero vector.
	v_new = v / t
*/
t_vec3	vec3_div(t_vec3 v, double t)
{
	t_vec3	result;

	if (t == 0.0)
		return ((t_vec3){0, 0, 0});
	result.x = v.x / t;
	result.y = v.y / t;
	result.z = v.z / t;
	return (result);
}

/* vec3_dot()
	Computes the dot product of two vectors.
	The result is a scalar value (double).
	dot = v1 . v2
*/
double	vec3_dot(t_vec3 v1, t_vec3 v2)
{
	double	result;

	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return (result);
}
