/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 23:35:03 by anemet            #+#    #+#             */
/*   Updated: 2025/10/15 13:04:19 by anemet           ###   ########.fr       */
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
	double	cos_angle;

	if (co->angle <= 0 || co->angle >= 90)
		return (error_msg("Half angle must be in (0, 90)"));
	cos_angle = cos(co->angle * M_PI / 180.0);
	co->cos_angle_sq = cos_angle * cos_angle;
	return (1);
}

char	**get_tokens(char *line)
{
	char	**tokens;

	tokens = ft_splits(line, " \t\r\v\f");
	if (!tokens || !tokens[0])
	{
		free_tokens(tokens);
		return (NULL);
	}
	return (tokens);
}

/*
	Initialize safe defaults for optional material properties
	obj->speci = 0.0;                     // no specular by default
	obj->shine = 32.0;                    // reasonable Phong exponent
	obj->checker = 0;                     // no checkerboard / reflection
	obj->color2 = (t_color){1.0, 1.0, 1.0}; // secondary color (white)
	obj->pattern_scale = 1.0;             // 1x1 squares default
	obj->reflect = 0.0;                   // non-reflective by default
*/
void	set_default_material(t_object *obj)
{
	obj->speci = 0.0;
	obj->shine = 32.0;
	obj->checker = 0;
	obj->color2 = (t_color){1.0, 1.0, 1.0};
	obj->pattern_scale = 1.0;
	obj->reflect = 0.0;
}
