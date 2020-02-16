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

/*!
 * \brief   Tauscht den Port des ComponentPorts
 *
 * Zeigt der ComponentPort auf Port A, so zeigt er anschließend auf Port B und umgekehrt.
 */
void ComponentPort::invertPort()
{
    if (_port == Component::Port::A)
    {
        _port = Component::Port::B;
    }
    else if (_port == Component::Port::B)
    {
        _port = Component::Port::A;
    }
}

/*!
 * \brief   Erzeugt ComponentPort des mit invertiertem Port
 * @return  Gibt einen ComponentPort mit gleichem Zeiger auf einen Component, aber dem invertiertem Port zurück.
 */
ComponentPort ComponentPort::getOppisiteComponentPort()
{
    return ComponentPort(_component, getOppisitePort());
}

Component::Port ComponentPort::getOppisitePort()
{
    if (_port == Component::Port::A)
    {
        return Component::B;
    }
    else
    {
        return Component::A;
    }
}
