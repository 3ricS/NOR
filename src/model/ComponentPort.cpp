//
// Created by erics on 003, 03, Februar.
//

#include "ComponentPort.h"

bool ComponentPort::operator==(ComponentPort otherComponentPort)
{
    bool equalComponent = (_component == otherComponentPort._component);
    bool equalPort = (_port == otherComponentPort._port);
    return equalComponent && equalPort;
}

void ComponentPort::invertPort()
{
    if(_port == Component::Port::A)
    {
        _port = Component::Port::B;
    }
    else if(_port == Component::Port::B)
    {
        _port = Component::Port::A;
    }
}

ComponentPort ComponentPort::getOppisiteComponentPort()
{
    return ComponentPort (_component, getOppisitePort());
}

Component::Port ComponentPort::getOppisitePort()
{
    if(_port == Component::Port::A)
    {
        return Component::B;
    }
    else
    {
        return Component::A;
    }
}
