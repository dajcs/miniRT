# Refraction (Transparent Materials)

Implementing transparent and semi-transparent materials is the natural next step after implementing mirrors. It's the second major component of the Whitted-Style Recursive Ray Tracing model and allows you to render stunning materials like glass, water, diamonds, and lenses.

The physical phenomenon you'll be simulating is called **refraction**.

### Conceptual Overview: What is Refraction?

When a ray of light passes from one medium to another (e.g., from air into water), it **bends**. This bending is what makes a straw in a glass of water look broken at the surface.

*   **How it's calculated:** The math is governed by **Snell's Law**, which uses a physical property of the material called the **Index of Refraction (IOR)**. The IOR measures how much the material slows down light.
    *   Vacuum: `IOR = 1.0` (baseline)
    *   Air: `IOR ≈ 1.0003` (we usually just use 1.0)
    *   Water: `IOR ≈ 1.33`
    *   Glass: `IOR ≈ 1.5`
    *   Diamond: `IOR ≈ 2.4`
*   **Total Internal Reflection:** A critical part of refraction is that sometimes, at very shallow angles, the light doesn't pass through the surface at all—it reflects perfectly off the *inside* of the surface. This is why you can see a perfect reflection of the bottom of a swimming pool if you look at the water's surface from underwater at a shallow angle. Our code **must** handle this phenomenon.

---

### Step-by-Step Guide to Implementation

The process is very similar to adding reflection, but with more complex math.

#### Step 1: Add New Properties to Objects

We need to know if an object is transparent and what its Index of Refraction is.

**A. Modify `minirt.h`:**

```c
// in minirt.h

typedef struct s_object
{
    // ... all your existing properties (color, specular, reflectivity, etc.) ...

    // --- NEW MEMBERS FOR REFRACTION ---
    double        transparency; // A value from 0.0 (opaque) to 1.0 (fully transparent)
    double        ior;          // The material's Index of Refraction (>= 1.0)

    struct s_object *next;
}               t_object;
```

**B. Update the Parser:**

Extend your `.rt` file format again to include these two new values.

**New Scene File Format Example:**
```rt
# id | ... | reflectivity | transparency | ior
   sp  0,0,20  20    255,255,255  0.1  1.0  1.5  # This is a glass sphere
   sp  5,0,15  10    255,0,0      0.0  0.0  1.0  # A normal opaque red sphere
```
*The glass sphere has low reflectivity (`0.1`), high transparency (`1.0`), and the IOR of glass (`1.5`).*

Update your parsing functions (`parse_sphere`, etc.) to read these values.

---

### Step 2: Implement the Physics (Snell's Law)

This requires a new, complex math helper function. This function will calculate the direction of the refracted ray and will also tell us if Total Internal Reflection occurred.

Add this function to your math library (`src/math/vec3_opsX.c`).

```c
/**
 * @brief Calculates the direction of a refracted ray using Snell's Law.
 * @param uv      The normalized incoming ray direction.
 * @param n       The surface normal (must point against the incoming ray).
 * @param etai_over_etat The ratio of the incident IOR to the transmitted IOR.
 * @param refracted  A pointer to a t_vec3 to store the resulting refracted ray.
 * @return      1 if refraction occurs, 0 if Total Internal Reflection occurs.
 *
 * @details This function implements Snell's Law. It also calculates the
 * discriminant to check for Total Internal Reflection (TIR). If TIR occurs,
 * no refraction is possible, and the function returns 0. Otherwise, it
 * calculates the refracted vector and returns 1.
 */
int	refract(t_vec3 uv, t_vec3 n, double etai_over_etat, t_vec3 *refracted)
{
	double	cos_theta;
	double	sin_theta_sq;
	t_vec3	r_out_perp;
	t_vec3	r_out_parallel;

	cos_theta = fmin(vec3_dot(vec3_mul(uv, -1), n), 1.0);
	sin_theta_sq = etai_over_etat * etai_over_etat * (1.0 - cos_theta * cos_theta);

	// Check for Total Internal Reflection
	if (sin_theta_sq > 1.0)
		return (0);

	r_out_perp = vec3_mul(vec3_add(uv, vec3_mul(n, cos_theta)), etai_over_etat);
	r_out_parallel = vec3_mul(n, -sqrt(fabs(1.0 - sin_theta_sq)));
	*refracted = vec3_add(r_out_perp, r_out_parallel);
	return (1);
}
```

