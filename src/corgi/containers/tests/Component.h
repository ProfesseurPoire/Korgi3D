#pragma once

class Component
{
public:

    Component()=default;

    Component(int a):
        number(a)
    {}

    int number{3};
};