#ifndef MATHS_H
#define MATHS_H

#include <stdio.h>
#include <cmath>
#include <vector>
#include <queue>
#include <numeric>
#include <cstdlib>

#undef min
#undef max

namespace Math {

const float PI = 3.14159265358979f;
const float TWO_PI = 6.283185307f;

static float max(float a, float b) {
	return (a < b) ? b : a;
}

static float min(float a, float b) {
	return (a < b) ? a : b;
}

static float limit(float value, float min, float max) {
	if (value < min) {
		return min;
	} else if (value > max) {
		return max;
	} else {
		return value;
	}
}

static float limit(float value, float maxMin) {
	return limit(value, -maxMin, maxMin);
}

static float abs(float num) {
    return num >= 0 ? num : -num;
}

static float sign(float num) {
    return num >= 0 ? 1.0f : -1.0f;
}

static float round(float r, int places = 1) {
    float off = (float)std::pow(10, places);

    return (float)((int)(r * off) / off);
}

static float degToRad(float degrees) {
    return degrees * Math::PI / 180.0f;
}

static float radToDeg(float radians) {
    return radians * 180.0f / Math::PI;
}

static float sin(float a) {
    return ::sin(a);
}

static float cos(float a) {
    return ::cos(a);
}

static float asin(float a) {
    return ::asin(a);
}

static float acos(float a) {
    return ::acos(a);
}

static float tan(float a) {
    return ::tan(a);
}

static float atan(float a) {
    return ::atan(a);
}

static float exp(float a) {
    return ::exp(a);
}

static float pow(float a, float b) {
    return ::pow(a, b);
}

static float sqrt(float a) {
    return ::sqrt(a);
}

static float map(float value, float inMin, float inMax, float outMin, float outMax) {
	if (value < inMin) {
		return outMin;
	} else if (value > inMax) {
		return outMax;
	}

	return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;

	/*float leftSpan = inMax - inMin;
    float rightSpan = outMax - outMin;
    // Convert the left range into a 0-1 range (float)
    float valueScaled = (value - inMin) / leftSpan;
    // Convert the 0-1 range into a value in the right range.
    float result = outMin + (valueScaled * rightSpan);
	if (result < outMin) {
		result = outMin;
	} else if (result > outMax) {
		result = outMax;
	}
	return result;*/
}

static float floatModulus(float a, float b) {
    return ::fmod(a, b);
}

static float distanceBetween(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

static float standardDeviation(std::vector<float> elements, float& mean) {
	float sum = (float)std::accumulate(elements.begin(), elements.end(), 0.0);
	mean = sum / elements.size();
	float sqSum = (float)std::inner_product(elements.begin(), elements.end(), elements.begin(), 0.0);

	return std::sqrt(sqSum / elements.size() - mean * mean);
}

static float getAngleDir(float from, float to) {
    float dir = from - to;

	if (dir > 0 && abs(dir) <= Math::PI) {
	    return -1;
	} else if (dir > 0 && abs(dir) > Math::PI) {
	    return 1;
	} else if (dir < 0 && abs(dir) <= Math::PI) {
	    return 1;
	} else {
	    return -1;
	}
}

static float getAngleAvg(float a, float b) {
	float x = floatModulus(abs(a - b), Math::TWO_PI);

	if (x >= 0 && x <= Math::PI) {
		return floatModulus((a + b) / 2, Math::TWO_PI);
	} else if (x > Math::PI && x < Math::PI * 6.0 / 4.0) {
		return floatModulus((a + b) / 2, Math::TWO_PI) + Math::PI;
	} else {
		return floatModulus((a + b) / 2, Math::TWO_PI) - Math::PI;
	}
};

static float getAngleDiff(float source, float target) {
    float diff = target - source;

    diff += (diff > Math::PI) ? -Math::TWO_PI : (diff < -Math::PI) ? Math::TWO_PI : 0;

    return diff;
}

static int randomInt(int min = 0, int max = 100) {
    return min + (rand() % (int)(max - min + 1));
}

static float randomFloat(float min = 0.0f, float max = 1.0f) {
    float r = (float)rand() / (float)RAND_MAX;

    return min + r * (max - min);
}

static float randomGaussian(float deviation = 0.5f, float mean = 0.0f) {
    return ((Math::randomFloat() * 2.0f - 1.0f) + (Math::randomFloat() * 2.0f - 1.0f) + (Math::randomFloat() * 2.0f - 1.0f)) * deviation + mean;
}

static float getGaussian(float mu, float sigma, float x) {
    return Math::exp(-Math::pow(mu - x,  2.0f) / Math::pow(sigma, 2.0f) / 2.0f) / Math::sqrt(2.0f * Math::PI * Math::pow(sigma, 2.0f));
}

class Matrix3x1;

class Matrix3x3 {
    public:
        Matrix3x3();
        Matrix3x3(
            float a11, float a12, float a13,
            float a21, float a22, float a23,
            float a31, float a32, float a33
        );

        float getDeterminant() const;
        const Matrix3x3 getMultiplied(float scalar) const;
        const Matrix3x3 getMultiplied(const Matrix3x3& b) const;
        const Matrix3x1 getMultiplied(const Matrix3x1& b) const;
        const Matrix3x3 getInversed() const;

        float a11; float a12; float a13;
        float a21; float a22; float a23;
        float a31; float a32; float a33;
};

class Matrix3x1 {
    public:
        Matrix3x1(
            float a11,
            float a21,
            float a31
        );

        const Matrix3x1 getMultiplied(float scalar) const;

        float a11;
        float a21;
        float a31;
};

class Matrix4x1;

class Matrix4x3 {
    public:
        Matrix4x3();
        Matrix4x3(
            float a11, float a12, float a13,
            float a21, float a22, float a23,
            float a31, float a32, float a33,
            float a41, float a42, float a43
        );

        const Matrix4x3 getMultiplied(float scalar) const;
        const Matrix4x1 getMultiplied(const Matrix3x1& b) const;

        float a11; float a12; float a13;
        float a21; float a22; float a23;
        float a31; float a32; float a33;
        float a41; float a42; float a43;
};

class Matrix4x1 {
    public:
        Matrix4x1(
            float a11,
            float a21,
            float a31,
            float a41
        );

        float a11;
        float a21;
        float a31;
        float a41;
};

class Vector {
    public:
        Vector() : x(0), y(0) {}
        Vector(float x, float y) : x(x), y(y) {}

        float getLength() const;
		float distanceTo(const Vector& b) const;
        float dotProduct(const Vector& b) const;
        float getAngleBetween(const Vector& b) const;
        Vector getRotated(float angle) const;
        Vector getNormalized() const;
        Vector getScaled(float magnitude) const;
        Vector getSummed(const Vector& b) const;

        static Vector createForwardVec(float dir, float magnitude = 1.0f);
        static Vector createDirVec(const Vector& from, const Vector& to);

        float x;
        float y;
};

struct Position : public Math::Vector {
    inline Position(float x = 0.0f, float y = 0.0f, float orientation = 0.0f) : Vector(x, y), orientation(orientation) {}

    float orientation;
};

typedef std::queue<Math::Position> PositionQueue;

class Angle {
    public:
        virtual float deg() const = 0;
        virtual float rad() const = 0;
};

class Deg : public Angle {
    public:
        Deg(float degrees) : degrees(degrees) {}

        inline float deg() const { return degrees; }
        inline float rad() const { return Math::degToRad(degrees); }

    private:
        float degrees;
};

class Rad : public Angle {
    public:
        Rad(float radians) : radians(radians) {}

        inline float deg() const { return Math::radToDeg(radians); }
        inline float rad() const { return radians; }

    private:
        float radians;
};

struct Point {
    Point(float x, float y) : x(x), y(y) {}
    Point() : x(0), y(0) {}
    Point getRotated(float angle) const;
    float getDistanceTo(Point other) const;

    float x;
    float y;
};

typedef std::vector<Point> PointList;
typedef std::vector<Point>::iterator PointListIt;

class Polygon {

public:
    Polygon();
    Polygon(const PointList& points);

    void addPoint(float x, float y);
    void addPoint(Point point);
    bool containsPoint(float x, float y) const;
    Polygon getTranslated(float dx, float dy) const;
    Polygon getScaled(float sx, float sy) const;
    Polygon getRotated(float angle) const;

private:
    PointList points;

};

class Circle {

public:
	struct Intersections {
		Intersections() : exist(false), x1(-1), y1(-1), x2(-1), y2(-1) {}

		bool exist;
		float x1;
		float y1;
		float x2;
		float y2;
	};

	Circle(float x, float y, float radius) : x(x), y(y), radius(radius) {}

	Intersections getIntersections(const Circle& other);

	float x;
	float y;
	float radius;

};

class Avg {

public:
	Avg(int sampleCount = 10) : sampleCount(sampleCount) {}
	void add(float sample);
	float value();
	int size() { return samples.size(); }
	bool full() { return size() == sampleCount; }
	void clear() { samples.clear(); }

private:
	int sampleCount;
	std::vector<float> samples;

};

static float getAngleBetween(Math::Position pointA, Math::Position pointB, float orientationB) {
	Vector forwardVec = Vector::createForwardVec(orientationB);
	Vector dirVec = Vector::createDirVec(pointA, pointB);

	float angle = atan2(dirVec.y, dirVec.x) - atan2(forwardVec.y, forwardVec.x);

	if (angle < -Math::PI) {
		angle += Math::PI * 2;
	} else if (angle > Math::PI) {
		angle -= Math::PI * 2;
	}

	return angle;
};

//Get acceleration needed for change in speed (while travelling a certain distance)
static float getAcceleration(float currentSpeed, float finalSpeed, float distance){
	//return (Math::pow(finalSpeed, 2.0f) - Math::pow(currentSpeed, 2.0f)) / (2.0f * distance);
	return (Math::pow(finalSpeed, 2.0f) - Math::pow(currentSpeed, 2.0f)) / distance;
}

} // namespace Math

#endif // MATHS_H
