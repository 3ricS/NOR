//
// Created by erics on 003, 03, Februar.
//

#ifndef NOR_COMPONENTPORT_H
#define NOR_COMPONENTPORT_H

#include "model/Component.h"

class ComponentPort
{
public:
    ComponentPort(Component* component, Component::Port port) : _component(component), _port(port) {}

    bool operator==(ComponentPort otherComponentPort);

    //getter
    Component* getComponent(void) {return _component;}
    Component::Port getPort(void) {return _port;}

private:
    Component* _component;
    Component::Port _port;
};


#endif //NOR_COMPONENTPORT_H
