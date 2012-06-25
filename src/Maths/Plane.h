//==========================================================
//==========================================================

#ifndef PLANE_H
#define PLANE_H

#include <Maths/Vector3.h>
#include <Maths/Vector4.h>
#include <istream>

namespace Agmd
{
    class Plane
    {
    public :

        Plane(float a = 0.0f, float b = 0.0f, float c = 0.0f, float d = 0.0f);

        Plane(const vec3& v0, const vec3& v1, const vec3& v2);

        Plane(const vec3& normal, const vec3& point);

        void BuildFromPoints(const vec3& v0, const vec3& v1, const vec3& v2);

        float DistanceToPoint(const vec3& point) const;
        float DistanceToPoint(const vec4& point) const;

        bool operator ==(const Plane& p) const;
        bool operator !=(const Plane& p) const;

        vec3 n; // Normal
        float d; // Distance
    };

    std::istream& operator >>(std::istream& stream, Plane& plane);
    std::ostream& operator <<(std::ostream& stream, const Plane& plane);

    #include "Plane.inl"
}
#endif
