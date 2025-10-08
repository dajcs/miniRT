/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minirt.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 14:36:49 by anemet            #+#    #+#             */
/*   Updated: 2025/10/08 13:07:47 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINIRT_H
# define MINIRT_H

# define _GNU_SOURCE
# include <math.h>
# include <float.h> // for DBL_MAX
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <fcntl.h>

# include "libft.h"
# include "mlx.h"

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
	double			fov;	// Horizontal field of view in degrees
	// --- Pre-calculated values for rendering ---
	t_vec3			u;		// Camera coordinate system basis vectors
	t_vec3			v;		// Camera coordinate system basis vectors
	t_vec3			w;		// Camera coordinate system basis vectors
	double			viewport_height;
	double			viewport_width;
	t_vec3			viewport_u;	// Horizontal vector of the viewport
	t_vec3			viewport_v;	// Vertical vector of the viewport
	t_vec3			pixel_delta_u;	// delta pixel vector (horizontal)
	t_vec3			pixel_delta_v;	// delta pixel vector (vertical)
	t_point3		pixel00_loc;	// Location of the top-left pixel
}					t_camera;

// Use a linked list for lights and objects for easier parsing
typedef struct s_light
{
	t_point3		position; // Parsed 'L' coordinates
	double			ratio;	// Light brightness ratio (0.0 - 1.0)
	t_color			color;	// Parsed 'L' color (0-255 range), used at bonus
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
	// t_object		*obj;	// The object that was hit
	// int			front_face;	// 1 if ray hits from outside, 0 when
								// hitting from inside. Alternatively we flip
								// the normal when hit from inside instead
}					t_hit_record;

// A helper struct to hold the coefficients of the quadratic equation
// This helps to be within the norminette 4-parameter / 5-variable limit
typedef struct s_quadratic
{
	double			a;
	double			b;
	double			c;
	double			discriminant;
	double			t1;
	double			t2;
}					t_quadratic;

// A struct to hold information about a potential (cylinder) intersection.
// It is used to pass hit data back from helper functions
typedef struct s_hit_info
{
	int				hit;	// set to 1 if valid intersection has been found
	double			hit_t;	// using hit_t to track the closest hit so far
	double			t;		// t value of a **potential** new intersection
	t_point3		p;		// the calc.3D point of a potential intersesection
	t_vec3			normal; // stores the calculated normal at the pot.inters.
}					t_hit_info;

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

/*

grep c functions:
- doesn't start with blank or comment or {}
- has an opening (

grep -E '^[^[:blank:]/#{}*][^()]*\('  *.c

*/

/*
	########### Parser Module ##############
*/

/* --- errors.c --- */
int					error_msg(char *message);

/* --- parser.c --- */
int					parse_line(char *line, t_scene *scene);
void				init_scene(t_scene *scene);
int					read_and_parse_file(int fd, t_scene *scene);
t_scene				*parse_scene(const char *filename);
void				free_scene(t_scene *scene);

/* --- parser_elements.c --- */
int					parse_ambient(char **tokens, t_scene *scene);
int					parse_camera(char **tokens, t_scene *scene);
int					parse_light(char **tokens, t_scene *scene);
int					parse_sphere(char **tokens, t_scene *scene);

/* --- parser_element2.c --- */
int					parse_plane(char **tokens, t_scene *scene);
int					parse_cylinder(char **tokens, t_scene *scene);

/* --- parser_utils.c --- */
int					count_tokens(char **tokens);
void				free_tokens(char **tokens);
int					parse_color(char *str, t_color *color);
int					parse_vec3(char *str, t_vec3 *vec, int need_norm);
int					parse_double(char *str, double *val);

/* --- parser_validation.c --- */
int					validate_ratio(double ratio);
int					validate_fov(double fov);
int					validate_norm_vec3(t_vec3 vec);

/*
	############## Window Module ###################
*/

/* --- cleanup.c --- */
int					cleanup(t_program_data *data);

/* --- hooks.c --- */

int					key_hook(int keycode, t_program_data *data);
int					close_hook(t_program_data *data);
void				setup_hooks(t_program_data *data);

/* --- window.c --- */
void				my_put_pixel_to_img(t_mlx_data *data, int x, int y,
						int color);
t_program_data		*init_program_data(char *scene_file);

/*
	############## Math Module ###################
*/

/* --- vec3_ops1.c --- */
t_vec3				vec3_add(t_vec3 v1, t_vec3 v2);
t_vec3				vec3_sub(t_vec3 v1, t_vec3 v2);
t_vec3				vec3_mul(t_vec3 v, double t);
t_vec3				vec3_div(t_vec3 v, double t);
double				vec3_dot(t_vec3 v1, t_vec3 v2);

/* --- vec3_ops2.c --- */
double				vec3_length_squared(t_vec3 v);
double				vec3_length(t_vec3 v);
t_vec3				vec3_normalize(t_vec3 v);
t_vec3				vec3_cross(t_vec3 v1, t_vec3 v2);
t_vec3				vec3_mulxyz(t_vec3 v1, t_vec3 v2);

/*
	############## Render Module ###################
*/

/* --- camera.c --- */
void				setup_camera(t_camera *cam, int img_width, int img_height);
t_ray				get_ray(t_camera *cam, int x, int y);

/* --- cylinder_intersect.c --- */
void				get_wall_normal(t_cylinder *cy, t_hit_info *info);
int					check_wall_hit(t_cylinder *cy, t_ray *ray,
						t_hit_info *info);
int					intersect_cylinder_wall(t_cylinder *cy, t_ray *r,
						t_hit_info *info);
int					check_single_cap(t_cylinder *cy, t_ray *r, t_hit_info *info,
						t_plane *cap);
int					intersect_cylinder_caps(t_cylinder *cy, t_ray *r,
						t_hit_info *info);

/* --- intersections.c --- */
int					hit_sphere(t_sphere *sp, t_ray *ray, double t_max,
						t_hit_record *rec);
int					hit_plane(t_plane *pl, t_ray *ray, double t_max,
						t_hit_record *rec);
int					hit_cylinder(t_cylinder *cy, t_ray *ray, double t_max,
						t_hit_record *rec);
int					hit_object(t_object *obj, t_ray *ray, double t_max,
						t_hit_record *rec);

/* --- ligthting.c --- */
int					is_in_shadow(t_point3 hit_point, t_light *light,
						t_scene *scene);
t_color				calculate_lighting(t_hit_record *rec, t_scene *scene);

/* --- renderer.c --- */
int					color_to_int(t_color color);
t_color				ray_color(t_ray *ray, t_scene *scene);
void				render(t_scene *scene, t_mlx_data *mlx);

#endif
