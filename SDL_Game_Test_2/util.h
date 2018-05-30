#pragma once

#include <SDL2/SDL_rect.h>

namespace util {
    
    struct xy
    {
        float x;
        float y;
        
        xy operator + (xy arg) {
            return {x + arg.x, y + arg.y};
        }
        
        xy operator - (xy arg) {
            return {x - arg.x, y - arg.y};
        }
        
        xy operator * (xy arg) {
            return {x * arg.x, y * arg.y};
        }
        
        xy operator / (xy arg) {
            return {x / arg.x, y / arg.y};
        }
        
        xy operator + (float arg) {
            return {x + arg, y + arg};
        }
        
        xy operator - (float arg) {
            return {x - arg, y - arg};
        }
        
        xy operator * (float arg) {
            return {x * arg, y * arg};
        }
        
        xy operator / (float arg) {
            return {x / arg, y / arg};
        }
        
        void operator += (xy arg) {
            x += arg.x;
            y += arg.y;
        }
        
        void operator -= (xy arg) {
            x -= arg.x;
            y -= arg.y;
        }
        
        void operator *= (xy arg) {
            x *= arg.x;
            y *= arg.y;
        }
        
        void operator /= (xy arg) {
            x /= arg.x;
            y /= arg.y;
        }
        
        void operator += (float arg) {
            x += arg;
            y += arg;
        }
        
        void operator -= (float arg) {
            x -= arg;
            y -= arg;
        }
        
        void operator *= (float arg) {
            x *= arg;
            y *= arg;
        }
        
        void operator /= (float arg) {
            x /= arg;
            y /= arg;
        }
    };
    
    struct v2
    {
        xy xy;
        float m;
    };
    
    
    struct aabb
    {
        xy xy;
        float w;
        float h;
        
        inline float l() {
            return xy.x;
        }
        
        inline float r() {
            return xy.x + w;
        }
        
        inline float t() {
            return xy.y;
        }
        
        inline float b() {
            return xy.y + h;
        }
        
        inline struct xy tl() { // upper-left
            return xy;
        }
        
        inline struct xy tr() { // upper-right
            return {xy.x + w, xy.y};
        }
        
        inline struct xy bl() { // bottom-left
            return {xy.x, xy.y + h};
        }
        
        inline struct xy br() { // bottom-right
            return {xy.x + w, xy.y + h};
        }
        
        inline struct xy center() {
            return {xy.x + w / 2, xy.y + h / 2};
        }
        
        inline struct aabb operator + (struct xy arg) {
            return aabb{(struct xy){xy.x + arg.x, xy.y + arg.y}, w, h};
        }
        
        inline struct aabb operator - (struct xy arg) {
            return aabb{(struct xy){xy.x - arg.x, xy.y - arg.y}, w, h};
        }
        
        inline struct aabb operator * (struct xy arg) {
            return aabb{(struct xy){xy.x * arg.x, xy.y * arg.y}, w, h};
        }
        
        inline struct aabb operator / (struct xy arg) {
            return aabb{(struct xy){xy.x / arg.x, xy.y / arg.y}, w, h};
        }
        
        inline void operator += (struct xy arg) {
            xy += arg;
        }
        
        inline void operator -= (struct xy arg) {
            xy -= arg;
        }
        
        inline void operator *= (struct xy arg) {
            xy *= arg;
        }
        
        inline void operator /= (struct xy arg) {
            xy /= arg;
        }
        
        inline void operator = (struct xy arg) {
            xy = arg;
        }
        
        inline bool contains(struct xy arg) {
            return arg.x >= l() && arg.x <= r() && arg.y >= t() && arg.y <= b();
        }
        
        inline bool overlaps(aabb arg) {
            return l() < arg.r() && r() > arg.l() && t() < arg.b() && b() > arg.t();
        }
        
        void centerOn(struct xy arg) {
            xy = arg - center();
        }
        
        SDL_Rect toSDL() {
            return SDL_Rect{ (int)xy.x, (int)xy.y, (int)w, (int)h };
        }
    };
    
    struct line {
        xy head;
        xy end;
    };
    
    struct color {
    	unsigned short r;
        unsigned short g;
        unsigned short b;
        unsigned short a;
    };
}

