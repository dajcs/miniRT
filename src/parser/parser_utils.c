/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 23:35:03 by anemet            #+#    #+#             */
/*   Updated: 2025/10/01 00:04:51 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

// Counts the number of strings in a null-terminated array of strings
int	count_tokens(char **tokens)
{
	int	i;

	i = 0;
	while(tokens && tokens[i])
		i++;
	return (i);
}

// Frees a null-terminated array of strings
void	free_tokens(char **tokens)
{
	int	i;

	if (!tokens)
		return ;
	i = 0;
	while (tokens[i])
	{
		free(tokens[i]);
		i++;
	}
	free(tokens);
}

// Parses "R,G,B" into a t_color with values in 0-1.0 range
int	parse_color(char *str, t_color *color)
{
	char	**parts;
	int		r;
	int		g;
	int		b;

	parts = ft_split(str, ',');
	if (count_tokens(parts) != 3)
	{
		free_tokens(parts);
		return (0);
	}
	r = ft_atoi(parts[0]);
	g = ft_atoi(parts[1]);
	b = ft_atoi(parts[2]);
	free_tokens(parts);
	if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
		return (0);
	color->x = r / 255.0;
	color->y = g / 255.0;
	color->z = b / 255.0;
	return (1);
}

// Parses "x,y,z" into a t_vec3 struct
int	parse_vec3(char *str, t_vec3 *vec)
{
	char	**parts;

	parts = ft_split(str, ',');
	if (count_tokens(parts) != 3)
	{
		free_tokens(parts);
		return (0);
	}
	vec->x = atof(parts[0]);
	vec->y = atof(parts[1]);
	vec->z = atof(parts[2]);
	free_tokens(parts);
	return (1);
}

// Parses a string into a double
// A more robust implementation could check for invalid characters
int	parse_double(char *str, double *val)
{
	*val = atof(str);
	return (1);
}
