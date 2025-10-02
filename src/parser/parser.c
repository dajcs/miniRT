/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 14:53:51 by anemet            #+#    #+#             */
/*   Updated: 2025/10/02 15:08:30 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

// Dispatcher to call the correct parser based on the element identifier
static int	parse_line(char *line, t_scene *scene)
{
	char	**tokens;
	int		result;

	tokens = ft_splits(line, " \t\r\v\f");
	if (!tokens || !tokens[0])
	{
		free_tokens(tokens);
		return (1);
	}
	if (ft_strcmp(tokens[0], "A") == 0)
		result = parse_ambient(tokens, scene);
	else if (ft_strcmp(tokens[0], "C") == 0)
		result = parse_camera(tokens, scene);
	else if (ft_strcmp(tokens[0], "L") == 0)
		result = parse_light(tokens, scene);
	else if (ft_strcmp(tokens[0], "sp") == 0)
		result = parse_sphere(tokens, scene);
	else if (ft_strcmp(tokens[0], "pl") == 0)
		result = parse_plane(tokens, scene);
	else if (ft_strcmp(tokens[0], "cy") == 0)
		result = parse_cylinder(tokens, scene);
	else
		result = error_msg("Unknown element identifier");
	free_tokens(tokens);
	return (result);
}

// Initialize the scene struct with default values
static void	init_scene(t_scene *scene)
{
	scene->width = 1280;
	scene->height = 720;
	scene->ambient_light = (t_color){0, 0, 0};
	scene->ambient_ratio = 0.0;
	scene->has_ambient = 0;
	scene->camera = (t_camera){0};
	scene->has_camera = 0;
	scene->lights = NULL;
	scene->objects = NULL;
}

// Reads the file line by line and calls parser for each line
// ignore empty lines and # comments
// Returns 1 on success, 0 on parsing error
static int	read_and_parse_file(int fd, t_scene *scene)
{
	char	*line;
	int		status;

	status = 1;
	line = get_next_line(fd);
	while (line != NULL)
	{
		if (*line != '\n' && *line != '#')
		{
			if (!parse_line(line, scene))
				status = 0;
		}
		free(line);
		if (status == 0)
			return (0);
		line = get_next_line(fd);
	}
	return (1);
}

// Main entry point for parsing a scene file
// On read_and_parse_file error: free allocated scene and its contents
t_scene	*parse_scene(const char *filename)
{
	int		fd;
	t_scene	*scene;

	if (ft_strlen(filename) < 4 || ft_strcmp(filename + ft_strlen(filename) - 3,
			".rt") != 0)
		return (error_msg("Filename must end with .rt"), NULL);
	fd = open(filename, O_RDONLY);
	if (fd < 0)
		return (error_msg("Failed to open file"), NULL);
	scene = malloc(sizeof(t_scene));
	if (!scene)
		return (error_msg("Scene memory allocation failed"), NULL);
	init_scene(scene);
	if (!read_and_parse_file(fd, scene))
		return (free_scene(scene), close(fd), NULL);
	close(fd);
	if (!scene->has_camera || !scene->has_ambient)
	{
		free_scene(scene);
		return (error_msg("Scene must have Camera and Ambient light"), NULL);
	}
	return (scene);
}

// frees all allocated memory in the scene struct including linked lists
void	free_scene(t_scene *scene)
{
	t_light		*light;
	t_light		*next_light;
	t_object	*object;
	t_object	*next_object;

	if (!scene)
		return ;
	light = scene->lights;
	while (light)
	{
		next_light = light->next;
		free(light);
		light = next_light;
	}
	object = scene->objects;
	while (object)
	{
		next_object = object->next;
		free(object->shape_data);
		free(object);
		object = next_object;
	}
	free(scene);
}
