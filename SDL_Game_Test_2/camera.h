#pragma once

#include "util/physics.h"

class Camera : public phys::PhysObj {

public:
    Camera();
    Camera(const Camera& camera);
    ~Camera();
    
    geom::xy target;
    
    void update();
};

