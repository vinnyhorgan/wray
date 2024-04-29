// Simple 2d vector class

class Vector {
    construct new(x, y) {
        _x = x
        _y = y
    }

    x { _x }
    y { _y }
    x=(v) { _x = v }
    y=(v) { _y = v }

    - { Vector.new(-_x, -_y) }
    ==(other) { _x == other.x && _y == other.y }
    !=(other) { _x != other.x || _y != other.y }
    +(other) { Vector.new(_x + other.x, _y + other.y) }
    -(other) { Vector.new(_x - other.x, _y - other.y) }

    *(other) {
        if (other is Vector) {
            return Vector.new(_x * other.x, _y * other.y)
        } else {
            return Vector.new(_x * other, _y * other)
        }
    }

    /(other) {
        if (other is Vector) {
            return Vector.new(_x / other.x, _y / other.y)
        } else {
            return Vector.new(_x / other, _y / other)
        }
    }

    length { (_x.pow(2) + _y.pow(2)).sqrt }

    unit {
        if (length == 0) {
            return Vector.new(0, 0)
        }

        return Vector.new(_x / length, _y / length)
    }

    toString { "Vector(%(_x), %(_y))" }
}
