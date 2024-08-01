#pragma once
#define PI ( 3.1415927f )

#include <iostream>
#include <math.h>

static constexpr float DEG_RAD2 = PI / 360.0f;
static constexpr float DEG_RAD = 180.0f / PI;

struct Vector2
{
	float x, y;
	Vector2() { x = y = 0; }
	Vector2(float a, float b) : x(a), y(b) {}
	Vector2& operator=(const Vector2 &copy) {
		x = copy.x;
		y = copy.y;
		return *this;
	}
	bool operator==(const Vector2 &o) const { return x == o.x && y == o.y; }
	bool operator!=(const Vector2 &o) const { return x != o.x || y != o.y; }

	Vector2 &sub(float f) { x -= f; y -= f; return *this; }
	Vector2 &div(float f) { x /= f; y /= f; return *this; }

	Vector2 &div(const Vector2 &o) { x /= o.x; y /= o.y; return *this; }
	Vector2 &mul(const Vector2 &o) { x *= o.x; y *= o.y; return *this; }
	Vector2 &mul(float f) { x *= f; y *= f; return *this; }

	Vector2 &sub(const Vector2 &o) { x -= o.x; y -= o.y; return *this; }
	Vector2 &add(const Vector2 &o) { x += o.x; y += o.y; return *this; }


	Vector2 &normAngles() {
		while (x > 89.9f)
			x -= 180.0f;
		while (x < -89.9f)
			x += 180.0f;

		while (y > 180.0f)
			y -= 360.0f;
		while (y < -180.0f)
			y += 360.0f;
		return *this;
	}

	float squaredlen() const { return x * x + y * y; }
	float magnitude() const { return sqrtf(squaredlen()); }
};

struct Vector3
{
	union {
		struct {
			float x, y, z;
		};
		float floatArr[3];
	};
	

	Vector3() { x = y = z = 0; }
	Vector3(float a, float b, float c) : x(a), y(b), z(c) {}
	Vector3(float a, float b) : x(a), y(b), z(0) {}
	Vector3(const Vector3 &copy) {
		x = copy.x;
		y = copy.y;
		z = copy.z;
	}

	Vector3& operator=(const Vector3 &copy) {
		x = copy.x;
		y = copy.y;
		z = copy.z;

		return *this;
	}


	Vector3(float *v) : x(v[0]), y(v[1]), z(v[2]) {}

	bool iszero() const { return x == 0 && y == 0 && z == 0; }

	bool operator==(const Vector3 &o) const { return x == o.x && y == o.y && z == o.z; };
	bool operator!=(const Vector3 &o) const { return x != o.x || y != o.y || z != o.z; };
	Vector3 operator-() const { return Vector3(-x, -y, -z); };

	Vector3 &mul(float f) { x *= f; y *= f; z *= f; return *this; };
	Vector3 &div(float f) { x /= f; y /= f; z /= f; return *this; };
	Vector3 &add(float f) { x += f; y += f; z += f; return *this; };
	Vector3 &sub(float f) { x -= f; y -= f; z -= f; return *this; };

	Vector3 &floor() {
		x = floorf(x);
		y = floorf(y);
		z = floorf(z);
		return *this;
	};

	__forceinline Vector3 &add(const Vector3 &o) { x += o.x; y += o.y; z += o.z; return *this; }
	__forceinline Vector3 &sub(const Vector3 &o) { x -= o.x; y -= o.y; z -= o.z; return *this; }

	float squaredlen() const { return x * x + y * y + z * z; }
	float squaredxzlen() const { return x * x + z * z; }

	inline Vector3 lerp(const Vector3 other, float val) {
		Vector3 ne;
		ne.x = x + val * (other.x - x);
		ne.y = y + val * (other.y - y);
		ne.z = z + val * (other.z - z);
		return ne;
	}

	inline Vector3 lerp(const Vector3* other, float val) {
		Vector3 ne;
		ne.x = x + val * (other->x - x);
		ne.y = y + val * (other->y - y);
		ne.z = z + val * (other->z - z);
		return ne;
	}

	float sqrxy() const { return x * x + y * y; }

	float dot(const Vector3 &o) const { return x * o.x + y * o.y + z * o.z; }
	float dotxy(const Vector3 &o) const { return x * o.x + y * o.y; }

