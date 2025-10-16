/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 23:27:51 by anemet            #+#    #+#             */
/*   Updated: 2025/10/16 10:57:17 by anemet           ###   ########.fr       */
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
	write(2, message, ft_strlen(message));
	write(2, "\n", 1);
	return (0);
}

/* obj_err()
	make valgrind happy by freeing up generic `obj` and `shape` (sphere,
	plane, cylinder, cone) memory allocations before calling error_msg
	Input:
		message:	error message to display
		obj:		pointer to the generic obj struct
		shape:		pointer to the specific obj data (sp, pl, cy, co)
	Return:
		0 (returned by error_msg())
*/
int	obj_err(char *message, t_object *obj, void *shape)
{
	free(obj);
	free(shape);
	return (error_msg(message));
}

/* light_err()
	make valgrind happy by freeing up generic light malloc before
	calling error_msg
	Input:
		message:	error message to display
		light:		pointer to the light struct
	Return:
		0 (returned by error_msg())
*/
int	light_err(char *message, t_light *light)
{
	free(light);
	return (error_msg(message));
}
