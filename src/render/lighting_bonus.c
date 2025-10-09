/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lighting_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 07:13:43 by anemet            #+#    #+#             */
/*   Updated: 2025/10/09 16:56:49 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

static t_color	clamp_color(t_color color)
{
	color.x = fmin(1.0, color.x);
	color.y = fmin(1.0, color.y);
	color.z = fmin(1.0, color.z);
	return (color);
}

static t_color	add_spec_contrib(t_hit_record *rec, t_light *light,
	t_scene *scene, t_color final_color)
{
	t_spec_contrib	sc;

	sc.light_dir = vec3_normalize(vec3_sub(light->position, rec->p));
	sc.view_dir = vec3_normalize(vec3_sub(scene->camera.origin, rec->p));
	sc.reflect_dir = vec3_reflect(vec3_mul(sc.light_dir, -1), rec->normal);
	sc.spec_angle = fmax(0.0, vec3_dot(sc.view_dir, sc.reflect_dir));
	sc.spec_intensity = pow(sc.spec_angle, rec->shine);
	sc.spec_contrib = vec3_mul(light->color, sc.spec_intensity * rec->speci);
	sc.spec_contrib = vec3_mul(sc.spec_contrib, light->ratio);
	final_color = vec3_add(final_color, sc.spec_contrib);
	return (final_color);
}

/* is_in_shadow()
	Checks if a given point is in shadow from a specific light source
	Input:
		hit_point:	the point on the surface we are shading
		*light:		the light source to check against
		*scene:		the scene containing all objects
	Return: 1 if the point is in shadow, 0 otherwise

	It works by creating a new "shadow ray" that originates from the hit point
	and travels towards the light. We then check if this ray intersects with
	any other object in the scene before it reaches the light.
	A small epsilon (0.001) is added to the ray's origin to prevent the ray
	from intersecting with the object it originated from ("shadow acne").
*/
int	is_in_shadow(t_point3 hit_point, t_light *light, t_scene *scene)
{
	t_ray			shadow_ray;
	t_hit_record	rec;
	double			light_dist;
	t_object		*obj;

	shadow_ray.direction = vec3_sub(light->position, hit_point);
	light_dist = vec3_length(shadow_ray.direction);
	shadow_ray.direction = vec3_normalize(shadow_ray.direction);
	shadow_ray.origin = vec3_add(hit_point, vec3_mul(shadow_ray.direction,
				0.001));
	obj = scene->objects;
	while (obj)
	{
		if (hit_object(obj, &shadow_ray, light_dist, &rec))
			return (1);
		obj = obj->next;
	}
	return (0);
}

/* calculate_lighting()
	Calculates the final color of a point on an object's surface
	Input:
		*rec	the hit_record containing all information about the intersection
		*scene	the scene, containing lights and ambient light information
	Return: the final computed t_color for the point

	The function implements the Phong lighting model (mandatory part).
	It starts with the base ambient light. Then for each light in the scene it
	checks if the point is in shadow. If not, it calculates the diffuse
	contribution based on the angle between the surface normal and the light
	direction (Lambert's cosine law) and adds it to the final color.
	Finally clamp colors to [0,1] range

	Phong model:
	1.) Ambient Lighting: This represents indirect,
		scattered light in the scene.
	It's a constant base color applied to an object, ensuring it's never
	completely black, even in areas not directly lit.

	2.) Diffuse Lighting: This simulates light from a specific source that hits
	a surface and scatters equally in all directions. The brightness depends on
	the angle between the surface normal and the direction of the light.
	A surface facing the light directly will be brighter than one at an angle.
	This component gives objects their matte appearance.
		- calculate geometric diffuse factor `diffuse_intensity` (dot product)
		- calculate final diffuse contribution from this light:
			Contribution = (light_color * object_color) * intensity * ratio
		- add this diffuse_color to the final_color

	3.) Specular Lighting: This simulates shiny highlights or reflections of a
	light source on the surface. The intensity of the highlight depends on the
	viewing angle. It's brightest when the viewing direction is aligned with
	the reflection of the light source.

			// Specular add-on (for bonus)
			view_dir = vec3_normalize(vec3_sub(ray->origin, rec->p));
			reflect_dir = reflect(vec3_mul(light_dir, -1), rec->normal);
			specular_intensity = pow(fmax(0.0, vec3_dot(view_dir, reflect_dir)),
				shininess);
			specular_intensity *= light->ratio; // Use a specular strength if
												// available
			// Add specular highlight (assuming white light)
			final_color = vec3_add(final_color, vec3_mul((t_color){1,1,1},
				specular_intensity));

*/
t_color	calculate_lighting(t_hit_record *rec, t_scene *scene)
{
	t_color	final_color;
	t_vec3	light_dir;
	double	diffuse_intensity;
	t_color	diffuse_color;
	t_light	*light;

	final_color = vec3_mul(scene->ambient_light, scene->ambient_ratio);
	final_color = vec3_color_mul(final_color, rec->color);
	light = scene->lights;
	while (light)
	{
		if (!is_in_shadow(rec->p, light, scene))
		{
			light_dir = vec3_normalize(vec3_sub(light->position, rec->p));
			diffuse_intensity = fmax(0.0, vec3_dot(rec->normal, light_dir));
			diffuse_color = vec3_color_mul(light->color, rec->color);
			diffuse_color = vec3_mul(diffuse_color,
					diffuse_intensity * light->ratio);
			final_color = vec3_add(final_color, diffuse_color);
			final_color = add_spec_contrib(rec, light, scene, final_color);
		}
		light = light->next;
	}
	final_color = clamp_color(final_color);
	return (final_color);
}