	float magnitude() const { return sqrtf(squaredlen()); }

	Vector3 &normalize() { div(magnitude()); return *this; }

	float dist(const Vector3 &e) const { Vector3 t; return dist(e, t); }

	float dist(const Vector3 &e, Vector3 &t) const { t = *this; t.sub(e); return t.magnitude(); }

	float Get2DDist(const Vector3 &e) const { float dx = e.x - x, dy = e.y - y; return sqrtf(dx*dx + dy * dy); }

	float magnitudexy() const { return sqrtf(x*x + y * y); }
	float magnitudexz() const { return sqrtf(x*x + z * z); }

	Vector3 &cross(const Vector3 &a, const Vector3 &b) { x = a.y*b.z - a.z*b.y; y = a.z*b.x - a.x*b.z; z = a.x*b.y - a.y*b.x; return *this; }
	float cxy(const Vector3 &a) { return x * a.y - y * a.x; }

	Vector2 CalcAngle(Vector3 dst)
	{
		Vector3 diff = dst.sub(*this);

		diff.y = diff.y / diff.magnitude();
		Vector2 angles;
		angles.x = asinf(diff.y) * -DEG_RAD;
		angles.y = (float)-atan2f(diff.x, diff.z) * DEG_RAD;

		return angles;
	}
	Vector3 DifferenceAngle(Vector3 to)
	{
		Vector3 add;
		add.x = to.x - this->x;
		add.y = to.y - this->y;
		return add;
	}
	float DifferenceOfAngles(Vector3 to)
	{
		Vector3 from = *this;
		Vector3 vdifference;
		vdifference.y = from.y - to.y;
		vdifference.x = from.x - to.x;

		//normalize by making them positive values if they are negative
		if (vdifference.y < 0)
		{
			vdifference.y *= -1;
		}
		if (vdifference.x < 0)
		{
			vdifference.x *= -1;
		}

		//add them together and divide by 2, gives an average of the 2 angles
		float fDifference = (vdifference.y + vdifference.x) / 2;
		return fDifference;
	}
	Vector3 scaleFixedPoint(float scalex, float scaley, Vector3 fixedPoint)
	{
		Vector3 newvec;
		newvec.x = x * scalex + fixedPoint.x*(1 - scalex);
		newvec.y = y * scaley + fixedPoint.y*(1 - scaley);
		return newvec;
	}


	bool WorldToScreen2(float fovx, float fovy, float windowWidth, float windowHeight, Vector3 left, Vector3 up, Vector3 forward, Vector3 origin, Vector3 &screen)
	{
		Vector3 transform;
		float xc, yc;
		float px, py;
		float zO;

		px = (float)tan(fovx * DEG_RAD2);
		py = (float)tan(fovy * DEG_RAD2);

		transform = this->sub(origin); //this = destination

		xc = windowWidth / 2.0f;
		yc = windowHeight / 2.0f;

		zO = transform.dot(left);
		if (zO <= 0.1) { return false; }

		screen.x = xc - transform.dot(up) *xc / (zO*px);
		screen.y = yc - transform.dot(forward) *yc / (zO*py);
		return true;
	}
};

struct Vector3i {
	int x, y, z;

	Vector3i() { x = y = z = 0; }
	Vector3i(int a, int b, int c) : x(a), y(b), z(c) {}
	Vector3i(int a, int b) : x(a), y(b), z(0) {}
	Vector3i(const Vector3i &copy) {
		x = copy.x;
		y = copy.y;
		z = copy.z;
	}

	Vector3i(const Vector3 &copy) {
		x = (int) copy.x;
		y = (int) copy.y;
		z = (int) copy.z;
	}

	Vector3i& operator=(const Vector3i &copy) {
		x = copy.x;
		y = copy.y;
		z = copy.z;

		return *this;
	}

	Vector3i* operator=(const Vector3i *copy) {
		x = copy->x;
		y = copy->y;
		z = copy->z;

		return this;
	}


	Vector3i(int *v) : x(v[0]), y(v[1]), z(v[2]) {}

	bool iszero() const { return x == 0 && y == 0 && z == 0; }

