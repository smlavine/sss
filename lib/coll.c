#include "dragon.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>

CollPen collBmpRect(const Bmp b, CollRect r) {
    if (r.x < 0 || r.y < 0 || r.x >= b.w - 1 || r.y >= b.h - 1) {
        return (CollPen){false,0,0,0,0};
    }
    // TODO: assert(r.w == 1)
    // TODO: assert(r.h == 1)
    int southWest = bmpGet(&b, (uint32_t)r.x,     (uint32_t)r.y,     0);
    int southEast = bmpGet(&b, (uint32_t)r.x + 1, (uint32_t)r.y,     0);
    int northWest = bmpGet(&b, (uint32_t)r.x,     (uint32_t)r.y + 1, 0);
    int northEast = bmpGet(&b, (uint32_t)r.x + 1, (uint32_t)r.y + 1, 0);

    if (r.x + r.w <= (size_t)r.x + 1) {
        southEast = northEast = false;
    }
    if (r.y + r.h <= (size_t)r.y + 1) {
        northWest = northEast = false;
    }

    int n = (southWest<<3) + (southEast<<2) + (northWest<<1) + (northEast<<0);

    float penSouth = (size_t)r.y + 1 - r.y;
    float penNorth = r.h - penSouth;
    float penWest = (size_t)r.x + 1 - r.x;
    float penEast = r.w - penWest;

    float penSouthWestSq = penSouth * penSouth + penWest * penWest;
    float penSouthEastSq = penSouth * penSouth + penEast * penEast;
    float penNorthWestSq = penNorth * penNorth + penWest * penWest;
    float penNorthEastSq = penNorth * penNorth + penEast * penEast;

    bool south = false, north = false, west = false, east = false;
    switch (n) {
        case 15: // 1111
            south = north = west = east = true;
            break; // full
        case  0: // 0000
            break; // none
        case 14: // 1110
            south = west = true;
            break; // corner south west
        case 13: // 1101
            south = east = true;
            break; // corner south east
        case 11: // 1011
            north = west = true;
            break; // corner north west
        case  7: // 0111
            north = east = true;
            break; // corner north east
        case 12: // 1100
            south = true;
            break; // south
        case  3: // 0011
            north = true;
            break; // north
        case 10: // 1010
            west = true;
            break; // west
        case  5: // 0101
            east = true;
            break; // east
        case  8: // 1000
            if (penSouth <= penWest) {
                south = true;
            } else {
                west = true;
            }
            break; // south west
        case  4: // 0100
            if (penSouth <= penEast) {
                south = true;
            } else {
                east = true;
            }
            break; // south east
        case  2: // 0010
            if (penNorth <= penWest) {
                north = true;
            } else {
                west = true;
            }
            break; // north east
        case  1: // 0001
            if (penNorth <= penEast) {
                north = true;
            } else {
                east = true;
            }
            break; // north west
        case  9: // 1001
            if (penSouthEastSq <= penNorthWestSq) {
                south = east = true;
            } else {
                north = west = true;
            }
            break; // diagonal south west north east
        case  6: // 0110
            if (penSouthWestSq <= penNorthEastSq) {
                south = west = true;
            } else {
                north = east = true;
            }
            break; // diagonal south east north west
    }

    CollPen p;
    p.is = south | north | west | east;
    p.south = south ? penSouth : 0;
    p.north = north ? penNorth : 0;
    p.west = west ? penWest : 0;
    p.east = east ? penEast : 0;

    return p;
}

CollPen collRect(CollRect a, CollRect b) {
    if (a.x + a.w <= b.x || a.x >= b.x + b.w
     || a.y + a.h <= b.y || a.y >= b.y + b.h
     || a.w <= 0 || a.h <= 0 || b.w <= 0 || b.h <= 0) {
        return (CollPen){false, 0, 0, 0, 0};
    } else {
        CollPen p = {true, 0, 0, 0, 0};
        if (a.x < b.x) {
            float penEast = a.x + a.w - b.x;
            if (a.y < b.y) {
                float penNorth = a.y + a.h - b.y;
                if (penEast < penNorth) {
                    p.east = penEast;
                } else {
                    p.north = penNorth;
                }
            } else {
                float penSouth = b.y + b.h - a.y;
                if (penEast < penSouth) {
                    p.east = penEast;
                } else {
                    p.south = penSouth;
                }
            }
        } else {
            float penWest = b.x + b.w - a.x;
            if (a.y < b.y) {
                float penNorth = a.y + a.h - b.y;
                if (penWest < penNorth) {
                    p.west = penWest;
                } else {
                    p.north = penNorth;
                }
            } else {
                float penSouth = b.y + b.h - a.y;
                if (penWest < penSouth) {
                    p.west = penWest;
                } else {
                    p.south = penSouth;
                }
            }
        }
        return p;
    }
}
