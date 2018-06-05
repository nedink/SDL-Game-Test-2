//
//  physics.h
//  SDL_Game_Test_2
//
//  Created by David Bryant on 6/2/18.
//  Copyright © 2018 nedink. All rights reserved.
//

#ifndef physics_h
#define physics_h

#include <SDL2/SDL_rect.h>

namespace geom {
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
    
    struct aabb
    {
        xy pos;
        float w;
        float h;
        
        void setWidth(float arg) {
            w = arg;
        }
        
        void setHeight(float arg) {
            h = arg;
        }
        
        float l() {
            return pos.x;
        }
        
        float r() {
            return pos.x + w;
        }
        
        float t() {
            return pos.y;
        }
        
        float b() {
            return pos.y + h;
        }
        
        struct xy tl() { // upper-left
            return pos;
        }
        
        struct xy tr() { // upper-right
            return {pos.x + w, pos.y};
        }
        
        struct xy bl() { // bottom-left
            return {pos.x, pos.y + h};
        }
        
        struct xy br() { // bottom-right
            return {pos.x + w, pos.y + h};
        }
        
        struct xy center() const {
            return {pos.x + w / 2, pos.y + h / 2};
        }
        
        aabb operator + (struct xy arg) {
            xy newPos = pos + arg;
            return aabb{newPos, w, h};
        }
        
        struct aabb operator - (struct xy arg) {
            xy newPos = pos - arg;
            return aabb{newPos, w, h};
        }
        
        struct aabb operator * (struct xy arg) {
            xy newPos = pos * arg;
            return aabb{newPos, w, h};
        }
        
        struct aabb operator / (struct xy arg) {
            xy newPos = pos / arg;
            return aabb{newPos, w, h};
        }
        
        void operator += (struct geom::xy arg) {
            pos += arg;
        }
        
        void operator -= (struct geom::xy arg) {
            pos -= arg;
        }
        
        void operator *= (struct geom::xy arg) {
            pos *= arg;
        }
        
        void operator /= (struct geom::xy arg) {
            pos /= arg;
        }
        
        void operator = (struct geom::xy arg) {
            pos = arg;
        }
        
        bool contains(struct xy arg) {
            return arg.x >= l() && arg.x <= r() && arg.y >= t() && arg.y <= b();
        }
        
        bool overlaps(aabb arg) {
            return l() < arg.r() && r() > arg.l() && t() < arg.b() && b() > arg.t();
        }
        
        void centerOn(struct xy arg) {
            pos = arg - center();
        }
        
        SDL_Rect toSDL() {
            return SDL_Rect{ (int)pos.x, (int)pos.y, (int)w, (int)h };
        }
    };
    
    struct circle {
        float r;
        xy pos;
        
        xy center() const {
            return pos;
        }
    };
    
    struct line {
        xy start;
        xy end;
        
        float run() {
            return start.x - end.x;
        }
        
        float rise() {
            return start.y - end.y;
        }
        
        float length() {
            return sqrt(run() * run() + rise() * rise());
        }
        
        line normalized() {
            xy diff = {end - start};
            xy dist = {diff.x / length(), diff.y / length()};
            return line{start, start + dist};
        }
    };
}



namespace phys {
    
    class PhysObj {
    protected:
        geom::aabb body;
        geom::xy& pos = body.pos;
        geom::xy vel;
        
    public:
        
        void operator += (struct geom::xy arg) {
            body += arg;
        }
        
        void operator -= (struct geom::xy arg) {
            body -= arg;
        }
        
        void operator *= (struct geom::xy arg) {
            body *= arg;
        }
        
        void operator /= (struct geom::xy arg) {
            body /= arg;
        }
        
        void operator = (struct geom::xy arg) {
            body = arg;
        }
        
        float l() {
            return body.l();
        }
        
        float r() {
            return body.r();
        }
        
        float t() {
            return body.t();
        }
        
        float b() {
            return body.b();
        }
        
        geom::xy tl() { // upper-left
            return body.tl();
        }
        
        geom::xy tr() { // upper-right
            return body.tr();
        }
        
        geom::xy bl() { // bottom-left
            return body.bl();
        }
        
        geom::xy br() { // bottom-right
            return body.br();
        }
        
        geom::xy center() const {
            return body.center();
        }
        
        void centerOn(geom::xy arg) {
            body.centerOn(arg);
        }
        
        void applyForce(geom::xy arg) {
            vel += arg;
        }
        
        void setPos(geom::xy arg) {
            pos = arg;
        }
        
        void setWidth(float arg) {
            body.w = arg;
        }
        
        void setHeight(float arg) {
            body.h = arg;
        }
        
        void setVel(geom::xy arg) {
            vel = arg;
        }
        
        geom::aabb getBody() const {
            return body;
        }
        
        geom::xy getPos() const {
            return pos;
        }
        
        geom::xy getVel() const {
            return vel;
        }
        
        void displace(geom::xy arg) {
            pos += arg;
        }
        
        SDL_Rect toSDL() {
            return body.toSDL();
        }
    };
}


#endif /* physics_h */
