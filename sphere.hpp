#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <math.h>
#include "vec3.hpp"

typedef Vec3<float> vec3f;

class Sphere {
    public:
        vec3f center;                       //the center of the sphere
        float rad, rad2;              //the radius and radius squared
        vec3f surfaceColor, emissionColor;  //RGB values for colors
        float transparency, reflection;

        //sphere constructor
        Sphere(
            const vec3f &c,
            const float &r,
            const vec3f &sc,
            const float &ref = 0,
            const float &trans = 0,
            const vec3f &ec = 0) :
            center(c), rad(r), rad2(r*r), surfaceColor(sc), emissionColor(ec), 
            transparency(trans), reflection(ref)
        { /* empty*/ }

        //check if the 2 rays intersect on this sphere
        bool intersect(const vec3f &ogray, const vec3f dirray, float &t0, float &t1) const {
            vec3f l = center - ogray;
            float tca = l.dot(dirray);
            if(tca < 0) return false;
            float  d2 = l.dot(l) - tca * tca;
            if(d2 > rad2) return false;
            float thc = sqrt(rad2 - d2);
            t0 = tca - thc;
            t1 = tca + thc;
            return true;
        }

};

#endif