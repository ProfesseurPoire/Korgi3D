#pragma once
#include <stdio.h>

class Component
{
public:

    Component()=default;

    Component(int a):
        number(a)
    {}

    Component(int a, int b){}

    ~Component() = default;

    int number{3};
};