	bool operator==(const Vector3i &o) const { return x == o.x && y == o.y && z == o.z; }
	bool operator!=(const Vector3i &o) const { return x != o.x || y != o.y || z != o.z; }

	Vector3i &add(int f) { x += f; y += f; z += f; return *this; }

	Vector3i &add(int a, int b, int c) {
		x += a;
		y += b;
		z += c;

		return *this;
	};

	Vector3i* addAndReturn(const Vector3i o) {
		return new Vector3i(x + o.x, y + o.y, z + o.z);
	}

	Vector3i* subAndReturn(const Vector3i o) {
		return new Vector3i(x - o.x, y - o.y, z - o.z);
	}

	void set(Vector3i* o) {
		x = o->x;
		y = o->y;
		z = o->z; 
	};

	Vector3 toFloatVector() {
		Vector3 vec;
		vec.x = (float)x;
		vec.y = (float)y;
		vec.z = (float)z;
		return vec;
	}
};
struct Vector4
{
	union
	{
		struct { float x, y, z, w; };
		float v[4];
	};
	Vector4() { x = 0, y = 0, z = 0, w = 0;}
	explicit Vector4(const Vector3 &p, float w = 0) : x(p.x), y(p.y), z(p.z), w(w) {};
	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
	float &operator[](int i) { return v[i]; };
	float  operator[](int i) const { return v[i]; };

	inline bool contains(Vector2 *point) { 
		/*
		Assumes:
			start: Vector2(x, y)
			end:   Vector2(z, w)

			start < end
		*/
		Vector2 start = Vector2(x, y);
		Vector2 end = Vector2(z, w);

		if (point->x <= start.x || point->y <= start.y)
			return false;

		if (point->x >= end.x || point->y >= end.y)
			return false;
		return true;
	};
};


struct glmatrixf
{
	union {
		float v[16];
		float v_nested[4][4];
	};
	

	__forceinline float operator[](int i) const { return v[i]; }
	__forceinline float &operator[](int i) { return v[i]; }

#define MULMAT(row, col) v[col + row] = x[row]*y[col] + x[row + 4]*y[col + 1] + x[row + 8]*y[col + 2] + x[row + 12]*y[col + 3];

	template<class XT, class YT>
	void mul(const XT x[16], const YT y[16])
	{
		MULMAT(0, 0); MULMAT(1, 0); MULMAT(2, 0); MULMAT(3, 0);
		MULMAT(0, 4); MULMAT(1, 4); MULMAT(2, 4); MULMAT(3, 4);
		MULMAT(0, 8); MULMAT(1, 8); MULMAT(2, 8); MULMAT(3, 8);
		MULMAT(0, 12); MULMAT(1, 12); MULMAT(2, 12); MULMAT(3, 12);
	}

#undef MULMAT

	glmatrixf* correct() {
		glmatrixf* newMatPtr = new glmatrixf;

		for (int i = 0; i < 4; i++) {
			newMatPtr->v[i * 4 + 0] = v[0 + i];
			newMatPtr->v[i * 4 + 1] = v[4 + i];
			newMatPtr->v[i * 4 + 2] = v[8 + i];
			newMatPtr->v[i * 4 + 3] = v[12 + i];
		}
		return newMatPtr;
	};

	inline bool OWorldToScreen(Vector3 origin, Vector3 pos, Vector2 &screen, Vector2 fov, Vector2 displaySize)
	{
		pos = pos.sub(origin);

		float x = transformx(pos);
		float y = transformy(pos);
		float z = transformz(pos);

		if (z > 0)
			return false;

		float mX = (float)displaySize.x / 2.0F;
		float mY = (float)displaySize.y / 2.0F;

		screen.x = mX + (mX * x / -z * fov.x);
		screen.y = mY - (mY * y / -z * fov.y);

		return true;
	}

	inline void mul(const glmatrixf &x, const glmatrixf &y)
	{
		mul(x.v, y.v);
	}

	inline void translate(float x, float y, float z)
	{
		v[12] += x;
		v[13] += y;
		v[14] += z;
	}

	inline void translate(const Vector3 &o)
	{
		translate(o.x, o.y, o.z);
	}

