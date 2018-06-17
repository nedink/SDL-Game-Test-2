
#ifndef bullet_h
#define bullet_h

#include "util/physics.h"

class Bullet : public phys::PhysObj {
public:
    Bullet();
    ~Bullet();
    
    void update();
};

#endif /* bullet_h */
