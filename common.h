#ifndef COMMON_H
#define COMMON_H
#include <QObject>
struct LcdDevice{
    qint32 resx, resy, perlbits;
    QString devname;
};
#endif // COMMON_H
