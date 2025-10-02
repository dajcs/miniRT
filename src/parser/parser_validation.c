/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_validation.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 15:12:50 by anemet            #+#    #+#             */
/*   Updated: 2025/10/02 15:17:11 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

/* validate_ratio()
	Validates that the ratio (e.g., for light or ambient)
	is within the range [0.0, 1.0]
*/
int	validate_ratio(double ratio)
{
	return (ratio >= 0.0 && ratio <= 1.0);
}

/* validate_fov()
	Validates that the field of view (FOV) is within a reasonable range
	Typically between 1 and 179 degrees to avoid extreme distortion
*/
int	validate_fov(double fov)
{
	return (fov > 0 && fov < 180);
}

/* validate_normalized_vector()
	Validates that a vector is normalized (i.e., has a length of 1)
	within a small epsilon to account for floating-point precision
*/
int	validate_normalized_vector(t_vec3 vec)
{
	double	length;
	const double	epsilon = 1e-6;

	length = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	return (fabs(length - 1.0) < epsilon);
}
