/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Eine Node ist Knoten, an dem Connections abzweigen.
 *
 * Ein Node besitzt eine Id, eine Liste von ComponentPorts und ob der Knoten mit der Spannungsquelle verbunden ist.
 */
#ifndef NOR_NODE_H
#define NOR_NODE_H

#include <model/componentport.h>

class Node
{
public:
    Node(int id, QList<ComponentPort> componentPorts, bool isConnectedToPowerSupply) :
        _id(id), _componentPorts(componentPorts), _isConnectedToPowerSupply(isConnectedToPowerSupply)
    {
    }

    bool operator==(const Node& rhs)
    {
        return _id == rhs._id;
    }

    bool operator!=(const Node& rhs)
    {
        return !(operator==(rhs));
    }

    bool isConnectedToPowerSupply(void) {return _isConnectedToPowerSupply;}

    int getId(void) const
    { return _id; }

    QList<ComponentPort> getComponentPorts(void)
    { return _componentPorts; }


private:
    int                  _id;
    QList<ComponentPort> _componentPorts;
    bool                 _isConnectedToPowerSupply = false;
};


#endif //NOR_NODE_H
