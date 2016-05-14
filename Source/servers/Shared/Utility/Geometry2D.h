#ifndef __GEOMETRY2D_H_
#define __GEOMETRY2D_H_

#include <limits>  
#include <math.h>
#include "BaseDefine.h"

struct Point2DEx;
struct Point2D
{
	Point2D(int32 nX = 0, int32 nY = 0) : nX(nX), nY(nY) { }
	Point2D(const Point2DEx &src);
	int32 nX;		// x轴正方向向左 
	int32 nY;		// z轴正方向向下 
};

struct Point2DEx
{
	float fX;		// x轴正方向向左 
	float fY;		// z轴正方向向下 

	Point2DEx() { }
	Point2DEx(const Point2D& src) : fX(src.nX * 1.f), fY(src.nY * 1.f) { }
};

inline bool operator==(const Point2D &left, const Point2D &right)
{
	return !memcmp(&left, &right, sizeof(Point2D));
}

inline int32 DistanceSq(const Point2D &left, const Point2D &right)
{
	int32 nXSeparation = right.nX - left.nX;
	int32 nZSeparation = right.nY - left.nY;

	return nXSeparation * nXSeparation + nZSeparation * nZSeparation;
}
inline int32 DistanceSq(const Point2DEx &left, const Point2DEx &right)
{
	float nXSeparation = right.fX - left.fX;
	float nZSeparation = right.fY - left.fY;
	float nSqResult = nXSeparation * nXSeparation + nZSeparation * nZSeparation;
	return static_cast<int32>(nSqResult);
}
struct Vector2D
{
	int32 nX;		// x轴正方向向左 
	int32 nY;		// z轴正方向向下 

	Vector2D() : nX(0), nY(0) { }
	Vector2D(int32 _nX,int32 _nZ) :nX(_nX),nY(_nZ) {}
	Vector2D(const Vector2D& src) : nX(src.nX), nY(src.nY) { }
	Vector2D(const Point2D& src) : nX(src.nX), nY(src.nY) { }
};

struct Vector2DEx
{
	float fX;		// x轴正方向向左 
	float fY;		// z轴正方向向下 

	Vector2DEx() { }
	Vector2DEx(const Vector2D& src) : fX(src.nX * 1.f), fY(src.nY * 1.f) { }

	inline float LengthSq() const
	{
		return (fX * fX + fY * fY);
	}

	inline float Length() const
	{
		return sqrt(fX * fX + fY * fY);
	}
	
	inline float Dot(const Vector2DEx& v2) const
	{
		return fX * v2.fX + fY * v2.fY;
	}

	inline Vector2DEx& Normalize()
	{
		float vector_length = Length();

		if (vector_length > std::numeric_limits<float>::epsilon())
		{
			fX /= vector_length;
			fY /= vector_length;
		}

		return *this;
	}

	const Vector2DEx& operator=(const Vector2D& rVector2D )
	{
		this->fX = static_cast<float>(rVector2D.nX);
		this->fY = static_cast<float>(rVector2D.nY);
		return *this;
	}
};



struct Rectangle2D
{
	Rectangle2D() { }
	// x1,y1-----
	// -----x2,y2
	Rectangle2D(int32 nLeft, int32 nTop, int32 nRight,int32 nBottom)
		: sTopLeft(nLeft, nTop), sBottomRight(nRight, nBottom) { }

	Point2D sTopLeft;		// 左上角坐标 
	Point2D sBottomRight;	// 右下角坐标 
};

// 两点之间的向量 
inline Vector2DEx operator-(const Point2D &left, const Point2DEx &right)
{
	Vector2DEx result;
	result.fX = left.nX - right.fX;
	result.fY = left.nY - right.fY;

	return result;
}
inline Vector2DEx operator-(const Point2DEx &left, const Point2DEx &right)
{
	Vector2DEx result;
	result.fX = left.fX - right.fX;
	result.fY = left.fY - right.fY;

	return result;
}

// 点平移 
inline Point2D operator+(const Point2D &left, const Vector2D &right)
{
	Point2D result;
	result.nX = left.nX + right.nX;
	result.nY = left.nY + right.nY;

	return result;
}
// 点平移重载 
inline Point2D operator+(const Point2D &left, const Vector2DEx &right)
{
	Point2D result;
	result.nX = static_cast<int32>(left.nX + right.fX);
	result.nY = static_cast<int32>(left.nY + right.fY);
	return result;
}

