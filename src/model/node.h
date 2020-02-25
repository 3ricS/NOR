//
// Created by erics on 024, 24, Februar.
//

#ifndef NOR_NODE_H
#define NOR_NODE_H

#include <model/componentport.h>

class Node
{
public:
    Node(int id, QList<ComponentPort> componentPorts) : _id(id), _componentPorts(componentPorts)
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

    int getId(void) const
    { return _id; }

    QList<ComponentPort> getComponentPorts()
    { return _componentPorts; }

    int getComponentPortCount()
    { return _componentPorts.count(); }

private:
    int _id;
    QList<ComponentPort> _componentPorts;
};


#endif //NOR_NODE_H
