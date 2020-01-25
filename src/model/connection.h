#ifndef CONNECTION_H
#define CONNECTION_H

#include <model/component.h>

class Connection : public Component
{
public:
    Connection(int xStart, int yStart, int xEnd, int yEnd);

    // Component interface
    virtual int getXStartPosition() override;
    virtual int getYStartPosition() override;


signals:
    void draw();
    void remove();

private:


};

#endif // CONNECTION_H
