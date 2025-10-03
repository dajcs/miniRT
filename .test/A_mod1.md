Of course. Here is a detailed elaboration of Student A's Module 1 (the parser), designed specifically to comply with the strict Norminette rules.

The key challenge with Norminette is its 25-line and 5-variable limit per function. This forces a design pattern of breaking down problems into many small, single-purpose functions. Our parser will be built on this principle.

### Parser Philosophy under Norminette

1.  **One Job Per Function:** Each function will do one thing and one thing only. For example, one function reads a line, another dispatches it, another parses a sphere, and yet another validates a vector.
2.  **Error Propagation:** Every function that can fail will return a status code (e.g., `0` for success, `-1` for error). The calling function is responsible for checking this status and acting immediately, usually by returning an error itself. This prevents deep nesting and complex logic.
3.  **Top-Down Design:** We'll design the main function first and then create the helpers it needs.
4.  **Memory Management:** Memory must be freed on any error path. This requires careful, consistent error handling.

---

### Module 1: Detailed Breakdown

#### **File: `src/parser/parser.c`**

This file contains the main entry point for the parsing logic and the top-level control flow.

**1. `int parse_scene(const char *filename, t_scene *scene)`**

*   **Prototype:** `int parse_scene(const char *filename, t_scene *scene);`
*   **Purpose:** The main orchestrator. It opens the scene file, reads it line by line, and uses helper functions to process each line.
*   **Implementation Details (Norminette Compliant):**
    1.  **Variables:** `int fd`, `char *line`, `int status`.
    2.  Validate the filename extension (`.rt`). If invalid, return an error.
    3.  Open the file. If `open()` fails, return an error.
    4.  Initialize `status` to `SUCCESS` (0).
    5.  Use a `while` loop with `get_next_line(fd, &line)` (assuming you have it in your `libft`). The loop continues as long as `get_next_line` returns `1`.
    6.  Inside the loop:
        *   If the line is not empty (e.g., not just `\n`), call `parse_line(line, scene)`.
        *   Update `status` with the return value of `parse_line`.
        *   Free the `line` returned by `get_next_line`.
        *   If `status` is an error, break the loop immediately.
    7.  After the loop, close the file descriptor (`fd`).
    8.  Call a final validation function, `validate_scene(scene)`, to ensure mandatory elements (like Camera and Ambient light) were declared.
    9.  Return the final `status`.

**2. `int parse_line(char *line, t_scene *scene)`**

*   **Prototype:** `int parse_line(char *line, t_scene *scene);`
*   **Purpose:** Acts as a dispatcher. It takes a single, non-empty line from the file, identifies the element type, and calls the appropriate specific parser.
*   **Implementation Details (Norminette Compliant):**
    1.  **Variables:** `char **tokens`, `int status`.
    2.  Use `ft_split(line, ' ')` to tokenize the line. Handle potential `malloc` failure.
    3.  Check if `tokens` or `tokens[0]` is `NULL`. If so, free `tokens` and return success (treating it as an empty line).
    4.  Use a series of `if/else if` statements with `ft_strncmp` to check `tokens[0]`:
        *   If it's "A", call `parse_ambient(tokens, scene)`.
        *   If it's "C", call `parse_camera(tokens, scene)`.
        *   If it's "L", call `parse_light(tokens, scene)`.
        *   If it's "sp", "pl", or "cy", call `parse_object(tokens, scene)`.
        *   If none match, set `status` to `ERROR` (unrecognized identifier).
    5.  Store the return value of the called function in `status`.
    6.  Free the `tokens` array using a helper function (`free_split_array`).
    7.  Return `status`.

---

#### **File: `src/parser/parser_elements.c`**

This file contains the specific logic for parsing each type of scene element.

**1. `int parse_ambient(char **tokens, t_scene *scene)`**

*   **Prototype:** `int parse_ambient(char **tokens, t_scene *scene);`
*   **Purpose:** Parses the Ambient lighting line.
*   **Implementation Details (Norminette Compliant):**
    1.  Check if ambient light has already been declared (`scene->ambient_ratio >= 0` if you initialize it to -1). If so, return an error for duplicate declaration.
    2.  Validate token count (should be 3). If not, return an error.
    3.  Call `parse_double(tokens[1], &scene->ambient_ratio)`. Check for error.
    4.  Validate that the ratio is in the range `[0.0, 1.0]`. If not, return an error.
    5.  Call `parse_color(tokens[2], &scene->ambient_light)`. Check for error.
    6.  Return `SUCCESS`.

**2. `int parse_camera(char **tokens, t_scene *scene)`**

