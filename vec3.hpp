#ifndef VEC3_HPP
#define VEC3_HPP

template<typename T>
class Vec3 
{
    public:
    T x, y, z;
    //constructors to initialize 3D vec
    Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
    Vec3(T xx) : x(xx), y(xx), z(xx) {}
    Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
    Vec3 & norm() {
        T norm2 = length2();
        if  (norm2 > 0) {
            T invNorm = 1 / sqrt(norm2);
            x *= invNorm;
            y *= invNorm;
            z *= invNorm;
        }
        return *this;
    }

    //define custom operators for 3D vectors
    Vec3<T> operator * (const T &f) const { return Vec3<T>(x*f, y*f, z*f);}
    Vec3<T> operator * (const Vec3<T> &v) const { return Vec3<T>(x*v.x, y*v.y, z*v.z); } 
    T dot(const Vec3<T> &v) const { return x*v.x + y*v.y + z*v.z; } 
    Vec3<T> operator - (const Vec3<T> &v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); } 
    Vec3<T> operator + (const Vec3<T> &v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); } 
    Vec3<T>& operator += (const Vec3<T> &v) { x += v.x, y += v.y, z += v.z; return *this; } 
    Vec3<T>& operator *= (const Vec3<T> &v) { x *= v.x, y *= v.y, z *= v.z; return *this; } 
    Vec3<T> operator - () const { return Vec3<T>(-x, -y, -z); } 
    T length2() const { return x*x + y*y + z*z; }
    T length() const { return sqrt(length2());}
    friend std::ostream & operator << (std::ostream &os, const Vec3<T> &v) 
    { 
        os << "[" << v.x << " " << v.y << " " << v.z << "]"; 
        return os; 
    }   
};

#endif