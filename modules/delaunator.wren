var EPSILON = 2.pow(-52)
var EDGE_STACK = List.filled(512, 0)

class Delaunator {
    static from(points) {
        var n = points.count
        var coords = List.filled(n * 2, 0)

        for (i in 0...n) {
            var p = points[i]
            coords[2 * i] = p[0]
            coords[2 * i + 1] = p[1]
        }

        return Delaunator.new(coords)
    }

    static from(points, getX, getY) {
        var n = points.count
        var coords = List.filled(n * 2, 0)

        for (i in 0...n) {
            var p = points[i]
            coords[2 * i] = getX.call(p)
            coords[2 * i + 1] = getY.call(p)
        }

        return Delaunator.new(coords)
    }

    construct new(coords) {
        var n = coords.count >> 1
        if (n > 0 && coords[0].type != Num) Fiber.abort("Expected coords to contain numbers.")

        _coords = coords

        var maxTriangles = (2 * n - 5).max(0)
        _triangles = List.filled(maxTriangles * 3, 0)
        _halfedges = List.filled(maxTriangles * 3, 0)

        _hashSize = n.sqrt.ceil
        _hullPrev = List.filled(n, 0)
        _hullNext = List.filled(n, 0)
        _hullTri = List.filled(n, 0)
        _hullHash = List.filled(_hashSize, 0)

        _ids = List.filled(n, 0)
        _dists = List.filled(n, 0)

        update()
    }

    triangles { _triangles }
    halfedges { _halfedges }

    update() {
        var n = _coords.count >> 1

        var minX = Num.infinity
        var minY = Num.infinity
        var maxX = -Num.infinity
        var maxY = -Num.infinity

        for (i in 0...n) {
            var x = _coords[2 * i]
            var y = _coords[2 * i + 1]
            if (x < minX) minX = x
            if (y < minY) minY = y
            if (x > maxX) maxX = x
            if (y > maxY) maxY = y
            _ids[i] = i
        }
        var cx = (minX + maxX) / 2
        var cy = (minY + maxY) / 2

        var i0
        var i1
        var i2

        var minDist = Num.infinity
        for (i in 0...n) {
            var d = Delaunator.dist_(cx, cy, _coords[2 * i], _coords[2 * i + 1])
            if (d < minDist) {
                i0 = i
                minDist = d
            }
        }
        var i0x = _coords[2 * i0]
        var i0y = _coords[2 * i0 + 1]

        minDist = Num.infinity
        for (i in 0...n) {
            if (i == i0) continue
            var d = Delaunator.dist_(i0x, i0y, _coords[2 * i], _coords[2 * i + 1])
            if (d < minDist && d > 0) {
                i1 = i
                minDist = d
            }
        }
        var i1x = _coords[2 * i1]
        var i1y = _coords[2 * i1 + 1]

        var minRadius = Num.infinity

        for (i in 0...n) {
            if (i == i0 || i == i1) continue
            var r = Delaunator.circumradius_(i0x, i0y, i1x, i1y, _coords[2 * i], _coords[2 * i + 1])
            if (r < minRadius) {
                i2 = i
                minRadius = r
            }
        }
        var i2x = _coords[2 * i2]
        var i2y = _coords[2 * i2 + 1]

        if (minRadius == Num.infinity) {
            for (i in 0...n) {
                _dists[i] = (_coords[2 * i] - coords[0]) || (_coords[2 * i + 1] - coords[1])
            }

            Delaunator.quicksort_(_ids, _dists, 0, n - 1)
            var hull = List.filled(n, 0)
            var j = 0

            var d0 = -Num.infinity
            for (i in 0...n) {
                var id = _ids[i]
                var d = _dists[id]
                if (d > d0) {
                    hull[j] = id
                    j = j + 1
                    d0 = d
                }
            }
            _hull = hull[0...j]
            _triangles = List.filled(0, 0)
            _halfedges = List.filled(0, 0)
            return
        }

        if (Delaunator.orient_(i0x, i0y, i1x, i1y, i2x, i2y)) {
            var i = i1
            var x = i1x
            var y = i1y
            i1 = i2
            i1x = i2x
            i1y = i2y
            i2 = i
            i2x = x
            i2y = y
        }

        var center = Delaunator.circumcenter_(i0x, i0y, i1x, i1y, i2x, i2y)
        _cx = center["x"]
        _cy = center["y"]

        for (i in 0...n) {
            _dists[i] = Delaunator.dist_(_coords[2 * i], _coords[2 * i + 1], center["x"], center["y"])
        }

        Delaunator.quicksort_(_ids, _dists, 0, n - 1)

        _hullStart = i0
        var hullSize = 3

        _hullNext[i0] = i1
        _hullPrev[i2] = i1
        _hullNext[i1] = i2
        _hullPrev[i0] = i2
        _hullNext[i2] = i0
        _hullPrev[i1] = i0

        _hullTri[i0] = 0
        _hullTri[i1] = 1
        _hullTri[i2] = 2

        _hullHash = List.filled(_hullHash.count, -1)
        _hullHash[hashKey_(i0x, i0y)] = i0
        _hullHash[hashKey_(i1x, i1y)] = i1
        _hullHash[hashKey_(i2x, i2y)] = i2

        _trianglesLen = 0
        addTriangle_(i0, i1, i2, -1, -1, -1)

        var xp
        var yp
        for (k in 0..._ids.count) {
            var i = _ids[k]
            var x = _coords[2 * i]
            var y = _coords[2 * i + 1]

            if (k > 0 && (x - xp).abs <= EPSILON && (y - yp).abs <= EPSILON) continue
            xp = x
            yp = y

            if (i == i0 || i == i1 || i == i2) continue

            var start = 0
            var key = hashKey_(x, y)
            for (j in 0..._hashSize) {
                start = _hullHash[(key + j) % _hashSize]
                if (start != -1 && start != _hullNext[start]) break
            }

            start = _hullPrev[start]
            var e = start

            while (true) {
                var q = _hullNext[e]
                if (Delaunator.orient_(x, y, _coords[2 * e], _coords[2 * e + 1], _coords[2 * q], _coords[2 * q + 1])) break
                e = q
                if (e == start) {
                    e = -1
                    break
                }
            }
            if (e == -1) continue

            var t = addTriangle_(e, i, _hullNext[e], -1, -1, _hullTri[e])

            _hullTri[i] = legalize_(t + 2)
            _hullTri[e] = t
            hullSize = hullSize + 1

            var n = _hullNext[e]

            while (true) {
                var q = _hullNext[n]
                if (!(Delaunator.orient_(x, y, _coords[2 * n], _coords[2 * n + 1], _coords[2 * q], _coords[2 * q + 1]))) break
                t = addTriangle_(n, i, q, _hullTri[i], -1, _hullTri[n])
                _hullTri[i] = legalize_(t + 2)
                _hullNext[n] = n
                hullSize = hullSize - 1
                n = q
            }

            if (e == start) {
                while (true) {
                    var q = _hullPrev[e]
                    if (!(Delaunator.orient_(x, y, _coords[2 * q], _coords[2 * q + 1], _coords[2 * e], _coords[2 * e + 1]))) break
                    t = addTriangle_(q, i, e, -1, _hullTri[e], _hullTri[q])
                    legalize_(t + 2)
                    _hullTri[q] = t
                    _hullNext[e] = e
                    hullSize = hullSize - 1
                    e = q
                }
            }

            _hullStart = e
            _hullPrev[i] = e
            _hullNext[e] = i
            _hullPrev[n] = i
            _hullNext[i] = n

            _hullHash[hashKey_(x, y)] = i
            _hullHash[hashKey_(_coords[2 * e], _coords[2 * e + 1])] = e
        }

        _hull = List.filled(hullSize, 0)
        var e = _hullStart
        for (i in 0...hullSize) {
            _hull[i] = e
            e = _hullNext[e]
        }

        _triangles = _triangles[0..._trianglesLen]
        _halfedges = _halfedges[0..._trianglesLen]
    }