*   **Prototype:** `int parse_camera(char **tokens, t_scene *scene);`
*   **Purpose:** Parses the Camera line.
*   **Implementation Details (Norminette Compliant):**
    1.  Check for duplicate Camera declaration.
    2.  Validate token count (should be 4).
    3.  Call `parse_vec3(tokens[1], &scene->camera.lookfrom)`. Check for error.
    4.  Call `parse_vec3(tokens[2], &scene->camera.lookat)`. Check for error and then validate it's a normalized vector using a helper.
    5.  Call `parse_double(tokens[3], &fov_temp)`. Check for error.
    6.  Validate FOV is in the range `[0, 180]`.
    7.  Assign the validated FOV to `scene->camera.fov`.
    8.  Return `SUCCESS`.

**3. `int parse_object(char **tokens, t_scene *scene)`**

*   **Prototype:** `int parse_object(char **tokens, t_scene *scene);`
*   **Purpose:** A sub-dispatcher that calls the correct object parser (`sp`, `pl`, `cy`). This helps keep `parse_line` under 25 lines.
*   **Implementation Details:**
    1.  Use `if/else if` with `ft_strncmp` on `tokens[0]`.
    2.  Call `parse_sphere(tokens, scene)`, `parse_plane(tokens, scene)`, or `parse_cylinder(tokens, scene)`.
    3.  Return the status from whichever function was called.

**4. `int parse_sphere(char **tokens, t_scene *scene)`**

*   **Prototype:** `int parse_sphere(char **tokens, t_scene *scene);`
*   **Purpose:** Parses, validates, and adds a sphere to the scene's object list.
*   **Implementation Details:**
    1.  **Variables:** `t_object *obj`, `t_sphere *sp`, `int status`.
    2.  Validate token count (should be 4).
    3.  Allocate memory for `t_object` (`obj`) and `t_sphere` (`sp`). Handle `malloc` failure.
    4.  Set `obj->type = SPHERE` and `obj->shape_data = sp`.
    5.  Call helper functions to parse and validate `tokens[1]` (center), `tokens[2]` (diameter -> radius), and `tokens[3]` (color).
    6.  On any parsing failure, free the allocated `obj` and `sp` and return `ERROR`.
    7.  If all successful, add the new object to the scene's linked list: `obj->next = scene->objects; scene->objects = obj;`.
    8.  Return `SUCCESS`.

*(The functions `parse_plane` and `parse_cylinder` will follow a very similar structure to `parse_sphere`.)*

---

#### **File: `src/parser/parser_utils.c`**

This is the most critical part for meeting Norminette constraints. It contains small, reusable helper functions.

**1. `int parse_vec3(char *str, t_vec3 *vec)`**

*   **Prototype:** `int parse_vec3(char *str, t_vec3 *vec);`
*   **Purpose:** Parses a string like "x,y,z" into a `t_vec3` struct.
*   **Implementation Details:**
    1.  **Variables:** `char **parts`, `int status`.
    2.  `parts = ft_split(str, ',')`. Handle `malloc` failure.
    3.  Check if `parts` has exactly 3 elements. If not, free `parts` and return `ERROR`.
    4.  Use a helper `ft_atof` (a robust string-to-double function you'll need) to convert `parts[0]`, `parts[1]`, and `parts[2]` into `vec->x`, `vec->y`, and `vec->z`. You might need another helper to validate that the strings are valid numbers.
    5.  Free the `parts` array.
    6.  Return `SUCCESS`.

**2. `int parse_color(char *str, t_color *color)`**

*   **Prototype:** `int parse_color(char *str, t_color *color);`
*   **Purpose:** Parses "R,G,B" into a `t_color` struct, validating range and normalizing.
*   **Implementation Details:**
    1.  Very similar to `parse_vec3`, but using `ft_atoi`.
    2.  After converting to integers, validate that each R, G, B value is in the `[0, 255]` range.
    3.  If valid, populate the `t_color` struct by normalizing the values: `color->x = r / 255.0;`, etc.
    4.  Return `SUCCESS` or `ERROR`.

**3. `int validate_normalized_vector(t_vec3 *vec)`**

*   **Prototype:** `int validate_normalized_vector(t_vec3 *vec);`
*   **Purpose:** Checks if a vector's components are in `[-1, 1]` and if its length is approximately 1.0.
*   **Implementation Details:**
    1.  Check if `vec->x`, `vec->y`, `vec->z` are all within `[-1.0, 1.0]`.
    2.  Calculate the vector's length. Check if it's very close to 1.0 (allow for small floating-point inaccuracies, e.g., `len > 0.99 && len < 1.01`).
    3.  Return `SUCCESS` or `ERROR`.

This highly modularized structure is the best way to tackle the parser while adhering to Norminette's strict rules. It makes the code clean, readable, and much easier to debug.