	inline void scale(float x, float y, float z)
	{
		v[0] *= x; v[1] *= x; v[2] *= x; v[3] *= x;
		v[4] *= y; v[5] *= y; v[6] *= y; v[7] *= y;
		v[8] *= z; v[9] *= z; v[10] *= z; v[11] *= z;
	}

	inline void invertnormal(Vector3 &dir) const
	{
		Vector3 n(dir);
		dir.x = n.x*v[0] + n.y*v[1] + n.z*v[2];
		dir.y = n.x*v[4] + n.y*v[5] + n.z*v[6];
		dir.z = n.x*v[8] + n.y*v[9] + n.z*v[10];
	}

	inline void invertvertex(Vector3 &pos) const
	{
		Vector3 p(pos);
		p.x -= v[12];
		p.y -= v[13];
		p.z -= v[14];
		pos.x = p.x*v[0] + p.y*v[1] + p.z*v[2];
		pos.y = p.x*v[4] + p.y*v[5] + p.z*v[6];
		pos.z = p.x*v[8] + p.y*v[9] + p.z*v[10];
	}

	inline void transform(const Vector3 &in, Vector4 &out) const
	{
		out.x = transformx(in);
		out.y = transformy(in);
		out.z = transformz(in);
		out.w = transformw(in);
	}

	__forceinline float transformx(const Vector3 &p) const
	{
		return p.x*v[0] + p.y*v[4] + p.z*v[8] + v[12];
	}

	__forceinline float transformy(const Vector3 &p) const
	{
		return p.x*v[1] + p.y*v[5] + p.z*v[9] + v[13];
	}

	__forceinline float transformz(const Vector3 &p) const
	{
		return p.x*v[2] + p.y*v[6] + p.z*v[10] + v[14];
	}

	__forceinline float transformw(const Vector3 &p) const
	{
		return p.x*v[3] + p.y*v[7] + p.z*v[11] + v[15];
	}

	__forceinline Vector3 gettranslation() const
	{
		return Vector3(v[12], v[13], v[14]);
	}

	//assault cube world2screen
	Vector3 transform(glmatrixf *matrix, Vector3 &totransform)
	{
		return Vector3(matrix->transformx(totransform),
			matrix->transformy(totransform),
			matrix->transformz(totransform)).div(matrix->transformw(totransform));
	}

	///pos should be the exact center of the enemy model for scaling to work properly
	Vector3 WorldToScreen(Vector3 pos, int width, int height)
	{
		//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
		Vector4 clipCoords;
		clipCoords.x = pos.x*v[0] + pos.y*v[4] + pos.z*v[8] + v[12];
		clipCoords.y = pos.x*v[1] + pos.y*v[5] + pos.z*v[9] + v[13];
		clipCoords.z = pos.x*v[2] + pos.y*v[6] + pos.z*v[10] + v[14];
		clipCoords.w = pos.x*v[3] + pos.y*v[7] + pos.z*v[11] + v[15];

		//perspective division, dividing by clip.W = Normalized Device Coordinates
		Vector3 NDC;
		NDC.x = clipCoords.x / clipCoords.w;
		NDC.y = clipCoords.y / clipCoords.w;
		NDC.z = clipCoords.z / clipCoords.w;

		//viewport tranform to screenCooords

		Vector3 playerscreen;
		playerscreen.x = (width / 2 * NDC.x) + (NDC.x + width / 2);
		playerscreen.y = -(height / 2 * NDC.y) + (NDC.y + height / 2);

		return playerscreen;
	}
};

struct AABB {
	Vector3 lower;
	Vector3 upper;
	AABB() {}
	AABB(const AABB &aabb) {
		lower = Vector3(aabb.lower);
		upper = Vector3(aabb.upper);
	}
	AABB(Vector3 lower, float width, float height, float eyeHeight) {
		this->lower = lower.sub(Vector3(width, eyeHeight * 2, width).div(2));
		upper = Vector3(lower.x + width, lower.y + height, lower.z + width);
	}

	bool operator==(const AABB &rhs) const {
		return lower == rhs.lower && upper == rhs.upper;
	}
};

inline int random(int start, int end) {
	return rand() % (end - start + 1) + start;
}

inline float randomf(int start, int end) {
	return (float)random(start, end);
}
