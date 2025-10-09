# Specular Reflection in MiniRT

### The Concept: The "Glint"

Think of specular reflection as the bright, sharp glint you see on a shiny object, like a billiard ball or a polished car. This glint is essentially a mirror-like reflection of the light source itself.

To calculate it, we need three vectors:

1.  **Light Direction (`L`):** The direction from the point on the surface *to* the light source. (You already have this).
2.  **View Direction (`V`):** The direction from the point on the surface *to* the camera.
3.  **Reflection Direction (`R`):** The direction the light ray would bounce off the surface if it were a perfect mirror.

The core idea is: **If the viewer is looking from a direction (`V`) that is very close to the mirror reflection direction (`R`), they will see a bright highlight.** The closer `V` and `R` are, the brighter the highlight.

The formula for the reflection vector `R` is: `R = 2 * (N · L) * N - L` where `N` is the surface normal.

---

### Step 1: Add Constants for Material Properties

For specular reflection, we need two new properties for our materials:

*   **Specular Strength (`SPECULAR_STRENGTH`):** How intense is the highlight? A value of `1.0` is very strong, `0.5` is moderate.
*   **Shininess (`SHININESS`):** How large is the highlight? A high value (e.g., `128`, `256`) creates a small, tight highlight like on polished metal. A low value (e.g., `8`, `16`) creates a large, blurry highlight like on plastic.

For now, since you don't have these properties in your `.rt` file, let's define them as global constants. You can add them to your `minirt.h` file.

```c
// In minirt.h
#ifndef MINIRT_H
# define MINIRT_H

// ... other includes and definitions ...

# define SPECULAR_STRENGTH 0.7
# define SHININESS 100.0

// ... rest of the file ...
#endif
```

---

### Step 2: Update `calculate_lighting()`

Now, we'll add the specular calculation inside your lighting loop. This requires the `pow()` function, so make sure you have `#include <math.h>`.

#### **BEFORE** (Diffuse + Ambient only)
```c
t_color	calculate_lighting(...)
{
	t_color	final_color;
	// ... other variables

	final_color = vec3_color_mul(scene->ambient_light, rec->color);
	final_color = vec3_mul(final_color, scene->ambient_ratio);

	light = scene->lights;
	while (light)
	{
		if (!is_in_shadow(...))
		{
			// ... diffuse calculation ...
			final_color = vec3_add(final_color, diffuse_contribution);
		}
		light = light->next;
	}
	// ... clamping and return ...
}
```

#### **AFTER** (Full Phong Model: Ambient + Diffuse + Specular)
This version adds the new specular component right after the diffuse calculation.

```c
#include <math.h> // Make sure this is included at the top of your file

/**
 * @brief Calculates the full Phong reflection model (Ambient + Diffuse + Specular).
 * @param rec   The hit_record with all intersection information.
 * @param scene The scene, containing lights and ambient light information.
 * @return      The final computed t_color for the point.
 *
 * @details This function combines all three lighting components. After
 * calculating ambient and diffuse light, it calculates the specular highlight.
 * This is done by finding the reflection of the light vector and comparing it
 * to the view vector. If they are aligned, a bright highlight (the color of
 * the light source) is added.
 */
t_color	calculate_lighting(t_hit_record *rec, t_scene *scene)
{
	t_color	final_color;
	t_light	*light;
	t_vec3	light_dir;
	double	diffuse_intensity;

	// 1. Start with the ambient light component.
	final_color = vec3_color_mul(scene->ambient_light, rec->color);
	final_color = vec3_mul(final_color, scene->ambient_ratio);

	light = scene->lights;
	while (light)
	{
		if (!is_in_shadow(rec->p, light, scene))
		{
			light_dir = vec3_normalize(vec3_sub(light->position, rec->p));
			diffuse_intensity = fmax(0.0, vec3_dot(rec->normal, light_dir));

			// 2. Add the DIFFUSE component (same as before).
			t_color diffuse_contrib;
			diffuse_contrib = vec3_color_mul(light->color, rec->color);
			diffuse_contrib = vec3_mul(diffuse_contrib, diffuse_intensity * light->ratio);
			final_color = vec3_add(final_color, diffuse_contrib);
			
			// 3. Add the SPECULAR component (NEW).
			// A highlight should only appear if the surface is facing the light.
			if (diffuse_intensity > 0.0)
			{
				t_vec3 view_dir = vec3_normalize(vec3_sub(scene->camera.lookfrom, rec->p));
				t_vec3 reflect_dir = vec3_sub(vec3_mul(rec->normal, 2.0 * vec3_dot(rec->normal, light_dir)), light_dir);
				double spec_angle = fmax(0.0, vec3_dot(view_dir, reflect_dir));
				double specular_intensity = pow(spec_angle, SHININESS);
				
				t_color specular_contrib;
				specular_contrib = vec3_mul(light->color, specular_intensity * SPECULAR_STRENGTH * light->ratio);
				final_color = vec3_add(final_color, specular_contrib);
			}
		}
		light = light->next;
	}
	// 4. Clamp the final color to the valid [0, 1] range.
	final_color.x = fmin(1.0, final_color.x);
	final_color.y = fmin(1.0, final_color.y);
	final_color.z = fmin(1.0, final_color.z);
	return (final_color);
}
```

### What You Should See

With this change, your objects will now have bright hotspots on them that correspond to the light sources.

*   The **color of the highlight** will be the color of the **light source**, not the object. A red sphere under a white light will have a white glint.
*   As you move the camera in your scene file, the **position of the highlight will move** across the object's surface.
*   You can experiment by changing the `SHININESS` constant:
    *   `SHININESS 256.0` -> Very small, sharp highlights (like polished metal).
    *   `SHININESS 16.0` -> Large, blurry highlights (like semi-gloss plastic).
