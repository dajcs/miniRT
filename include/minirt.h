/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minirt.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 14:36:49 by anemet            #+#    #+#             */
/*   Updated: 2025/10/01 22:17:33 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINIRT_H
# define MINIRT_H

# define _GNU_SOURCE
# include <math.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <fcntl.h>

# include "libft.h"
# include "mlx.h"
# include "parser.h"

// Using t_vec3 for points, vectors, and colors for simplicity
typedef struct s_vec3
{
	double			x;
	double			y;
	double			z;
}					t_vec3;

typedef t_vec3	t_point3;	// alias for 3D point
typedef t_vec3	t_color;	// alias for RGB color (0-1.0 range)

// A ray is defined by an origin and a direction vector
typedef struct s_ray
{
	t_point3		origin;
	t_vec3			direction;
}					t_ray;

// Camera definition. Pre-calculating viewport values is highly recommended
typedef struct s_camera
{
	t_point3		origin;	// The camera's position (parsed 'C' coordinates)
	t_vec3			orientation;	// The point the camera is looking at
	double			vfov;	// Vertical field of view in degrees
	// --- Pre-calculated values for rendering ---
	t_vec3			u;		// Camera coordinate system basis vectors
	t_vec3			v;		// Camera coordinate system basis vectors
	t_vec3			w;		// Camera coordinate system basis vectors
	double			viewport_height;
	double			viewport_width;
	t_point3		viewport_u;	// Horizontal vector of the viewport
	t_point3		viewport_v;	// Vertical vector of the viewport
	t_point3		pixel00_loc;	// Location of the top-left pixel
}					t_camera;

// Use a linked list for lights and objects for easier parsing
typedef struct s_light
{
	t_point3		position; // Parsed 'L' coordinates
	double			ratio;	// Light brightness ratio (0.0 - 1.0)
	t_color			color;	// Parsed 'L' color (0-255 range)
	struct s_light	*next;
}					t_light;

// Generic object structure with type and data pointer
// This simplifies storage and traversal
typedef enum e_obj_type
{
	SPHERE,
	PLANE,
	CYLINDER
}					t_obj_type;

typedef struct s_object
{
	t_obj_type		type;
	t_color			color;	// Parsed object color (0-255 range)
	void			*shape_data; // Pointer to one of the structs below
	struct s_object	*next;
}					t_object;

typedef struct s_sphere
{
	t_point3		center; // Parsed 'sp' coordinates
	double			radius;	// Parsed 'sp' diameter / 2
}					t_sphere;

typedef struct s_plane
{
	t_point3		point; // A point on the plane (parsed 'pl' coordinates)
	t_vec3			normal;	// Normal vector (parsed 'pl' orientation vector,
					// (should be normalized)
}					t_plane;

typedef struct s_cylinder
{
	t_point3		center; // Center of the cylinder base (parsed 'cy' coords)
	t_vec3			axis;	// Axis vector (parsed 'cy' orientation vector,
						// should be normalized)
	double			diameter; // Parsed 'cy' diameter
	double			height;	// Parsed 'cy' height
}					t_cylinder;

// The main scene structure
typedef struct s_scene
{
	int				width;
	int				height;
	t_color			ambient_light;	// Ambient light color (0-255 range)
	double			ambient_ratio;	// Ambient light ratio (0.0 - 1.0)
	int				has_ambient;	// Flag to ensure only one ambient light
	t_camera		camera;
	int				has_camera;		// Flag to ensure only one camera
	t_light			*lights;		// Linked list of lights
	t_object		*objects;		// Linked list of objects
}					t_scene;

// Record of a ray-object intersection
typedef struct s_hit_record
{
	t_point3		p;		// Point of intersection
	t_vec3			normal;	// Surface normal at the intersection
	t_color			color;	// Color of the object hit
	double			t;		// 'time' or distance along the ray
	int				front_face;	// 1 if ray hits from outside, 0 if from inside
}					t_hit_record;

// --- Window management ---

// Holds all data related to the MiniLibX window and image buffer
typedef struct s_mlx_data
{
	void			*mlx_ptr;	// Pointer to the MLX instance
	void			*win_ptr;	// Pointer to the window
	void			*img_ptr;	// Pointer to the image buffer
	char			*addr;		// Address of the image data
	int				bits_per_pixel; // Bits per pixel in the image
	int				line_length;	// Length of a line in bytes
	int				endian;	// Endianness of the image data
}					t_mlx_data;

// A master struct to hold pointers to all major components of the program
typedef struct s_program_data
{
	t_scene			*scene;
	t_mlx_data		*mlx;
}					t_program_data;

#endif
