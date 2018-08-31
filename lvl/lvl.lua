#!/usr/bin/env lua5.3


local PIXEL = {}
PIXEL.HERO = "255   0   0"
PIXEL.NONE = "255 255 255"
PIXEL.COIN = "255 255   0"
PIXEL.WALL = "  0   0   0"


local function copyBmp(b)
    local bmp = {["w"] = b.w, ["h"] = b.h}
    for x = 1, bmp.w do
        bmp[x] = {}
        for y = 1, bmp.h do
            bmp[x][y] = b[x][y]
        end
    end
    return bmp
end


local function expandRectOnBmp(b, x, y)
    local r = {["x"] = x, ["y"] = y, ["w"] = 0, ["h"] = 0}
    while r.x + r.w <= b.w and b[r.x + r.w][r.y] do
        r.w = r.w + 1
    end
    while r.y + r.h <= b.h do
        local lineOK = true
        for X = r.x, r.x + r.w - 1 do
            if not b[X][r.y + r.h] then
                lineOK = false
                break
            end
        end
        if lineOK then
            r.h = r.h + 1
        else
            break
        end
    end
    return r
end


local function deleteRectOnBmp(b, r)
    for x = r.x, r.x + r.w - 1 do
        for y = r.y, r.y + r.h - 1 do
            b[x][y] = false
        end
    end
end


local function countRectAreaOnBmp(b, r)
    local n = 0
    for x = r.x, r.x + r.w - 1 do
        for y = r.y, r.y + r.h - 1 do
            if b[x][y] then
                n = n + 1
            end
        end
    end
    return n
end


local function bmpRects(b)
    local rects = {}
    local b2 = copyBmp(b)
    for y = 1, b2.h do
        for x = 1, b2.w do
            if b2[x][y] then
                local r = expandRectOnBmp(b, x, y)
                local r2 = expandRectOnBmp(b2, x, y)
                if countRectAreaOnBmp(b2, r) > countRectAreaOnBmp(b2, r2) then
                    deleteRectOnBmp(b2, r)
                    rects[#rects + 1] = r
                else
                    deleteRectOnBmp(b2, r2)
                    rects[#rects + 1] = r2
                end
            end
        end
    end
    return rects
end


local function parsePlainPPM(s)
    s = s:gsub("#[^\n]*", " ") -- remove comments

    local header, w, h = s:match("(P3%s+(%d+)%s+(%d+)%s+%d+%s*)")
    s = s:sub(#header + 1) -- remove header

    local img = {["w"] = tonumber(w), ["h"] = tonumber(h), ["p"] = {}}
    for x = 1, img.w do
        img[x] = {}
    end

    local x, y = 1, img.h
    for r, g, b in s:gmatch(("(%d+)%s*"):rep(3)) do
        img[x][y] = ("%3s %3s %3s"):format(r, g, b)
        img.p[img[x][y]] = true
        x = x + 1
        if x > img.w then
            x = 1
            y = y - 1
        end
    end

    return img
end


local function pixelBmp(img, pixels)
    local b = {["w"] = img.w, ["h"] = img.h}
    for x = 1, b.w do
        b[x] = {}
        for y = 1, b.h do
            b[x][y] = pixels[img[x][y]] == true
        end
    end
    return b
end


local function findPixelRects(img, pixels)
    for k, _ in pairs(pixels) do
        if img.p[k] then
            return bmpRects(pixelBmp(img, pixels))
        end
    end
    return {}
end


local function bmpStr(b)
    local s = {}
    for y = 1, b.h do
        s[y] = {}
        for x = 1, b.w do
            s[y][x] = b[x][b.h - y + 1] and "1" or "0"
        end
        s[y] = table.concat(s[y], " ")
    end
    return table.concat(s, "\n")
end


local function rectStr(r)
    return ("%2d %2d %2d %2d"):format(r.x - 1, r.y - 1, r.w, r.h)
end


local function rectArrStr(rects)
    local s = {tostring(#rects)}
    for _, r in ipairs(rects) do
        s[#s + 1] = rectStr(r)
    end
    return table.concat(s, "\n")
end


local function main()
    local img = parsePlainPPM(io.read("a"))
    print("255 255 255 255\n") -- background color
    print("255   0   0 255\n") -- hero color
    print("  0   0   0 255\n") -- wall color
    print("255 255   0 255\n") -- coin color
    print(("%d %d\n"):format(img.w, img.h))
    print(rectStr(findPixelRects(img, {[PIXEL.HERO] = true})[1]) .. "\n")
    print(rectArrStr(findPixelRects(img, {[PIXEL.WALL] = true})) .. "\n")
    print(rectArrStr(findPixelRects(img, {[PIXEL.COIN] = true})) .. "\n")
end


main()
