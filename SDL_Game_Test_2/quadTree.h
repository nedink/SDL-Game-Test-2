
#ifndef quadTree_h
#define quadTree_h

#include "util.h"

#include <stdio.h>
#include <vector>

using namespace std;
using namespace util;

class QuadTree {
private:
    int MAX_OBJECTS;
    int MAX_LEVELS;
    
    int level;
    vector<xy*> objects;
    aabb bounds;
    QuadTree* nodes[4];
    
    /*
     * Constructor
     */
public:
    QuadTree(int pLevel, aabb pBounds) {
        level = pLevel;
//        objects = new vector<xy>();
        bounds = pBounds;
    }
};

#endif /* quadTree_h */
