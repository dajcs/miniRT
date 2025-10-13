
```c
// ...existing code...
static int	intersect_cone_wall(t_cone *co, t_ray *r, t_hit_info *info)
{
    t_quadratic	q;
    t_vec3		oc;
    double		d_dot_a;
    double		oc_dot_a;
    double		m;
    t_vec3		v;

    oc = vec3_sub(r->origin, co->tip);
    d_dot_a = vec3_dot(r->direction, co->axis);
    oc_dot_a = vec3_dot(oc, co->axis);
    q.a = d_dot_a * d_dot_a - co->cos_angle_sq;
    q.b = 2 * (d_dot_a * oc_dot_a - vec3_dot(r->direction, oc)
            * co->cos_angle_sq);
    q.c = oc_dot_a * oc_dot_a - vec3_dot(oc, oc) * co->cos_angle_sq;
    q.discriminant = q.b * q.b - 4 * q.a * q.c;
    if (q.discriminant < 0)
        return (0);
    q.t1 = (-q.b - sqrt(q.discriminant)) / (2 * q.a);
    if (q.t1 > 0.001 && q.t1 < info->hit_t
        && is_hit_in_bounds(oc_dot_a + q.t1 * d_dot_a, co->height))
    {
        info->hit = 1;
        info->hit_t = q.t1;
        info->t = q.t1;
        info->p = vec3_add(r->origin, vec3_mul(r->direction, q.t1));
        v = vec3_sub(info->p, co->tip);
        m = vec3_dot(co->axis, v);
        info->normal = vec3_normalize(vec3_sub(vec3_mul(co->axis, m),
                    vec3_mul(v, co->cos_angle_sq)));
    }
    q.t2 = (-q.b + sqrt(q.discriminant)) / (2 * q.a);
    if (q.t2 > 0.001 && q.t2 < info->hit_t
        && is_hit_in_bounds(oc_dot_a + q.t2 * d_dot_a, co->height))
    {
        info->hit = 1;
        info->hit_t = q.t2;
        info->t = q.t2;
        info->p = vec3_add(r->origin, vec3_mul(r->direction, q.t2));
        v = vec3_sub(info->p, co->tip);
        m = vec3_dot(co->axis, v);
        info->normal = vec3_normalize(vec3_sub(vec3_mul(co->axis, m),
                    vec3_mul(v, co->cos_angle_sq)));
    }
    return (info->hit);
}
// ...existing code...
static int	intersect_cone_cap(t_cone *co, t_ray *r, t_hit_info *info)
{
    t_plane			cap;
    t_hit_record	temp_rec;
    double			radius_sq;

    cap.point = vec3_add(co->tip, vec3_mul(co->axis, co->height));
    cap.normal = co->axis;
    if (hit_plane(&cap, r, info->hit_t, &temp_rec))
    {
        radius_sq = co->height * co->height * (1.0 / co->cos_angle_sq - 1.0);
        if (vec3_length_squared(vec3_sub(temp_rec.p, cap.point)) <= radius_sq)
        {
            info->hit = 1;
            info->hit_t = temp_rec.t;
            info->t = temp_rec.t;
            info->p = temp_rec.p;
            info->normal = temp_rec.normal;
            return (1);
        }
    }
    return (0);
}
// ...existing code...
int	hit_cone(t_cone *co, t_ray *ray, double t_max, t_hit_record *rec)
{
    t_hit_info	info;

    info.hit = 0;
    info.hit_t = t_max;
    info.t = t_max;
    intersect_cone_wall(co, ray, &info);
    intersect_cone_cap(co, ray, &info);
    if (!info.hit)
        return (0);
    rec->t = info.hit_t;
    rec->p = info.p;
    rec->normal = info.normal;
    if (vec3_dot(ray->direction, rec->normal) > 0.0)
        rec->normal = vec3_mul(rec->normal, -1);
    return (1);
}
// ...existing code...

```
