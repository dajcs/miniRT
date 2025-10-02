/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mock_render.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 13:15:20 by anemet            #+#    #+#             */
/*   Updated: 2025/10/02 13:22:08 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

/*
** This is a mock render function for testing purposes.
** It iterates through every pixel of the window and paints it a solid
** color (in this case, a shade of purple).
	// A distinct color like purple is good for testing.
	// Format is 0x00RRGGBB (or 0xAARRGGBB if alpha is used).
**
** This proves that:
** 1. The scene data (width, height) was parsed and is accessible.
** 2. The MLX window and image buffer are correctly initialized.
** 3. The my_put_pixel_to_img() function works as expected.
*/
void	render(t_scene *scene, t_mlx_data *mlx)
{
	int	x;
	int	y;
	int	color;

	color = 0x009933FF;
	y = 0;
	while (y < scene->height)
	{
		x = 0;
		while (x < scene->width)
		{
			my_put_pixel_to_img(mlx, x, y, color);
			x++;
		}
		y++;
	}
}
