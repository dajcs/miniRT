# Handling Bump Map Texture Artifacts in miniRT

When applying bump mapping in miniRT, you might notice some unwanted artifacts, such as "dust" or noise on surfaces, especially on planes. This is often due to floating-point precision issues when calculating texture coordinates and normals.

It's a significant step up in complexity from the checkerboard pattern but delivers a huge "wow" factor.

### 1. The "What": The Visual Effect

The "Handle bump map textures" feature creates the **illusion of fine, detailed surface geometry without actually adding any new polygons or making the object more complex**.

Imagine you want to render a brick wall.
*   **Without bump mapping:** Your wall is a perfectly flat plane. The brick texture you apply looks flat and painted on. It doesn't react realistically to light.
*   **With bump mapping:** The lighting on the wall behaves *as if* the bricks have depth and the mortar is recessed. When a light moves across the surface, the highlights and shadows will realistically play across the bumpy texture of the bricks and the rough mortar, making the wall look incredibly detailed and three-dimensional.

**The key is that it's an *illusion*.** The wall is still a perfectly flat plane. Its silhouette will be a straight line. But the shading on its surface is tricked into thinking it's bumpy.

**Analogy:** Think of the difference between a perfectly smooth billiard ball and an orange. Both are spheres, but the orange's peel has tiny bumps and craters. Bump mapping is a technique to make the smooth billiard ball *look* and *shade* like it has the detailed surface of the orange peel.

### 2. The "How": The Core Trick - Faking the Normals

The entire lighting calculation you've built (diffuse and specular) depends on one crucial piece of information: the **surface normal**. The normal is the vector that tells the light which way the surface is facing at a given point.

The core trick of bump mapping is to **perturb (slightly alter) the surface normal at render time** based on information from a texture map.

Here's the process:

1.  **The Bump Map:** You start with a special kind of texture: a grayscale image called a **bump map** (or a height map). In this image, the color of each pixel doesn't represent actual color, but rather "perceived height."
    *   White pixels represent the highest points.
    *   Black pixels represent the lowest points.
    *   Gray pixels are somewhere in between.

2.  **Perturbing the Normal:** When your ray hits the object (e.g., the flat wall plane):
    *   You first calculate the object's true, geometric normal. For the wall, this would be a vector pointing straight out, perpendicular to the plane.
    *   You then figure out which pixel in the bump map corresponds to the hit point on the wall.
    *   You look at the grayscale value of that pixel and its immediate neighbors in the image.
    *   By comparing the "heights" of neighboring pixels, you can calculate a **gradient**, or "slope," of the texture at that point.
    *   This gradient gives you a direction. You use this direction to slightly "nudge" or alter the original, perfectly flat geometric normal.
    *   The result is a **new, perturbed normal** that now points slightly askew, as if it were coming off a bumpy surface.

3.  **The Final Shading:** You then pass this **fake, perturbed normal** to your `calculate_lighting()` function. The lighting function, having no idea it's been tricked, calculates the diffuse and specular light as if the surface were actually oriented in this new, bumpy direction. This is what creates the incredibly realistic lighting illusion.

### 3. Implementation Steps

Implementing this is a multi-stage process, significantly more involved than the checkerboard pattern.

#### **Step 1: Prerequisite - Image Loading**

You need a way to load image files. For miniRT projects, the standard is to use the `mlx_xpm_file_to_image` function from the MiniLibX to load `.xpm` files. This will give you a buffer of pixel data in memory.

#### **Step 2: Update Data Structures (`minirt.h`)**

Your `t_object` needs to be able to store the texture data.

```c
// in minirt.h

// A struct to hold texture data
typedef struct s_texture
{
    void    *img_ptr;
    char    *addr;
    int     width;
    int     height;
    int     bits_per_pixel;
    int     line_length;
    int     endian;
}               t_texture;

typedef struct s_object
{
    // ... all the other members ...

    // --- NEW MEMBERS FOR BUMP MAPPING ---
    t_texture   *bump_map; // Pointer to the loaded bump map texture
    double      bump_intensity; // How strong the effect is
}               t_object;
```

#### **Step 3: Update the Parser**

You need to add a way to specify a bump map file in your scene description.

**New Scene File Format Example:**
```c
// id | ... other params ... | bump_map_file.xpm | bump_intensity
   pl   0,-10,0 0,1,0 0,255,0 0.3 50  textures/bricks.xpm 1.0
```
Your parser would read the filename, use `mlx_xpm_file_to_image` to load it, and store the resulting `t_texture` struct in the object.

#### **Step 4: Texture Coordinates (UV Mapping) - The Hardest Part**

This is the most critical step. You need a function that can take any 3D intersection point `p` on an object and map it to a 2D coordinate `(u, v)` on your texture image. These `(u, v)` coordinates range from `(0,0)` (bottom-left of the image) to `(1,1)` (top-right).

This requires a different mathematical formula for each shape:

*   **Plane:** Project the 3D point onto two axes. For a Y-up plane, `u = p.x` and `v = p.z`. You can add scaling and modulo to make the pattern repeat.
*   **Sphere:** Use spherical coordinates (like latitude and longitude).
    *   `u = 0.5 + atan2(p.z, p.x) / (2 * M_PI)`
    *   `v = 0.5 - asin(p.y) / M_PI`
*   **Cylinder:**
    *   **Walls:** "Unwrap" the cylinder. `u` is the angle around the axis (like the sphere's `u`), and `v` is the height along the axis (`v = p.y / height`).
    *   **Caps:** Use planar mapping, similar to the plane.

#### **Step 5: The Normal Perturbation Logic**

This is the final step, which happens right after an intersection is found (e.g., at the end of `hit_object`).

1.  Calculate the `(u, v)` texture coordinates for the hit point `rec->p`.
2.  Use these `(u, v)` coordinates to find the corresponding `(x, y)` pixel in your loaded bump map image.
3.  Sample the grayscale value at `(x, y)`. Also sample the values at `(x+1, y)` and `(x, y+1)`.
4.  The difference in "height" (grayscale value) between `(x, y)` and `(x+1, y)` gives you a gradient in the `u` direction (`grad_u`).
5.  The difference in "height" between `(x, y)` and `(x, y+1)` gives you a gradient in the `v` direction (`grad_v`).
6.  You now have the original geometric normal (`rec->normal`) and two tangent vectors that define the "slope" of the bump. You combine these three vectors to create the new, perturbed normal.
7.  Finally, you overwrite `rec->normal` with this new vector before calling `calculate_lighting()`.

**In summary, bump mapping is a powerful illusion that adds incredible realism by tricking your lighting calculations with fake surface normals derived from a texture image.**
