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

    void invertPort();

    //getter
    Component* getComponent(void) const {return _component;}
    Component::Port getPort(void) const {return _port;}
    ComponentPort getOppisiteComponentPort(void);

private:
    Component* _component;
    Component::Port _port;
    Component::Port getOppisitePort(void);
};


#endif //NOR_COMPONENTPORT_H
