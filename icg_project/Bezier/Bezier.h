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
    // TODO replace by base Bezier function
    double bezier_simple(double A ,  // Start value
                         double B ,  // First control value
                         double C ,  // Second control value
                         double D ,  // Ending value
                         double t)  // Parameter 0 <= t <= 1
    {
        double s = 1 - t;
        double AB = A * s + B * t;
        double BC = B * s + C * t;
        double CD = C * s + D * t;
        double ABC = AB * s + BC * t;
        double BCD = BC * s + CD * t;
        return ABC * s + BCD * t;
    }
    glm::vec2 bezier_simple_2D(glm::vec2 t){
        glm::vec2 A=glm::vec2(points.at(0));
        glm::vec2 B=glm::vec2(points.at(1));
        glm::vec2 C=glm::vec2(points.at(2));
        glm::vec2 D=glm::vec2(points.at(3));
        glm::vec2 s = glm::vec2(1.0f - t);
        glm::vec2 AB = A * s + B * t;
        glm::vec2 BC = B * s + C * t;
        glm::vec2 CD = C * s + D * t;
        glm::vec2 ABC = AB * s + BC * t;
        glm::vec2 BCD = BC * s + CD * t;
        return ABC * s + BCD * t;
    };

    void go_to_2d(){

    }
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
            cout << endl << "point: " << vec.x << ", "<< vec.y << ", " << endl;
        }
    }
    void print_test(float t){ // simply print first point
        cout << bezier(t,n_points-1);
    }


private:
    vector<glm::vec2> points;
    int n_points;

    float bezier(float t, int n){
        float sum;
        for(int i=0;i<n;i++){
            cout << "i : " << i << endl;
            sum+=points.at(i).x*bernstein(t,n, i);
        }
        return sum;
    }

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