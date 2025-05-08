/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Matrix.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    March 24, 2025
*/
#include "Matrix.h"
#include <cmath>

Math::TransformationMatrix::TransformationMatrix()
{
    Reset();
}

void Math::TransformationMatrix::Reset() {
    
    matrix[0][0] = 1.0; matrix[0][1] = 0.0; matrix[0][2] = 0.0;
    matrix[1][0] = 0.0; matrix[1][1] = 1.0; matrix[1][2] = 0.0;
    matrix[2][0] = 0.0; matrix[2][1] = 0.0; matrix[2][2] = 1.0;
}


Math::TransformationMatrix Math::TransformationMatrix::operator * (TransformationMatrix m) const {
    TransformationMatrix result;

    result.matrix[0][0] = matrix[0][0] * m.matrix[0][0] + matrix[0][1] * m.matrix[1][0] + matrix[0][2] * m.matrix[2][0];
    result.matrix[0][1] = matrix[0][0] * m.matrix[0][1] + matrix[0][1] * m.matrix[1][1] + matrix[0][2] * m.matrix[2][1];
    result.matrix[0][2] = matrix[0][0] * m.matrix[0][2] + matrix[0][1] * m.matrix[1][2] + matrix[0][2] * m.matrix[2][2];

    result.matrix[1][0] = matrix[1][0] * m.matrix[0][0] + matrix[1][1] * m.matrix[1][0] + matrix[1][2] * m.matrix[2][0];
    result.matrix[1][1] = matrix[1][0] * m.matrix[0][1] + matrix[1][1] * m.matrix[1][1] + matrix[1][2] * m.matrix[2][1];
    result.matrix[1][2] = matrix[1][0] * m.matrix[0][2] + matrix[1][1] * m.matrix[1][2] + matrix[1][2] * m.matrix[2][2];

    result.matrix[2][0] = matrix[2][0] * m.matrix[0][0] + matrix[2][1] * m.matrix[1][0] + matrix[2][2] * m.matrix[2][0];
    result.matrix[2][1] = matrix[2][0] * m.matrix[0][1] + matrix[2][1] * m.matrix[1][1] + matrix[2][2] * m.matrix[2][1];
    result.matrix[2][2] = matrix[2][0] * m.matrix[0][2] + matrix[2][1] * m.matrix[1][2] + matrix[2][2] * m.matrix[2][2];

    return result;
}

Math::TransformationMatrix& Math::TransformationMatrix::operator *= (Math::TransformationMatrix m) {
    (*this) = (*this) * m; 
    return (*this);
}

Math::vec2 Math::TransformationMatrix::operator * (vec2 v) const {
    Math::vec2 result;

    result.x = matrix[0][0] * v.x + matrix[0][1] * v.y + matrix[0][2] * 1.0;
    result.y = matrix[1][0] * v.x + matrix[1][1] * v.y + matrix[1][2] * 1.0;
    return result;
}


Math::TranslationMatrix::TranslationMatrix(ivec2 translate) : TransformationMatrix() 
{
    this->matrix[0][2] = static_cast<double>(translate.x);
    this->matrix[1][2] = static_cast<double>(translate.y);
}

Math::TranslationMatrix::TranslationMatrix(vec2 translate) : TransformationMatrix()
{
    // Reset();
    this->matrix[0][2] = translate.x;
    this->matrix[1][2] = translate.y;
}

Math::ScaleMatrix::ScaleMatrix(double scale) : TransformationMatrix()
{
    // Reset();
    matrix[0][0] = scale; matrix[0][1] = 0; matrix[0][2] = 0;
    matrix[1][0] = 0; matrix[1][1] = scale; matrix[1][2] = 0;
    matrix[2][0] = 0; matrix[2][1] = 0; matrix[2][2] = 1;
}

Math::ScaleMatrix::ScaleMatrix(vec2 scale) : TransformationMatrix()
{
    // Reset();
    matrix[0][0] = scale.x; matrix[0][1] = 0; matrix[0][2] = 0;
    matrix[1][0] = 0; matrix[1][1] = scale.y; matrix[1][2] = 0;
    matrix[2][0] = 0; matrix[2][1] = 0; matrix[2][2] = 1;
}

Math::RotationMatrix::RotationMatrix(double theta) : TransformationMatrix()
{
    matrix[0][0] = cos(theta); matrix[0][1] = -sin(theta); matrix[0][2] = 0;
    matrix[1][0] = sin(theta); matrix[1][1] = cos(theta); matrix[1][2] = 0;
    matrix[2][0] = 0; matrix[2][1] = 0; matrix[2][2] = 1;
}