#pragma once

#include "util.h"

using namespace util;

class Camera {

public:
    Camera();
    Camera(const Camera& camera);
    ~Camera();
    
    aabb body;
    xy target;
    
    void update();
};

