/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 09:33:40 by anemet            #+#    #+#             */
/*   Updated: 2025/10/02 09:42:13 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

/* cleanup()
	Frees all allocated resources in the reverse order of creation
	and exits the program cleanly
TODO: free_scene() in the parser module
*/
int	cleanup(t_program_data *data)
{
	if (!data)
		exit(0);
	if (data->scene)
		free_scene(data->scene);
	if (data->mlx)
	{
		if (data->mlx->img_ptr)
			mlx_destroy_image(data->mlx->mlx_ptr, data->mlx->img_ptr);
		if (data->mlx->win_ptr)
			mlx_destroy_window(data->mlx->mlx_ptr, data->mlx->win_ptr);
		if (data->mlx->mlx_ptr)
		{
			mlx_destory_display(data->mlx->mlx_ptr);
			free(data->mlx->mlx_ptr);
		}
		free(data->mlx);
	}
	free(data);
	exit(0);
	return (0);
}
