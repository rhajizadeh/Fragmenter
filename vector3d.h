#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <qmath.h>
class Vector3D
{
    float m_x, m_y, m_z;
public:

    Vector3D();
    Vector3D(float m_x, float m_y, float m_z);
    ~Vector3D();
    void setX(const float m_x);
    void setY(const float m_y);
    void setZ(const float m_z);
    float x() const { return m_x; }
    float y() const { return m_y; }
    float z() const { return m_z; }

    static float dotProduct(Vector3D& v1, Vector3D& v2){
        return v1.x()*v2.y() + v1.y()*v2.y() + v1.z()*v2.z();
    }
    static Vector3D crossProduct(Vector3D& v1, Vector3D& v2){
        Vector3D ans;
        ans.setX(v1.y() * v2.z() - v1.z() * v2.y());
        ans.setY(-v1.x() * v2.z() + v1.z() * v2.x());
        ans.setZ(v1.x() * v2.y() - v1.y() * v2.x());
        return ans;
    }


    float distanceToPoint(const Vector3D& v1){
        return qSqrt(qPow(m_x-v1.m_x,2) + qPow(m_y-v1.m_y,2) + qPow(m_z-v1.m_z,2));
    }
};
inline const Vector3D operator-(const Vector3D& v1, const Vector3D& v2){
    Vector3D ans;
    ans.setX(v1.x() - v2.x());
    ans.setY(v1.y() - v2.y());
    ans.setZ(v1.z() - v2.z());
    return ans;
}

inline const Vector3D operator*(const Vector3D& v1, const Vector3D& v2){
    Vector3D ans;
    ans.setX(v1.x() * v2.x());
    ans.setY(v1.y() * v2.y());
    ans.setZ(v1.z() * v2.z());
    return ans;
}
inline const Vector3D operator*(const Vector3D& v1, const double d){
    Vector3D ans;
    ans.setX(v1.x() * d);
    ans.setY(v1.y() * d);
    ans.setZ(v1.z() * d);
    return ans;
}

#endif // VECTOR3D_H
