#include "application.h"
#include "model/networkgraphics.h"
#include "view/mainwindow.h"

#include <QFileOpenEvent>
#include <QDebug>

Application::Application(int &argc, char **argv) : QApplication(argc, argv)
{
    _model = new NetworkGraphics();
    _window = new MainWindow(_model);
    _window->show();

    QSysInfo s;
    if(s.productType() == "windows")
    {
        setWindowIcon(QIcon(":/img/resistorLogo.png"));
    }
    else if(s.productType() == "osx")
    {
        setWindowIcon(QIcon(":/img/resistorLogo.icns"));
    }

    if (arguments().size() > 1)
    {
        const QString fileName = arguments().at(1);
        _model->loadFromFile(fileName);
    }
}


bool Application::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
        _model->loadFromFile(openEvent->file());
    }

    return QApplication::event(event);
}
