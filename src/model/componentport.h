/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Ein ComponentPort zeigt auf einen Component und gibt dabei gleichzeitig einen Port an
 *
 * Mithilfe eines ComponentPorts kann auf einen bestimmten Port bei einem bestimmten Component gezeigt werden.
 * Will man genau spezifizieren, mit welchem Ende der Komponente eine Verbindung verbunden ist, ist ein ComponentPort nötig.
 * Hiermit kann man den Port und den Component zu einem Element zusammenfassen.
 */

#ifndef NOR_COMPONENTPORT_H
#define NOR_COMPONENTPORT_H

#include "model/component.h"

class ComponentPort
{
public:
    ComponentPort(Component* component, Component::Port port) : _component(component), _port(port) {}

    bool operator==(const ComponentPort& rhs);
    bool operator!=(const ComponentPort& rhs);

    void invertPort(void);

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
