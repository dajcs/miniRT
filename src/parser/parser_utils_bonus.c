/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 23:35:03 by anemet            #+#    #+#             */
/*   Updated: 2025/10/10 15:18:12 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

// Parses a string into an int
// A more robust implementation could check for invalid characters
// only unsed to parse the `checker` flag
// if that one is missing then we don't need to parse subsequent parameters
int	parse_int(char *str, int *val)
{
	if (!str)
		return (0);
	*val = ft_atoi(str);
	return (1);
}

int	validate_angle(t_cone *co)
{
	double cos_angle;

	if (co->angle <= 0 || co->angle >= 90)
		return (error_msg("Half angle must be in (0, 90)"));
	cos_angle = cos(co->angle * M_PI / 180.0);
	co->cos_angle_sq = cos_angle * cos_angle;
	return 1;
}
