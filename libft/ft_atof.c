/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 14:31:05 by anemet            #+#    #+#             */
/*   Updated: 2025/10/03 06:47:08 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static double	parse_fractional(const char **nptr)
{
	double	frac;
	int		frac_div;

	frac = 0.0;
	frac_div = 1;
	if (**nptr == '.')
		(*nptr)++;
	while (**nptr >= '0' && **nptr <= '9')
	{
		frac = frac * 10.0 + (**nptr - '0');
		(*nptr)++;
		frac_div *= 10;
	}
	return (frac / frac_div);
}

double	ft_atof(const char *nptr)
{
	double	result;
	double	sign;

	result = 0.0;
	sign = 1.0;
	while (*nptr == ' ' || (*nptr >= 9 && *nptr <= 13))
		nptr++;
	if (*nptr == '-' || *nptr == '+')
	{
		if (*nptr == '-')
			sign = -1.0;
		nptr++;
	}
	while (*nptr >= '0' && *nptr <= '9')
		result = result * 10.0 + (*nptr++ - '0');
	return (sign * (result + parse_fractional(&nptr)));
}
