/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_element2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 14:26:45 by anemet            #+#    #+#             */
/*   Updated: 2025/10/02 15:31:11 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

// TODO: Implementation of parse_plane and parse_cylinder like parse_sphere()

// Parses Plane: pl <x,y,z> <nx,ny,nz> <R,G,B>
int	parse_plane(char **tokens, t_scene *scene)
{
	(void)tokens;
	(void)scene;
	return (0);
}

// Parses Cylinder: cy <x,y,z> <nx,ny,nz> <diameter> <height> <R,G,B>
int	parse_cylinder(char **tokens, t_scene *scene)
{
	(void)tokens;
	(void)scene;
	return (0);
}
