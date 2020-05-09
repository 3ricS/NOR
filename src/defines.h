/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   In Defines sind die Globalenvariablen hinterlegt.
 *
 * Die Werte für die Gitterlänge sind im ganzen Programm einheitlich und in dieser Klasse als Globalevariablen hinterlegt.
 */
#include <QtGui/QColor>

#ifndef DEFINES_H
#define DEFINES_H

#endif // DEFINES_H


class Defines
{
public:
    static constexpr int gridLength = 120;
    static constexpr int defaultSceneSize = 6000;
    static const QColor shapeColor;
    static const QColor hightlightColor;
};

