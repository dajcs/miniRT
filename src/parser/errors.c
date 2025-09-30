/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 23:27:51 by anemet            #+#    #+#             */
/*   Updated: 2025/09/30 23:49:02 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

/*
	Writes a standard error prefix, followed by a specific message.
	Always returns 0 to allow for easy error chaining, e.g.,
	return (error_msg("Invalid arguments"));
*/
int	error_msg(char *message)
{
	write(2, "Error\n", 6);
	write(2, message, strlen(message));
	write(2, "\n", 1);
	return (0);
}
