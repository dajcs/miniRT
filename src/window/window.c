/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 09:53:54 by anemet            #+#    #+#             */
/*   Updated: 2025/10/02 10:24:24 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

/* my_put_pixel_to_img()
	Calculates the memory offset for a pixel at (x, y) and sets its color.
	This is the bridge between the renderer's color calculations and the
	actual image buffer.
*/
void	my_put_pixel_to_img(t_mlx_data *data, int x, int y, int color)
{
	char	*dst;

	if (!data || !data->addr)
		return ;
	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

/* init_mlx_data()
	Initializes the MLX connection, creates a window,
		and sets up an image buffer
	Returns a pointer to the t_mlx_data struct on success, or NULL on failure.
*/
static t_mlx_data	*init_mlx_data(t_scene *scene)
{
	t_mlx_data	*mlx;

	mlx = malloc(sizeof(t_mlx_data));
	if (!mlx)
		return (NULL);
	mlx->mlx_ptr = mlx_init();
	if (!mlx->mlx_ptr)
		return (free(mlx), NULL);
	mlx->win_ptr = mlx_new_window(mlx->mlx_ptr, scene->width, scene->height,
			"miniRT");
	if (!mlx->win_ptr)
		return (free(mlx->mlx_ptr), free(mlx), NULL);
	mlx->img_ptr = mlx_new_image(mlx->mlx_ptr, scene->width, scene->height);
	if (!mlx->img_ptr)
		return (mlx_destroy_window(mlx->mlx_ptr, mlx->win_ptr),
			free(mlx->mlx_ptr), free(mlx), NULL);
	mlx->addr = mlx_get_data_addr(mlx->img_ptr, &mlx->bits_per_pixel,
			&mlx->line_length, &mlx->endian);
	return (mlx);
}

/* init_program_data()
	The main initialization function.
	It orchestrates parsing the scene file and setting up the MLX window
	and data structures.
*/
t_program_data	*init_program_data(char *scene_file)
{
	t_program_data	*data;
	t_scene			*scene;
	t_mlx_data		*mlx;

	scene = parse_scene(scene_file);
	if (!scene)
		return (NULL);
	mlx = init_mlx_data(scene);
	if (!mlx)
	{
		free_scene(scene);
		return (NULL);
	}
	data = malloc(sizeof(t_program_data));
	if (!data)
	{
		free(mlx);
		return (NULL);
	}
	data->scene = scene;
	data->mlx = mlx;
	return (data);
}
