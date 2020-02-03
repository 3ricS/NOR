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