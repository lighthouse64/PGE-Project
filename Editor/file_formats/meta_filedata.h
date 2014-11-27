#ifndef META_FILEDATA_H
#define META_FILEDATA_H

#include <QtCore>

struct Bookmark{
    QString bookmarkName;
    double x;
    double y;
};


struct MetaData{
    QVector<Bookmark> bookmarks;
};

#endif // META_FILEDATA_H