#include "vector3d.h"

Vector3D::Vector3D():m_x(0),m_y(0),m_z(0)
{

}

Vector3D::Vector3D(float x, float y, float z):m_x(x),m_y(y),m_z(z)
{

}

Vector3D::~Vector3D()
{

}

void Vector3D::setX(const float x)
{
    this->m_x=x;
}
void Vector3D::setY(const float y)
{
    this->m_y=y;
}
void Vector3D::setZ(const float z)
{
    this->m_z=z;
}

