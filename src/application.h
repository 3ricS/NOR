#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class NetworkGraphics;
class MainWindow;

class Application : public QApplication
{
public:
    Application(int &argc, char **argv);

    bool event(QEvent *event) override;

private:
    NetworkGraphics* _model = nullptr;
    MainWindow* _window = nullptr;
};

#endif // APPLICATION_H
