/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hooks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 09:43:20 by anemet            #+#    #+#             */
/*   Updated: 2025/10/02 09:53:12 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

// 65307 is a common ESC on Linux with X11
#define ESC_KEY 65307

/* key_hook()
	This function is called when a key is pressed.
	It checks if the pressed key is ESC. If so, it starts the cleanup process.
*/
int	key_hook(int keycode, t_program_data *data)
{
	if (keycode == ESC_KEY)
		cleanup(data);
	return (0);
}

/* close_hook()
	This function is called when the window's red cross is clicked
	It triggers the cleanup process
*/
int	close_hook(t_program_data *data)
{
	cleanup(data);
	return (0);
}

/* setup_hooks()
	Registers the event handlers (hooks) with MiniLibX.
	- KeyPress (key_hook) is for keyboard input
	- DestroyNotify (close_hook) is for the window close click
*/
void	setup_hooks(t_program_data *data)
{
	mlx_hook(data->mlx->win_ptr, 17, 0, close_hook, data);
	mlx_key_hook(data->mlx->win_ptr, key_hook, data);
}
