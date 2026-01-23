#ifndef ICON_GENERATOR_H
#define ICON_GENERATOR_H

#include <QColor>
#include <QIcon>
#include <QSize>
#include <QVector>

namespace iut {

QIcon createIcon(int r, int g, int b, QSize size = QSize(100, 100));

QVector<QColor> generateOrangeShades(int N);

}  // namespace iut

#endif  // ICON_GENERATOR_H
