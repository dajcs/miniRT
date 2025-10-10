
Checker patter looks nice on curved surfaces, but on plane surfaces there some "dust" appears. This artifact is introduced by get_pattern_color() because it appears even when only the ambien light is present


You have encountered a classic and very subtle problem in computer graphics known as **"surface acne"** or **"self-intersection artifacts,"** caused by floating-point imprecision. Your diagnosis is perfect—it is indeed happening in `get_pattern_color`, and your test with only ambient light correctly isolates it from lighting issues.

### The "Why": The Root Cause of the "Dust"

The artifact is not random dust. It's a form of **Z-fighting** happening at the boundaries of your checkerboard squares.

1.  **Floating-Point Numbers Aren't Perfect:** A `double` cannot represent every possible number. There are tiny gaps. A number that should be exactly `3.0` might be stored as `2.9999999999999996` or `3.0000000000000001`.

2.  **`floor()` is Very Sensitive:** The `floor()` function is a hard boundary. `floor(2.999...96)` is `2`, but `floor(3.000...01)` is `3`.

3.  **Planes are Perfectly Flat:** When your ray tracer calculates the intersection point `rec->p` for two adjacent pixels on a flat plane, the resulting `x`, `y`, or `z` coordinates can oscillate slightly around the integer boundaries of your checkerboard grid due to these tiny precision errors.

4.  **The Result:** A pixel that should be on the `floor(x)=2` square might get calculated as being on the `floor(x)=3` square, and its neighbor might flip back. This creates a noisy, "dusty" pattern right at the seams of the checkers.

**Why doesn't it happen on curved surfaces?**
On a sphere or cylinder, the surface is constantly curving away. The relationship between a pixel and the world coordinate is non-linear. A ray is far less likely to travel for a long distance precisely along a checkerboard boundary, so the artifact is much less visible. On a plane, a ray can graze the surface for a very long time, hitting many of these unstable boundaries.

---

### Solution: 2D Planar UV Mapping (The Correct and Robust Fix)

The industry-standard solution is to stop using the object's 3D world coordinates (`x,y,z`) for texturing a flat plane. Instead, we create a **2D coordinate system** that lies *on the plane itself*. These are called **UV coordinates**.

This is much more stable because the coordinates are relative to the plane, not the world, eliminating the floating-point fighting.

Here’s how to implement it:

#### Step 1: Create Two Basis Vectors for the Plane

We need two perpendicular vectors that lie on the plane's surface. We can generate them from the plane's normal vector.

Add this helper function, perhaps to a new `src/render/uv_mapping.c` file or `vec3_ops.c`.

```c
/**
 * @brief Creates an arbitrary, consistent tangent vector from a normal vector.
 * @param n The surface normal of the plane.
 * @return A vector that is perpendicular to the normal (lies on the plane).
 *
 * @details This function generates a reliable "right" or "U" axis for a plane.
 * It handles the edge case where the normal is aligned with the world's Y-axis
 * by using the Z-axis as a temporary up vector instead of the Y-axis.
 */
t_vec3	get_plane_u_axis(t_vec3 n)
{
	t_vec3	world_up;
	t_vec3	u_axis;

	if (fabs(n.x) < 1e-6 && fabs(n.z) < 1e-6)
		world_up = (t_vec3){0, 0, 1}; // Normal is aligned with Y, use Z as up
	else
		world_up = (t_vec3){0, 1, 0}; // Default world up
	
	u_axis = vec3_normalize(vec3_cross(world_up, n));
	return (u_axis);
}
```

#### Step 2: Create a `get_plane_uv` Function

This function will take the 3D hit point `p` and convert it into 2D `(u, v)` coordinates.

```c
/**
 * @brief Calculates the 2D UV coordinates for a hit point on a plane.
 * @param rec   The hit_record containing the 3D intersection point.
 * @param pl    The plane object.
 * @param u     Pointer to a double to store the U coordinate.
 * @param v     Pointer to a double to store the V coordinate.
 *
 * @details It establishes a local 2D coordinate system on the plane's surface
 * using the plane's normal and two generated perpendicular basis vectors (U and V).
 * It then projects the vector from the plane's origin to the hit point onto
 * these axes using the dot product to find the UV coordinates.
 */
void	get_plane_uv(t_hit_record *rec, t_plane *pl, double *u, double *v)
{
	t_vec3	u_axis;
	t_vec3	v_axis;
	t_vec3	hit_vec;

	// 1. Create the two perpendicular basis vectors on the plane.
	u_axis = get_plane_u_axis(pl->normal);
	v_axis = vec3_normalize(vec3_cross(pl->normal, u_axis));

	// 2. Get the vector from the plane's defined origin to the hit point.
	hit_vec = vec3_sub(rec->p, pl->point);

	// 3. Project the hit vector onto the U and V axes to get the 2D coords.
	*u = vec3_dot(hit_vec, u_axis);
	*v = vec3_dot(hit_vec, v_axis);
}
```

#### Step 3: Refactor `get_pattern_color` to Use the Correct Method

Now, your main pattern function becomes a dispatcher. It uses the stable 2D UV mapping for planes and can keep the simpler 3D mapping for curved surfaces where it works well.

```c
/**
 * @brief Determines the color of a point based on a procedural pattern.
 * @param rec   The hit_record containing intersection details.
 * @param obj   The object that was hit.
 * @return      One of the object's two pattern colors.
 *
 * @details This function acts as a dispatcher. For planes, it uses a stable
 * 2D UV mapping to create the checkerboard, which avoids floating-point
 * artifacts. For other objects (like spheres), it uses a simpler 3D world-space
 * checkerboard pattern.
 */
t_color	get_pattern_color(t_hit_record *rec, t_object *obj)
{
	double	u;
	double	v;
	int		sum;

	if (obj->type == PLANE)
	{
		get_plane_uv(rec, obj->shape_data, &u, &v);
		sum = (int)(floor(u * obj->pattern_scale) + \
					floor(v * obj->pattern_scale));
	}
	else // For spheres, cylinders, etc.
	{
		sum = (int)(floor(rec->p.x * obj->pattern_scale) + \
					floor(rec->p.y * obj->pattern_scale) + \
					floor(rec->p.z * obj->pattern_scale));
	}
	
	// Use `abs()` to handle negative sums correctly with modulo
	if (abs(sum) % 2 == 0)
		return (obj->color);
	else
		return (obj->color2);
}
```

By switching to this UV-based approach for planes, you are using the same robust technique found in professional render engines. Your checkerboard pattern on planes will now be perfectly clean and sharp, with no "dust" or artifacts at the edges.
