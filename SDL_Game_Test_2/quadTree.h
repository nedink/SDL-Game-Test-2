
#ifndef quadTree_h
#define quadTree_h

#include "physics.h"

#include <stdio.h>
#include <vector>

class QuadTree {
private:
    int MAX_OBJECTS;
    int MAX_LEVELS;
    
    int level;
    std::vector<geom::xy*> objects;
    geom::aabb bounds;
    QuadTree* nodes[4];
    
    /*
     * Constructor
     */
public:
    QuadTree(int pLevel, geom::aabb pBounds) {
        level = pLevel;
//        objects = new vector<xy>();
        bounds = pBounds;
    }
};

#endif /* quadTree_h */