inline Point2DEx operator-(const Point2D &left, const Vector2DEx &right)
{
	Point2DEx result;
	result.fX = left.nX - right.fX;
	result.fY = left.nY - right.fY;

	return result;
}


inline Vector2DEx operator*(const Vector2DEx &right,int32 nMultiple)
{
	Vector2DEx result;
	result.fX = right.fY * nMultiple;
	result.fY = right.fY * nMultiple;
	return result;
}

// 矩形平移 
inline Rectangle2D operator+(const Rectangle2D &left, const Vector2D &right)
{
	Rectangle2D result;
	result.sTopLeft = left.sTopLeft + right;
	result.sBottomRight = left.sBottomRight + right;

	return result;
}

struct RectangleRange : public Rectangle2D
{
	RectangleRange() {}
	RectangleRange(const Rectangle2D& _sRange)
		: sStandardRange(_sRange)
	{ }

	inline void SetPos(const Point2D& sPosition)
	{
		*static_cast<Rectangle2D*>(this) = sStandardRange + sPosition;
	}

	Rectangle2D		sStandardRange;		// 标准范围 
};

// 矩形相交检测 
inline bool CheckIntersect(const Rectangle2D& rFirst, const Rectangle2D& rSecond)
{
	return !(rFirst.sTopLeft.nX > rSecond.sBottomRight.nX ||
		rFirst.sTopLeft.nY < rSecond.sBottomRight.nY ||
		rFirst.sBottomRight.nX < rSecond.sTopLeft.nX ||
		rFirst.sBottomRight.nY > rSecond.sTopLeft.nY);
}

inline bool CheckIntersect(const RectangleRange& rFirst, const Rectangle2D& rSecond)
{
	return !(rFirst.sTopLeft.nX > rSecond.sBottomRight.nX ||
		rFirst.sTopLeft.nY < rSecond.sBottomRight.nY ||
		rFirst.sBottomRight.nX < rSecond.sTopLeft.nX ||
		rFirst.sBottomRight.nY > rSecond.sTopLeft.nY);
}

inline bool GetIntersect(const Rectangle2D& i_rFirst, const Rectangle2D& i_rSecond, Rectangle2D& o_rResult)
{
	if (CheckIntersect(i_rFirst, i_rSecond))
	{
		o_rResult.sTopLeft.nX = (i_rFirst.sTopLeft.nX > i_rSecond.sTopLeft.nX) ? i_rFirst.sTopLeft.nX : i_rSecond.sTopLeft.nX;
		o_rResult.sTopLeft.nY = (i_rFirst.sTopLeft.nY < i_rSecond.sTopLeft.nY) ? i_rFirst.sTopLeft.nY : i_rSecond.sTopLeft.nY;
		o_rResult.sBottomRight.nX = (i_rFirst.sBottomRight.nX > i_rSecond.sBottomRight.nX) ? i_rSecond.sBottomRight.nX : i_rFirst.sBottomRight.nX;
		o_rResult.sBottomRight.nY = (i_rFirst.sBottomRight.nY < i_rSecond.sBottomRight.nY) ? i_rSecond.sBottomRight.nY : i_rFirst.sBottomRight.nY;
	
		return true;
	}
	else
	{
		return false;
	}
}

// 点在矩形中检测 
inline bool CheckInside(const Point2D& rFirst, const Rectangle2D& rSecond)
{
	return !(rFirst.nX < rSecond.sTopLeft.nX ||
		rFirst.nY > rSecond.sTopLeft.nY ||
		rFirst.nX > rSecond.sBottomRight.nX ||
		rFirst.nY < rSecond.sBottomRight.nY);
}

inline bool CheckInside(const Point2D& rFirst, const RectangleRange& rSecond)
{
	return !(rFirst.nX < rSecond.sTopLeft.nX ||
		rFirst.nY > rSecond.sTopLeft.nY ||
		rFirst.nX > rSecond.sBottomRight.nX ||
		rFirst.nY < rSecond.sBottomRight.nY);
}

#endif

