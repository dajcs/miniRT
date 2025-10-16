Our current `L` lights are abstract, invisible points in space. You can't see them, you can only see the light they cast on other objects.

Implementing light-emitting materials (or "emissive" materials) allows you to create visible light sources, like a glowing sphere, a neon sign, or a rectangular ceiling light.

There are two main ways to approach this, with a huge difference in complexity and realism.

---

### Approach 1: The Simple "Glow" Effect (Recommended for miniRT)

This is the most common and practical way to implement this as a bonus feature.

**The Concept:** The object doesn't *actually* cast light onto other objects. It simply tells the ray tracer, "If you hit me, stop what you're doing and just return my bright, emissive color." It's self-illuminated and ignores all incoming light, shadows, or reflections.

**What it takes to implement:**

#### Step 1: Modify Data Structures (`minirt.h`)

Add a new property to your `t_object` struct. The simplest way is to add a `t_color` for emission. If the color is black (`<0,0,0>`), the object is not emissive.

```c
// in minirt.h

typedef struct s_object
{
    // ... all your existing properties ...
    double        reflectivity;
    double        transparency;
    double        ior;

    // --- NEW MEMBER FOR EMISSION ---
    t_color       emission_color; // Color this object emits. Black = non-emissive.

    struct s_object *next;
}               t_object;
```

#### Step 2: Update the Parser

Extend your `.rt` file format to read this new color. It's often the very last property on an object's line.

**New Scene File Format Example:**
```rt
# id | ... | ior | emission_color
   sp  -10,0,20 10 255,255,255 0 0 1.0 255,255,255 # A normal white sphere
   sp  10,0,20  5  0,0,0       0 0 1.0 255,165,0   # An orange glowing sphere
```
*The second sphere has a black base color (which will be ignored) and an orange emission color.*

Update your parsing functions (`parse_sphere`, etc.) to read this extra color. If it's not present, default it to black (`<0,0,0>`).

#### Step 3: Modify the `ray_color()` Function

This is where the magic happens. It's a very simple but powerful change.

```c
// in src/render/renderer.c

t_color	ray_color(t_ray *ray, t_scene *scene, int depth)
{
	t_hit_record	rec;

	if (depth <= 0)
		return ((t_color){0, 0, 0});
	if (!find_closest_hit(ray, scene, &rec))
		return ((t_color){0, 0, 0});

	// --- THE EMISSION CHECK ---
	// If the object we hit has an emission color (i.e., not black),
	// we immediately return that color. We skip all lighting, reflection,
	// and refraction calculations for this object. It simply glows.
	if (rec.emission_color.x > 0 || rec.emission_color.y > 0 || rec.emission_color.z > 0)
	{
		return (rec.emission_color);
	}

	// --- IF NOT EMISSIVE, PROCEED AS NORMAL ---
	t_color local_color = calculate_lighting(&rec, scene);
	t_color reflected_color = {0, 0, 0};
	t_color refracted_color = {0, 0, 0};

	// ... rest of your reflection and refraction logic ...

	// ... combine colors as before ...
	
	return (final_color);
}
```
*Note: You need to add `emission_color` to your `t_hit_record` struct and populate it in `hit_object`.*

**Pros of this approach:**
*   **Easy to implement:** Very few lines of code are needed.
*   **Fast:** It adds almost no performance overhead.
*   **Achieves the goal:** You can now see your light sources in the final render.

**Cons of this approach:**
*   **It's a "hack":** The glowing sphere is visible, but it **does not cast any light or shadows** onto other objects in the scene. The floor next to the glowing orange sphere will not have an orange glow on it. Illumination still *only* comes from your invisible `L` point lights.

---

### Approach 2: True Area Lighting (Physically-Based)

This is the "correct" but vastly more complex approach. It turns emissive objects into actual light sources.

**The Concept:** When your ray tracer is calculating the color of a point on the floor, it needs to understand that light is arriving not just from the `L` point lights, but from every point on the surface of the glowing sphere.

**What it takes to implement:**

This requires fundamentally changing your renderer from a Whitted-Style Ray Tracer into a **Path Tracer**.

1.  **Remove Point Lights:** You would get rid of the `L` lights entirely. All light now comes from emissive objects.
2.  **Change `calculate_lighting`:** This function would be completely replaced. Instead of looping through a list of lights, it would perform "next event estimation."
3.  **Sampling:** For a point on the floor, you would:
    a. Pick a random point on the surface of a light-emitting object (e.g., the glowing sphere).
    b. Cast a shadow ray from the floor point to that random point on the sphere.
    c. If the ray isn't blocked, you add the sphere's light to the floor's color, attenuated by distance and angle.
4.  **Monte Carlo Integration:** To get a smooth result (and soft shadows!), you must repeat step 3 **many times** (dozens or hundreds) for every single pixel, picking a new random point on the light source each time, and averaging the results.

**Why this is not recommended for miniRT:**
*   **Massive Complexity:** It's a completely different rendering algorithm.
*   **Performance:** It is extremely slow. A path tracer is orders of magnitude slower than a ray tracer because of the huge number of rays it must cast per pixel to get a clean, noise-free image.
*   **Project Scope:** It's far beyond the scope of a typical `miniRT` project and its bonuses.

### Conclusion and Recommendation

For your project, **absolutely go with Approach 1 (The "Glow" Effect).** It perfectly fulfills the spirit of the bonus, is easy to implement, and gives you a great visual result for the effort. You will be able to see your light sources, which is the primary goal.
