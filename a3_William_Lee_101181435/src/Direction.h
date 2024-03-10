#ifndef DIRECTION_H
#define DIRECTION_H

#include <QHash>

enum class Direction { UP, DOWN };

// Hash function needed to use as key in Qt map containers
inline uint qHash(Direction key, uint seed) {
    return ::qHash(static_cast<uint>(key), seed);
}

#endif /* DIRECTION_H */
