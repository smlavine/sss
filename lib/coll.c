#include "dragon.h"

CollPen collBmpRect(const Bmp b, CollRect r) {
    if (r.x < 0 || r.y < 0 || r.x + r.w >= b.w || r.y + r.h >= b.h) {
        return (CollPen){false, 0, 0, 0, 0};
    }

    int x = r.x;
    int y = r.y;
    int w = r.x + r.w - (int)r.x;
    int h = r.y + r.h - (int)r.y;

    int southWest = bmpGet(b, x,     y);
    int southEast = bmpGet(b, x + w, y);
    int northWest = bmpGet(b, x,     y + h);
    int northEast = bmpGet(b, x + w, y + h);

    bool south, north, west, east;
    south = north = west = east = false;

    for (int xi = x + 1; xi < x + w; ++xi) {
        south |= bmpGet(b, xi, y);
        north |= bmpGet(b, xi, y + h);
    }

    for (int yi = y + 1; yi < y + h; ++yi) {
        west |= bmpGet(b, x,     yi);
        east |= bmpGet(b, x + w, yi);
    }

    for (int xi = x + 1; xi < x + w; ++xi) {
        for (int yi = y + 1; yi < y + h; ++yi) {
            if (bmpGet(b, xi, yi)) {
                south = north = west = east = true;
            }
        }
    }

    float penSouth = (size_t)r.y + 1 - r.y;
    float penNorth = r.y + r.h - (size_t)(r.y + r.h);
    float penWest = (size_t)r.x + 1 - r.x;
    float penEast = r.x + r.w - (size_t)(r.x + r.w);

    float penSouthWestSq = penSouth * penSouth + penWest * penWest;
    float penSouthEastSq = penSouth * penSouth + penEast * penEast;
    float penNorthWestSq = penNorth * penNorth + penWest * penWest;
    float penNorthEastSq = penNorth * penNorth + penEast * penEast;

    int n = (southWest << 3) + (southEast << 2) + (northWest << 1) + northEast;
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
            if (penSouth <= penWest && !west) {
                south = true;
            } else if (!south) {
                west = true;
            }
            break; // south west
        case  4: // 0100
            if (penSouth <= penEast && !east) {
                south = true;
            } else if (!south) {
                east = true;
            }
            break; // south east
        case  2: // 0010
            if (penNorth <= penWest && !west) {
                north = true;
            } else if (!north) {
                west = true;
            }
            break; // north east
        case  1: // 0001
            if (penNorth <= penEast && !east) {
                north = true;
            } else if (!north) {
                east = true;
            }
            break; // north west
        case  9: // 1001
            if (south || east) {
                south = east = true;
            } else if (north || west) {
                north = west = true;
            } else if (penSouthEastSq <= penNorthWestSq) {
                south = east = true;
            } else {
                north = west = true;
            }
            break; // diagonal south west north east
        case  6: // 0110
            if (south || west) {
                south = west = true;
            } else if (north || east) {
                north = east = true;
            } else if (penSouthWestSq <= penNorthEastSq) {
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
    if (a.x+a.w <= b.x || a.x >= b.x+b.w || a.y+a.h <= b.y || a.y >= b.y+b.h){
        return (CollPen){false, 0, 0, 0, 0};
    } else {
        float penSouth = b.y + b.h - a.y;
        float penNorth = a.y + a.h - b.y;
        float penWest = b.x + b.w - a.x;
        float penEast = a.x + a.w - b.x;
        CollPen p = {true, 0, 0, 0, 0};
        if (a.x < b.x) {
            if (a.y < b.y) {
                if (penEast < penNorth) {
                    p.east = penEast;
                } else {
                    p.north = penNorth;
                }
            } else {
                if (penEast < penSouth) {
                    p.east = penEast;
                } else {
                    p.south = penSouth;
                }
            }
        } else {
            if (a.y < b.y) {
                if (penWest < penNorth) {
                    p.west = penWest;
                } else {
                    p.north = penNorth;
                }
            } else {
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
