#include "camera.h"

#include <stdio.h>

Camera::Camera()
{}

Camera::Camera(const Camera& camera)
{}

Camera::~Camera()
{}

void Camera::update()
{
    *this += (target - body.center()) / 10.0f;
}