---

### Step 3: The Final `ray_color()` Function

This is the final evolution of your tracing function. It now needs to handle **local color, reflected color, and refracted color**, and blend them all together.

```c
// in src/render/renderer.c
#define MAX_DEPTH 5

// Helper to determine the IOR ratio for Snell's Law
double get_ior_ratio(t_hit_record *rec)
{
    if (rec->front_face)
        return (1.0 / rec->ior); // Ray is entering the object (e.g., air to glass)
    else
        return (rec->ior); // Ray is exiting the object (e.g., glass to air)
}

/**
 * @brief Traces a ray recursively, calculating local, reflected, and refracted light.
 */
t_color	ray_color(t_ray *ray, t_scene *scene, int depth)
{
	t_hit_record	rec;
	t_color			final_color;

	if (depth <= 0)
		return ((t_color){0, 0, 0});
	if (!find_closest_hit(ray, scene, &rec))
		return ((t_color){0, 0, 0});

	t_color local_color = calculate_lighting(&rec, scene);
	t_color reflected_color = {0, 0, 0};
	t_color refracted_color = {0, 0, 0};

	// --- REFLECTION ---
	if (rec.reflectivity > 0)
	{
		t_ray reflection_ray = {vec3_add(rec.p, vec3_mul(rec.normal, 0.001)), vec3_reflect(ray->direction, rec.normal)};
		reflected_color = ray_color(&reflection_ray, scene, depth - 1);
	}

	// --- REFRACTION ---
	if (rec.transparency > 0)
	{
		t_ray refraction_ray;
		if (refract(ray->direction, rec.normal, get_ior_ratio(&rec), &refraction_ray.direction))
		{
			refraction_ray.origin = vec3_add(rec.p, vec3_mul(refraction_ray.direction, 0.001));
			refracted_color = ray_color(&refraction_ray, scene, depth - 1);
		}
		else // Total Internal Reflection: all light reflects
			reflected_color = ray_color(&reflection_ray, scene, depth - 1); // Using the reflection ray from above
	}

	// --- COMBINE RESULTS ---
	// Start with the refracted color, which passes through the object
	final_color = vec3_mul(refracted_color, rec.transparency);
	// Add the reflected color
	final_color = vec3_add(final_color, vec3_mul(reflected_color, rec.reflectivity));
	// Add the object's own color, scaled by how much light was NOT reflected or refracted
	// Note: For physical accuracy, reflectivity + transparency should be <= 1.0
	double local_contrib = fmax(0.0, 1.0 - rec.reflectivity - rec.transparency);
	final_color = vec3_add(final_color, vec3_mul(local_color, local_contrib));
	
	return (final_color);
}
```
*Note: You need to add `transparency` and `ior` to your `t_hit_record` struct and populate them in `hit_object`.* You also need to populate `front_face` in your intersection functions.

### What It Takes: Summary

1.  **Code Changes:** The effort is similar to implementing reflection. You'll add two `double`s to `t_object`, update the parser, add a complex but standard `refract` function to your math library, and significantly expand `ray_color` to handle a third lighting component and blend all three correctly.
2.  **Complexity:** The logic for determining the IOR ratio (am I entering or exiting the object?) and handling Total Internal Reflection makes this slightly more complex than mirrors.
3.  **Visual Payoff:** The results are spectacular. You can create realistic scenes with glass objects that both reflect their surroundings and show a distorted, magnified view of what's behind them. It's one of the most visually impressive features you can add to a ray tracer.
