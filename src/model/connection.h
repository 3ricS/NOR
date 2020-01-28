#ifndef CONNECTION_H
#define CONNECTION_H

#include <model/component.h>

class Connection : public Component
{
public:
    Connection(int xStart, int yStart, int xEnd, int yEnd);

    // Component interface
    int getXEndPosition() {return  _xEnd;}
    int getYEndPosition() {return _yEnd;}

    int getXStartPosition() override;

    int getYStartPosition() override;

private:
    int _xStart;
    int _yStart;
    int _xEnd;
    int _yEnd;

};

#endif // CONNECTION_H