    hashKey_(x, y) {
        return (Delaunator.pseudoAngle_(x - _cx, y - _cy) * _hashSize).floor % _hashSize
    }

    legalize_(a) {
        var i = 0
        var ar = 0

        while (true) {
            var b = _halfedges[a]

            var a0 = a - a % 3
            ar = a0 + (a + 2) % 3

            if (b == -1) {
                if (i == 0) break
                i = i - 1
                a = EDGE_STACK[i]
                continue
            }

            var b0 = b - b % 3
            var al = a0 + (a + 1) % 3
            var bl = b0 + (b + 2) % 3

            var p0 = _triangles[ar]
            var pr = _triangles[a]
            var pl = _triangles[al]
            var p1 = _triangles[bl]

            var illegal = Delaunator.inCircle_(
                _coords[2 * p0], _coords[2 * p0 + 1],
                _coords[2 * pr], _coords[2 * pr + 1],
                _coords[2 * pl], _coords[2 * pl + 1],
                _coords[2 * p1], _coords[2 * p1 + 1])

            if (illegal) {
                _triangles[a] = p1
                _triangles[b] = p0

                var hbl = _halfedges[bl]

                if (hbl == -1) {
                    var e = _hullStart

                    if (_hullTri[e] == bl) {
                        _hullTri[e] = a
                        break
                    }
                    e = _hullPrev[e]

                    while (e != _hullStart) {
                        if (_hullTri[e] == bl) {
                            _hullTri[e] = a
                            break
                        }
                        e = _hullPrev[e]
                    }
                }
                link_(a, hbl)
                link_(b, _halfedges[ar])
                link_(ar, bl)

                var br = b0 + (b + 1) % 3

                if (i < EDGE_STACK.count) {
                    EDGE_STACK[i] = br
                    i = i + 1
                }
            } else {
                if (i == 0) break
                i = i - 1
                a = EDGE_STACK[i]
            }
        }

        return ar
    }

