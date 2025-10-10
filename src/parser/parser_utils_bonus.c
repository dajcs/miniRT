/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 23:35:03 by anemet            #+#    #+#             */
/*   Updated: 2025/10/10 12:59:10 by anemet           ###   ########.fr       */
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
