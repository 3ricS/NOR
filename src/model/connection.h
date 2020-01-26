#ifndef CONNECTION_H
#define CONNECTION_H

#include <model/component.h>

class Connection : public Component
{
public:
    Connection(int xStart, int yStart, int xEnd, int yEnd);

    // Component interface
    virtual int getXStartPosition() override {return _xStart;}
    virtual int getYStartPosition() override {return _yStart;}
    int getXEndPosition() {return  _xEnd;}
    int getYEndPosition() {return _yEnd;}

signals:
    void draw();
    void remove();

private:
    int _xStart;
    int _yStart;
    int _xEnd;
    int _yEnd;

};

#endif // CONNECTION_H