    link_(a, b) {
        _halfedges[a] = b
        if (b != -1) _halfedges[b] = a
    }

    addTriangle_(i0, i1, i2, a, b, c) {
        var t = _trianglesLen

        _triangles[t] = i0
        _triangles[t + 1] = i1
        _triangles[t + 2] = i2

        link_(t, a)
        link_(t + 1, b)
        link_(t + 2, c)

        _trianglesLen = _trianglesLen + 3

        return t
    }

    static pseudoAngle_(dx, dy) {
        var p = dx / (dx.abs + dy.abs)
        if (dy > 0) return (3 - p) / 4
        return (1 + p) / 4
    }

    static dist_(ax, ay, bx, by) {
        var dx = ax - bx
        var dy = ay - by
        return dx * dx + dy * dy
    }

    static orientIfSure_(px, py, rx, ry, qx, qy) {
        var l = (ry - py) * (qx - px)
        var r = (rx - px) * (qy - py)

        if ((l - r).abs >= 3.3306690738754716e-16 * (l + r).abs) {
            return l - r
        } else {
            return 0
        }
    }

    static orient_(rx, ry, qx, qy, px, py) {
        return (orientIfSure_(px, py, rx, ry, qx, qy) ||
            orientIfSure_(rx, ry, qx, qy, px, py) ||
            orientIfSure_(qx, qy, px, py, rx, ry)) < 0
    }

    static inCircle_(ax, ay, bx, by, cx, cy, px, py) {
        var dx = ax - px
        var dy = ay - py
        var ex = bx - px
        var ey = by - py
        var fx = cx - px
        var fy = cy - py

        var ap = dx * dx + dy * dy
        var bp = ex * ex + ey * ey
        var cp = fx * fx + fy * fy

        return dx * (ey * cp - bp * fy) -
            dy * (ex * cp - bp * fx) +
            ap * (ex * fy - ey * fx) < 0
    }

    static circumradius_(ax, ay, bx, by, cx, cy) {
        var dx = bx - ax
        var dy = by - ay
        var ex = cx - ax
        var ey = cy - ay

        var bl = dx * dx + dy * dy
        var cl = ex * ex + ey * ey
        var d = 0.5 / (dx * ey - dy * ex)

        var x = (ey * bl - dy * cl) * d
        var y = (dx * cl - ex * bl) * d

        return x * x + y * y
    }

    static circumcenter_(ax, ay, bx, by, cx, cy) {
        var dx = bx - ax
        var dy = by - ay
        var ex = cx - ax
        var ey = cy - ay

        var bl = dx * dx + dy * dy
        var cl = ex * ex + ey * ey
        var d = 0.5 / (dx * ey - dy * ex)

        var x = ax + (ey * bl - dy * cl) * d
        var y = ay + (dx * cl - ex * bl) * d

        return {
            "x": x,
            "y": y
        }
    }

    static quicksort_(ids, dists, left, right) {
        if (right - left <= 20) {
            for (i in left+1..right) {
                var temp = ids[i]
                var tempDist = dists[temp]
                var j = i - 1
                while (j >= left && dists[ids[j]] > tempDist) {
                    ids[j + 1] = ids[j]
                    j = j - 1
                }
                ids[j + 1] = temp
            }
        } else {
            var median = (left + right) >> 1
            var i = left + 1
            var j = right
            swap_(ids, median, i)
            if (dists[ids[left]] > dists[ids[right]]) swap_(ids, left, right)
            if (dists[ids[i]] > dists[ids[right]]) swap_(ids, i, right)
            if (dists[ids[left]] > dists[ids[i]]) swap_(ids, left, i)

            var temp = ids[i]
            var tempDist = dists[temp]
            while (true) {
                i = i + 1
                while (dists[ids[i]] < tempDist) i = i + 1
                j = j - 1
                while (dists[ids[j]] > tempDist) j = j - 1
                if (j < i) break
                swap_(ids, i, j)
            }
            ids[left + 1] = ids[j]
            ids[j] = temp

            if (right - i + 1 >= j - left) {
                quicksort_(ids, dists, i, right)
                quicksort_(ids, dists, left, j - 1)
            } else {
                quicksort_(ids, dists, left, j - 1)
                quicksort_(ids, dists, i, right)
            }
        }
    }

    static swap_(arr, i, j) {
        var tmp = arr[i]
        arr[i] = arr[j]
        arr[j] = tmp
    }
}
