/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 10:52:00 by anemet            #+#    #+#             */
/*   Updated: 2025/10/03 16:52:34 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

/* setup_camera()
	Pre-calculate camera properties needed for rendering
	Input: t_camera *cam, int img_width, int img_height
	Return: void, sets camera parameters in place

	1. Setup the camera's orthonormal basis: u, v, w
		cam->w = normalize(lookfrom - lookat);
		cam->u = normalize(cross(up, w)); // up = t_vec3{0, 1 , 0}
		cam->v = cross(w, u);
	2. Calculate viewport dimensions: viewport_width, viewport_height for a
		pinhole camera at distance focal_length = 1.0
		The viewport is an imaginary plane at 1.0 unit in front of the camera
	3. Calculate viewport basis vectors: viewport_u, viewport_v
		these are vectors spanning the whole width and height of the viewport
		flip viewport_v sign to match screen coords (top->down)
	4. Calculate the per-pixel increment vectors: pixel_delta_u, pixel_delta_v
		we are dividing the total span vectors by the resolution
	5. Calculate the upper-left corner of the viewport
		distance 1.0 along w, - viewport_u / 2 - viewport_v / 2
	6. Set pixel00 location by shifting half a pixel down and to the right
		to get the middle of the pixel
*/
void	setup_camera(t_camera *cam, int img_width, int img_height)
{
	t_point3	viewport_upper_left;

	cam->w = vec3_normalize(vec3_sub(cam->origin, cam->orientation));
	cam->u = vec3_normalize(vec3_cross((t_vec3){0, 1, 0}, cam->w));
	cam->v = vec3_cross(cam->w, cam->u);
	cam->viewport_width = 2.0 * tan((cam->fov * M_PI / 180.0) / 2.0) * 1.0;
	cam->viewport_height = cam->viewport_width * ((double)img_height
			/ img_width);
	cam->viewport_u = vec3_mul(cam->u, cam->viewport_width);
	cam->viewport_v = vec3_mul(vec3_mul(cam->v, -1), cam->viewport_height);
	cam->pixel_delta_u = vec3_div(cam->viewport_u, img_width);
	cam->pixel_delta_v = vec3_div(cam->viewport_v, img_height);
	viewport_upper_left = vec3_sub(cam->origin, vec3_mul(cam->w, 1.0));
	viewport_upper_left = vec3_sub(viewport_upper_left,
			vec3_div(cam->viewport_u, 2));
	viewport_upper_left = vec3_sub(viewport_upper_left,
			vec3_div(cam->viewport_v, 2));
	cam->pixel00_loc = vec3_add(viewport_upper_left,
			vec3_mul(vec3_add(cam->pixel_delta_u, cam->pixel_delta_v), 0.5));
}

/* get_ray()
	Creates a ray from camera to a given pixel coordinate (x, y)
	Input: t_camera *cam, (containing pixel delta vectors)
		int x: pixel coordinate (from 0 to width -1)
		int y: pixel coordinate (from 0 to height - 1)
	Return: a t_ray originating from the camera and passing through the pixel
	- Starts from the pre-calculated center of pixel (0, 0).
	- "walks" x steps horizontally using pixel_delta_u
	- "walks" y steps vertically using pixel_delta_v
	- creates a normalized direction vector from the camera's origin to the
		calculated pixel center
*/
t_ray	get_ray(t_camera *cam, int x, int y)
{
	t_point3	pixel_center;
	t_vec3		temp_vec;

	temp_vec = vec3_add(vec3_mul(cam->pixel_delta_u, x),
			vec3_mul(cam->pixel_delta_v, y));
	pixel_center = vec3_add(cam->pixel00_loc, temp_vec);
	return ((t_ray){cam->origin, vec3_normalize(vec3_sub(pixel_center,
				cam->origin))});
}
