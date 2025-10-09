/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec3_ops3_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 15:07:09 by anemet            #+#    #+#             */
/*   Updated: 2025/10/09 15:20:35 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

/* vec3_reflect()
	Calculates the reflection of a vector 'in' around a normal 'n'
	Input:
		in:	incoming t_vec3 vector (e.g., from the light to the surface)
		n:	the surface normal (t_vec3)
	Return: the reflected vector (t_vec3)

	This function implements the reflection formula: R = D - 2 * (D·N) * N,
	where D is the incident vector and N is the normal to the surface
*/
t_vec3	vec3_reflect(t_vec3 in, t_vec3 n)
{
	double	dot_product;
	t_vec3	scaled_normal;

	dot_product = 2 * vec3_dot(in, n);
	scaled_normal = vec3_mul(n, dot_product);
	return (vec3_sub(in, scaled_normal));
}
