/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 21:42:32 by anemet            #+#    #+#             */
/*   Updated: 2025/10/01 22:16:47 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WINDOW_H
# define WINDOW_H

// --- Function Prototypes ---

// Main initialization function ofr creating the program's context
t_program_data	*init_program_data(char *scene_file);

// Sets up the event hooks for keyboard and window closing
void	setup_hooks(t_program_data *data);

// Puts a pixel of a given color into the image buffer at (x, y)
void	my_put_pixel_to_img(t_mlx_data *data, int x, int y, int color);

// Handles cleanup and clean exit of the program
int	cleanup(t_program_data *data);

#endif
