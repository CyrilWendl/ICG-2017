//
// Created by Cyril Wendl on 23.05.17.
//
#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "icg_helper.h"

#include <cmath>
#include <vector>

class Bezier {
public:
    void add_point(glm::vec2 point){
        points.push_back(point);
        n_points=points.size();
    }

    void empty_points(){
        points.clear();
        n_points = 0;
    }

    void print_points(){
        for(auto &vec : points){
            cout << "point: " << vec.x << ", "<< vec.y << endl;
            cout << "number of points: " << n_points << endl;
        }
    }

    glm::vec2 bezier(float t){
        glm::vec2 bezier = glm::vec2(0.0f);
        for(int i=0;i<n_points;i++){
            bezier+=points.at(i)*glm::vec2(bernstein(t,n_points-1, i));
        }
        return bezier;
    }


private:
    vector<glm::vec2> points;
    int n_points;

    float bernstein(double t , int n , int i) {
        return binomial(n , i)  * pow(1 - t , n - i) * pow(t , i);
    }

    float binomial(int n , int i) {
        return fact(n) / (fact(n - i) * fact(i));
    }

    int fact(int n) {
        return (n == 1 || n == 0) ? 1 : fact(n - 1) * n;
    }
};