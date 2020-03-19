#include "ComponentPort.h"

/*!
 * \brief   Tauscht den Port des ComponentPorts.
 *
 * Zeigt der ComponentPort auf Port A, so zeigt er anschließend auf Port B und umgekehrt.
 */
void ComponentPort::invertPort(void)
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
 * \brief   Erzeugt ComponentPort mit invertiertem Port.
 *
 * \return  Gibt einen ComponentPort mit gleichem Zeiger auf einen Component, aber dem invertiertem Port zurück.
 */
ComponentPort ComponentPort::getOppisiteComponentPort(void)
{
    return ComponentPort(_component, getOppisitePort());
}

Component::Port ComponentPort::getOppisitePort(void)
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

bool ComponentPort::operator==(const ComponentPort& rhs)
{
    bool equalComponent = (_component == rhs._component);
    bool equalPort = (_port == rhs._port);
    return equalComponent && equalPort;
}

bool ComponentPort::operator!=(const ComponentPort& rhs)
{
    return !(operator==(rhs));
}
