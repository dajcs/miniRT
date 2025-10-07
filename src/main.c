/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 10:35:00 by anemet            #+#    #+#             */
/*   Updated: 2025/10/07 14:10:51 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

int	main(int argc, char **argv)
{
	t_program_data	*data;

	if (argc != 2)
		return (error_msg("Usage: ./miniRT <scene.rt>"), 1);
	data = init_program_data(argv[1]);
	if (!data)
		return (1);
	render(data->scene, data->mlx);
	mlx_put_image_to_window(data->mlx->mlx_ptr, data->mlx->win_ptr,
		data->mlx->img_ptr, 0, 0);
	setup_hooks(data);
	mlx_loop(data->mlx->mlx_ptr);
	return (0);
}
