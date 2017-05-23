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

    void print_points(){ //
        for(auto &vec : points){
            cout << endl << "point: " << vec.x << ", "<< vec.y << endl;
        }
    }

    glm::vec2 bezier(float t){
        glm::vec2 bezier = glm::vec2(0.0f);
        for(int i=0;i<n_points;i++){
            bezier.x+=points.at(i).x*bernstein(t,n_points, i);
            bezier.y+=points.at(i).y*bernstein(t,n_points, i);
        }
        return bezier;
    }


private:
    vector<glm::vec2> points;
    int n_points;



    float bernstein(double t , int n , int i) {
        float bezier_nt=binomial(n , i)  * pow(1 - t , n - i) * pow(t , i);
        return bezier_nt;
    }

    float binomial(int n , int k) {
        return fact(n) / (fact(n - k) * fact(k));
    }

    int fact(int n) {
        return (n == 1 || n == 0) ? 1 : fact(n - 1) * n;
    }

};