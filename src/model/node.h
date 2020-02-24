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

    int getId(void) const {return _id;}
    QList<ComponentPort> getComponentPorts() {return _componentPorts;}

private:
    int _id;
    QList<ComponentPort> _componentPorts;
};


#endif //NOR_NODE_H
