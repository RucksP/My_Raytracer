#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include<fstream>
#include<iostream>

#include "vec3.hpp"
#include "sphere.hpp"

#define MAX_RAY_RECURSION 5
//using namespace std;






/** Description: this is an implementation of the fresnel algorithm to
 * calculate the how much of the relection result we mix with the refraction
 * result
 * 
 * Params:
 * a - reflection mix value
 * b - refraction mix value
 * mix - the mixed flection and refraction color
 * 
 * Return: the result of the fresnel algortihthm
 */
float mix(const float &a, const float &b, const float &mix) {
    return b * mix + a * (1 - mix);
}

/**Description: this is the main trace function. checks if the passed ray
 * intersects any of the objects in the scene. If it does, compute the intersection
 * point and the norm at the interssection point. Then shade the scene with the given
 * information. Shading is dependent on surface property.
 * 
 * Parameters:
 * ogray - the ray which we are checking if it intersects any geometry (should be empty at start)
 * dirray - the 3d array which contains the direction array
 * spheres - a vector of objects in the scene
 * depth - the amount of recursions we have done so far (for refraction/reflection)
 * 
 * Return: the color of the object if the object intersects the ray, or the background color
 * if it does not
 */
vec3f trace ( const vec3f &ogray, vec3f &dirray,
 const std::vector<Sphere> &spheres, const int &depth) {
     float tnear = INFINITY;
     const Sphere * sphere = NULL;

     //iterate thorugh objects in scene
     for(int i = 0; i < spheres.size() ; ++i) {
         float t0 = INFINITY, t1 = INFINITY;
         if(spheres[i].intersect(ogray, dirray, t0, t1)) {
             if( t0 < 0) t0 = t1;
             if(t0 < tnear) {
                 tnear = t0;
                 sphere = &spheres[i];
             }
         }
     }

    //if no sphere was found return black
     if(!sphere) return vec3f(2);
     vec3f surfaceColor = 0;    //color of the surface
     vec3f pHit = ogray + dirray * tnear;   //this is the point of intersection
     vec3f nHit = pHit - sphere->center;    //normal at intersectino
     nHit.norm();

    // If the normal and the view direction are not opposite to each other
    // reverse the normal direction. That also means we are inside the sphere so set
    // the inside bool to true. Finally reverse the sign of IdotN which we want
    // positive.
    float bias = 1e-4;
    bool inside = false;
    if (dirray.dot(nHit) > 0) {
        nHit = -nHit;
        inside = true;
    }
    //here is the code for handling relfective / refractive objectrs
    if( (sphere->reflection > 0 || sphere->transparency > 0) && (depth < MAX_RAY_RECURSION) ) {
        float facingratio = -dirray.dot(nHit);
        float fresnel = mix(pow(1 - facingratio, 3), 1 , 0.1);
        //compute reflection direction
        vec3f refdir = dirray - nHit * 2 * dirray.dot(nHit);
        refdir.norm();
        vec3f reflection = trace(pHit + nHit * bias, refdir, spheres, depth+1);
        vec3f refraction = 0;
        // if the sphere is transparent, compute the refraction ray
        if(sphere->transparency) {
            float ior = 1.1;
            float eta = (inside) ? ior : 1 / ior;   //are we inside or outside
            float cosi = -nHit.dot(dirray);
            float k = 1 - eta * eta * (1 - cosi * cosi);
            vec3f refracdir = dirray * eta + nHit * (eta * cosi - sqrt(k));
            refracdir.norm();
            refraction = trace(pHit - nHit * bias, refracdir, spheres, depth+1);

        }

        //the resulting color is a mix of reflection and refraction
        surfaceColor = (reflection * fresnel + refraction * (1 - fresnel)
         * sphere->transparency) * sphere->surfaceColor;
    } else {    //else its a diffuse object and there is no need to raytrace further

        for(int i = 0; i < spheres.size(); ++i) {
            //if this object is a light
            if(spheres[i].emissionColor.x > 0) {
                vec3f transmission = 1;
                vec3f lightDir = spheres[i].center - pHit;
                lightDir.norm();
                for(int j = 0; j < spheres.size(); ++j) {
                    if(i != j) {
                        float t0, t1;
                        if(spheres[j].intersect(pHit + nHit * bias, lightDir, t0 , t1)) {
                            transmission = 0;
                            break;
                        }
                    }
                }
                surfaceColor += sphere->surfaceColor * transmission *
                std::max(float(0),nHit.dot(lightDir)) * spheres[i].emissionColor;
            }
        }

    }

    return surfaceColor + sphere->emissionColor;
 }

 /**Description : this is the main draw function. It renders all of the spheres
  * in the image. There is a ray drawn for each combination of height and width.
  * 
  * Params:
  * spheres - a vector which contains all of the objects in the scene
  * 
  * Return: None
  */
 void draw(const std::vector<Sphere> &spheres) {
     int width = 640;
     int height = 480;
     vec3f *image = new vec3f[width * height], *pixel = image;
     float invWidth = 1 / float(width);
     float invHeight = 1 / float(height);
     float fov = 30;
     float aspectRatio = width / float(height);
     float angle = tan(M_PI * 0.5 * fov / 180.);
     //trace rays
     for(int y = 0; y < height; ++y) {
         for(int x = 0; x < width; ++x, ++pixel) {
             //create the direction ray
             float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectRatio;
             float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
             vec3f dirray(xx,yy, -1);
             // normalize the direction ray
             dirray.norm();
             //calculate the color of this array
             *pixel = trace(vec3f(0), dirray, spheres, 0);
         }
     }
     //save result to ppm image
     std::ofstream ofs("./spheres.ppm", std::ios::out | std::ios::binary);
     ofs << "P6\n" << width << " " << height << "\n255\n";
     for(int i = 0; i < width * height; ++i) {
         ofs << (unsigned char)(std::min(float(1), image[i].x) * 255) <<
                (unsigned char)(std::min(float(1), image[i].y) * 255) <<
                (unsigned char)(std::min(float(1), image[i].z) * 255);
     }

     ofs.close();
     delete [] image;
 }

 int main (int argc, char **argv) {

     srand(13);
     std::vector<Sphere> spheres;

    // position, radius, surface color, reflectivity, transparency, emission color of each sphere
    spheres.push_back(Sphere(vec3f( 0.0, -10004, -20), 10000, vec3f(0.20, 0.20, 0.20), 0, 0.0)); 
    spheres.push_back(Sphere(vec3f( 0.0,      0, -20),     4, vec3f(1.00, 0.32, 0.36), 1, 0.5)); 
    spheres.push_back(Sphere(vec3f( 4.0,     -1, -15),     2, vec3f(0.5, 0.86, 0.46), 1, 0.0)); 
    spheres.push_back(Sphere(vec3f( 4.0,      0, -25),     3, vec3f(0.1, 0.1, 0.97), 1, 0.0)); 
    spheres.push_back(Sphere(vec3f(-5.5,      0, -20),     3, vec3f(0.85, 0.85, 0.85), 1, 0.0)); 
    // this sphere is the light source
    spheres.push_back(Sphere(vec3f( 0.0,     20, -15),     3, vec3f(0.00, 0.00, 0.00), 0, 0.0, vec3f(3))); 
    draw(spheres); 
 
    return 0; 
 }