/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 16:22:58 by anemet            #+#    #+#             */
/*   Updated: 2025/09/30 16:51:23 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "minirt.h"

// --- Main parsing function ---
int		parse_scene(const char *filename, t_scene *scene);

// --- Element Parsers (Internal) ---
// not needed if called only from parser.c, but shown for clarity
int		parse_ambient(char **tokens, t_scene *scene);
int		parse_camera(char **tokens, t_scene *scene);
int		parse_light(char **tokens, t_scene *scene);
int		parse_sphere(char **tokens, t_scene *scene);
int		parse_plane(char **tokens, t_scene *scene);
int		parse_cylinder(char **tokens, t_scene *scene);

// --- Utility Parsers ---
int		parse_vector(const char *str, t_vec3 *vec);
int		parse_color(const char *str, t_color *color);
int		parse_double(const char *str, double *val);
// double	ft_atof(const char *str); // This should belong to libft

// --- Validation Utils ---
int		validate_normalized_vector(t_vec3 vec);
int		validate_ratio(double ratio);
int		validate_fov(double fov);

// --- Helper Functions ---
void	free_tokens(char **tokens);
int		count_tokens(char **tokens);
int		error_msg(char *message);

#endif
