/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 14:31:05 by anemet            #+#    #+#             */
/*   Updated: 2025/10/02 14:32:12 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

double	ft_atof(const char *nptr)
{
	double	result;
	double	sign;
	double	frac;
	int		frac_div;

	result = 0.0;
	sign = 1.0;
	frac = 0.0;
	frac_div = 1;
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
	if (*nptr == '.')
		nptr++;
	while (*nptr >= '0' && *nptr <= '9')
	{
		frac = frac * 10.0 + (*nptr++ - '0');
		frac_div *= 10;
	}
	return (sign * (result + frac / frac_div));
}
