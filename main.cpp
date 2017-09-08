#include "SDL.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>

#define local_persist static
#define global_variable static
#define internal static

#define ArrayCount(array) (sizeof(array)/sizeof((array)[0]))

#define DEBUG 1

#if DEBUG
#define Assert(Expression) if(!(Expression)){*(int*)0 = 0;}
#else
#define Assert(Expression)
#endif




/*
TODOS:
	Renderer:
		-Change Pixels to 32 bit values
		-Create one big draw call rather than drawing
				when the circles are calculated
		-Anti-Aliasing!!!
	Collision Detection:
		-Add basic cirlce-to-circle collision detection
	Entities:
		-Add more than one type of entity
*/

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int32 bool32;

typedef float real32;
typedef double real64;

#define TAU32 6.28318530718f
#define TAU64 6.28318530717958647692

inline real32 
SqRt(real32 A)
{
	real32 Result = sqrtf(A);
	return Result;
}

int32
Sign(int32 A)
{
	int32 Result = A > 0 ? 1 : (A < 0 ? -1 : 0);
	return Result;
}

inline real32
Square(real32 A)
{
	real32 Result = A * A;
	return Result;
}

inline real64
Square(real64 A)
{
	real64 Result = A * A;
	return Result;
}

inline int32
Square(int32 A)
{
	int32 Result = A * A;
	return Result;
}
inline int64
Square(int64 A)
{
	int64 Result = A * A;
	return Result;
}

inline real32
Abs(real32 A)
{
	real32 Result = fabsf(A);
	return Result;
}

inline int32
Abs(int32 A)
{
	int32 Result = abs(A);
	return Result;
}

inline int64
Abs(int64 A)
{
	int64 Result = abs(A);
	return Result;
}

inline real32
Sine(real32 A)
{
	real32 Result = sinf(A);
	return Result;
}

inline real32
Cosine(real32 A)
{
	real32 Result = cosf(A);
	return Result;
}

inline real32
ArcTan2(real32 A, real32 B)
{
	real32 Result = atan2f(B, A);
	return Result;
}

inline real32
Cube(real32 A)
{
	real32 Result = A * A * A;
	return Result;
}

typedef struct {
	union {
		struct {
			real32 X;
			real32 Y;
		};
		real32 E[2];
	};
}v2;


inline v2 
operator+(v2 A, v2 B)
{
	v2 Result;
	Result.X = A.X + B.X;
	Result.Y = A.Y + B.Y;
	return Result;
}

inline v2 
operator-(v2 A, v2 B)
{
	v2 Result;
	Result.X = A.X - B.X;
	Result.Y = A.Y - B.Y;
	return Result;
}

inline v2 
operator-(v2 A)
{
	v2 Result;
	Result.X = -A.X;
	Result.Y = -A.Y;
	return Result;
}

inline v2 
operator*(real32 A, v2 B)
{
	v2 Result;
	Result.X = A * B.X;
	Result.Y = A * B.Y;
	return Result;
}

inline v2 
operator*(v2 A, real32 B)
{
	v2 Result;
	Result.X = A.X * B;
	Result.Y = A.Y * B;
	return Result;
}

inline v2 
operator/(v2 A, real32 B)
{
	v2 Result;
	Result.X = A.X / B;
	Result.Y = A.Y / B;
	return Result;
}


inline v2 
operator+=(v2 A, v2 B)
{
	v2 Result;
	Result.X = A.X + B.X;
	Result.Y = A.Y + B.Y;
	return Result;
}

inline v2 
operator-=(v2 A, v2 B)
{
	v2 Result;
	Result.X = A.X - B.X;
	Result.Y = A.Y - B.Y;
	return Result;
}

inline real32 
Inner(v2 A, v2 B)
{
	real32 Result = (A.X * B.X) + (A.Y * B.Y);
	return Result;
}

inline real32 
LengthSq(v2 A)
{
	real32 Result = Inner(A, A);
	return Result;
}

inline real32 
Length(v2 A)
{
	real32 Result = SqRt(LengthSq(A));
	return Result;
}

inline real32 
DistanceSq(v2 A, v2 B)
{
	real32 Result = LengthSq(A - B);
	return Result;
}

inline real32 
Distance(v2 A, v2 B)
{
	real32 Result = Length(A - B);
	return Result;
}

inline v2
Normalized(v2 A)
{
	real32 Size = Length(A);
	Assert(Size != 0.0f);
	v2 Result = A / Size;
	return Result;
}


inline real32
GetAngle(v2 A)
{
	real32 Result = ArcTan2(A.X, A.Y);
	if (Result < 0.0f)
	{
		Result += TAU32;
	}
	return Result;
}

inline v2
ComplexMult(v2 A, v2 B)
{
	v2 Result;
	Result.X = A.X*B.X - A.Y * B.Y;
	Result.Y = A.X*B.Y + A.Y * B.X;
	return Result;
}


typedef struct {
	uint64 Current;
	uint64 W;
}rng_state;

inline void
Seed(rng_state *State, uint32 Seed)
{
	State->Current = Seed;
	State->W = 0;

}

inline uint32
NextRandom(rng_state *State)
{
	uint64 S = 0xb5ad4eceda1ce2a9;
	State->Current *= State->Current;
	State->Current += (State->W += S);
	State->Current = (State->Current >> 32) | (State->Current << 32);
	uint32 Result = (uint32)State->Current;
	return Result;
}

#if 0
uint64_t x = 0, w = 0, s = 0xb5ad4eceda1ce2a9;

inline static uint32_t msws() {

	x *= x; x += (w += s); return x = (x >> 32) | (x << 32);

}
#endif

inline int32
RandomRange(rng_state *State, int32 Min, int32 Max)
{
	int32 Result = Min + (NextRandom(State) % ((Max + 1) - Min));
	return Result;
}

inline real32
RandomUnilateral(rng_state *State)
{
	real32 Result = (real32)NextRandom(State) / (real32)((uint32)0xffffffff);
	return Result;
}



typedef struct {
	union {
		struct {
			real32 X;
			real32 Y;
			real32 Z;
		};
		struct {
			real32 R;
			real32 G;
			real32 B;
		};
	};
}v3;

typedef struct {
	union {
		struct {
			real32 X;
			real32 Y;
			real32 Z;
			real32 W;
		};
		struct {
			real32 R;
			real32 G;
			real32 B;
			real32 A;
		};
	};
}v4;



typedef struct {
	real32 dt;
	uint8 Action;
	uint8 Toggle;
	uint8 Up;
	uint8 Down;
	uint8 Left;
	uint8 Right;
}user_input;

typedef struct {
	int32 X;
	int32 Y;
	int32 W;
	int32 H;
}render_dim;

typedef struct {
	int32 X;
	int32 Y;
}render_coord;

typedef struct {
	union {
		struct {
			v2 P;
			v2 Size;
		};
		struct {
			real32 X;
			real32 Y;
			real32 W;
			real32 H;
		};
	};
}rect2;

typedef struct {
	v2 P;
	real32 R;
}circle;


typedef struct {
	rect2 Screen;
	render_dim RenderRect;
	circle Inversion;
	circle Player;
	circle Attack;
	circle Boundary;
	circle BoundaryAttack;
	circle Curiosity;
	rng_state RNG;
	real64 tSine;
	uint8 Toggle;
	uint16 SoundOffset;
}game_state;


inline v2
Invert(circle C, v2 P)
{
	v2 Result;
	v2 Diff = P - C.P;
	real32 Scalar = Square(C.R) / LengthSq(Diff);
	Result = (Scalar * Diff) + C.P;
	return Result;
}

inline v2
InvertFromUnit(v2 P)
{
	v2 Result = P / LengthSq(P);
	return Result;
}

/*
inline void
InvertCircle(circle Inversion, circle BeingInverted, v2 *NewP, real32 *NewRadius)
{
	v2 A = Invert(Invers, MainR, CircleP - v2{ CircleR,0.0f });
	v2 B = Invert(Origin, MainR, CircleP + v2{ CircleR,0.0f });
	v2 C = Invert(Origin, MainR, CircleP - v2{ 0.0f,CircleR });


	//NOTE(ian): This is based on the Circumcenter Coordinates section 
	//of the Wikipedia article on the circumcenter
	v2 PointB = B - A;
	v2 PointC = C - A;
	real32 LengthB = LengthSq(PointB);
	real32 LengthC = LengthSq(PointC);
	real32 Denominator = 2.0f*(PointB.X*PointC.Y - PointB.Y*PointC.X);
	v2 CircleCenter;
	CircleCenter.X = (PointC.Y * LengthB - PointB.Y * LengthC) / Denominator;
	CircleCenter.Y = (PointB.X*LengthC - PointC.X*LengthB) / Denominator;
	CircleCenter = CircleCenter + A;

	*NewP = CircleCenter;
	*NewRadius = Distance(CircleCenter, A);
}
*/

inline circle
InvertCircleFromUnit(circle Circle)
{
	v2 A = InvertFromUnit(Circle.P - v2{ Circle.R,0.0f });
	v2 B = InvertFromUnit(Circle.P + v2{ Circle.R,0.0f });
	v2 C = InvertFromUnit(Circle.P - v2{ 0.0f,Circle.R });


	//NOTE(ian): This is based on the Circumcenter Coordinates section 
	//of the Wikipedia article on the circumcenter
	v2 PointB = B - A;
	v2 PointC = C - A;
	real32 LengthB = LengthSq(PointB);
	real32 LengthC = LengthSq(PointC);
	real32 Denominator = 2.0f*(PointB.X*PointC.Y - PointB.Y*PointC.X);
	circle Result;
	Result.P.X = (PointC.Y * LengthB - PointB.Y * LengthC) / Denominator;
	Result.P.Y = (PointB.X*LengthC - PointC.X*LengthB) / Denominator;
	Result.P = Result.P + A;
	Result.R = Distance(Result.P, A);

	return Result;
}


inline real32
Clip(real32 Min, real32 A, real32 Max)
{
	real32 Result = A;
	if (A < Min)
	{
		Result = Min;
	}
	else if (A > Max)
	{
		Result = Max;
	}

	return Result;
}

inline int32
Clip(int32 Min, int32 A, int32 Max)
{
	int32 Result = A;
	if (A < Min)
	{ 
		Result = Min;
	}
	else if (A > Max)
	{
		Result = Max;
	}

	return Result;
}

inline int64
Clip(int64 Min, int64 A, int64 Max)
{
	int64 Result = A;
	if (A < Min)
	{
		Result = Min;
	}
	else if (A > Max)
	{
		Result = Max;
	}

	return Result;
}




inline v2
Clip(rect2 *Rect, v2 Point)
{
	v2 Result;
	Result.X = Clip(Rect->P.X, Point.X, Rect->P.X + Rect->Size.X);
	Result.Y = Clip(Rect->P.Y, Point.Y, Rect->P.Y + Rect->Size.Y);
	return Result;
}

inline bool32
WithinRenderRect(render_dim *Rect, int32 X, int32 Y)
{
	bool32 Result = (X >= Rect->X && X < Rect->X + Rect->W) &&
		(Y >= Rect->Y && Y < Rect->Y + Rect->H);
	return Result;
}

inline bool32
InsideRect(rect2 *Rect, v2 A)
{
	bool32 Result = (A.X >= Rect->P.X && A.X < Rect->P.X + Rect->Size.X) &&
		(A.Y >= Rect->P.Y && A.Y < Rect->P.Y + Rect->Size.Y);
	return Result;
}

inline real32
GetArea(rect2 *Rect)
{
	real32 Result = Rect->Size.X * Rect->Size.Y;
	return Result;
}


inline rect2
CircleExtent(circle Circle)
{
	rect2 Result;
	Result.P = Circle.P - v2{ Circle.R,Circle.R };
	Result.Size.X = Circle.R * 2.0f;
	Result.Size.Y = Result.Size.X;
	return Result;
}

inline rect2
CircleExtent(v2 CircleP, real32 Radius)
{
	rect2 Result;
	Result.P = CircleP - v2{ Radius,Radius };
	Result.Size.X = Radius * 2.0f;
	Result.Size.Y = Result.Size.X;
	return Result;
}


inline bool32
Intersects(rect2 *A, rect2 *B)
{
	bool32 Result = !(((A->P.X >= B->P.X + B->Size.X) ||
		(A->P.X + A->Size.X <= B->P.X)) ||
		((A->P.Y >= B->P.Y + B->Size.Y) ||
		(A->P.Y + A->Size.Y <= B->P.Y)));
	return Result;
}

inline rect2
GetIntersection(rect2 *A, rect2 *B)
{
	rect2 Result;

	Result.P.X = A->P.X > B->P.X ? A->P.X : B->P.X;
	Result.P.Y = A->P.Y > B->P.Y ? A->P.Y : B->P.Y;
	Result.Size.X = (A->Size.X + A->P.X < B->Size.X + B->P.X ?
		A->Size.X + (A->P.X -Result.P.X): B->Size.X + (B->P.X-Result.P.X));
	Result.Size.Y = (A->Size.Y + A->P.Y < B->Size.Y + B->P.Y ? 
		A->Size.Y + (A->P.Y-Result.P.Y) : B->Size.Y + (B->P.Y-Result.P.Y));

	return Result;
}

inline void
SplitRectHorizontally(rect2 *Rects, real32 Y)
{

}

inline void
SplitRectVertically(rect2 *Rects, real32 X)
{

}

inline real32
Lerp(real32 A, real32 t, real32 B)
{
	real32 Result = (1.0f - t) * A + t * B;
	return Result;
}

inline v2
Lerp(v2 A, real32 t, v2 B)
{
	v2 Result = (1.0f - t) * A + t * B;
	return Result;
}


inline real32
Berp(real32 A, real32 B, real32 t, real32 C, real32 D)
{
	real32 Result = Cube(1.0f - t) * A + 3.0f * Square(1.0f - t) * t * B +
		3.0f * (1.0f - t) * Square(t) * C + Cube(t) * D;
	return Result;
}

inline v2
Berp(v2 A, v2 B, real32 t, v2 C, v2 D)
{
	v2 Result = Cube(1.0f - t) * A + (3.0f * Square(1.0f - t) * t) * B +
		(3.0f * (1.0f - t) * Square(t)) * C + Cube(t) * D;
	return Result;
}

inline int32
GetQuadrant(v2 A)
{
	/*
	Conventional Quadrant Space

	1 - I. Quadrant
	2 - II. Quadrant
	3 - III. Quadrant
	4 - IV. Quadrant
	*/


	int32 Result;
	if (A.X >= 0.0f)
	{
		if (A.Y >= 0.0f)
		{
			Result = 1;
		}
		else
		{
			Result = 4;
		}
	}
	else
	{
		if (A.Y >= 0.0f)
		{
			Result = 2;
		}
		else
		{
			Result = 3;
		}
	}

	return Result;
}

inline int32
GetSlantedQuad(v2 A)
{
	/*
	0 - Positive X
	1 - Positive Y
	2 - Negative X
	3 - Negative Y
	*/


	int32 Result;
	if (Abs(A.X) >= Abs(A.Y))
	{
		if (A.X >= 0)
		{
			Result = 0;
		}
		else
		{
			Result = 2;
		}
	}
	else
	{
		if (A.Y >= 0)
		{
			Result = 1;
		}
		else
		{
			Result = 3;
		}
	}

	return Result;
}




internal uint32 
OutCode(render_dim *Rect, int32 X, int32 Y)
{
	/*
	const int INSIDE = 0; // 0000
	const int LEFT = 1;   // 0001
	const int RIGHT = 2;  // 0010
	const int BOTTOM = 4; // 0100
	const int TOP = 8;    // 1000
	*/


	uint32 Code = 0;

	if (X < Rect->X)
	{
		Code |= 1;
	}
	else if (X > Rect->X + Rect->W)
	{
		Code |= 2;
	}
	if (Y < Rect->Y)
	{
		Code |= 4;
	}
	else if (Y > Rect->Y + Rect->H)
	{
		Code |= 8;
	}

	return Code;
}

internal void 
ClipLine(render_dim *Rect, int32 *X0, int32 *Y0, int32 *X1, int32 *Y1)
{
	uint32 Outcode0 = OutCode(Rect, *X0, *Y0);
	uint32 Outcode1 = OutCode(Rect, *X1, *Y1);
	bool32 accept = 0;

	while (true) {
		if (!(Outcode0 | Outcode1)) 
		{
			accept = true;
			break;
		}
		else if (Outcode0 & Outcode1) 
		{
			*X0 = 0;
			*Y0 = 0;
			*X1 = 0;
			*Y1 = 0;
			break;
		}
		else
		{
			int32 X, Y;
			uint32 OutcodeOut = Outcode0 ? Outcode0 : Outcode1;

			if (OutcodeOut & 8)
			{
				X = *X0 + (*X1 - *X0) * ((Rect->Y + Rect->H) - *Y0) / (*Y1 - *Y0);
				Y = (Rect->Y + Rect->H);
			}
			else if (OutcodeOut & 4) 
			{
				X = *X0 + (*X1 - *X0) * (Rect->Y - *Y0) / (*Y1 - *Y0);
				Y = Rect->Y;
			}
			else if (OutcodeOut & 2) 
			{
				Y = *Y0 + (*Y1 - *Y0) * ((Rect->X+Rect->W) - *X0) / (*X1 - *X0);
				X = (Rect->X+Rect->W);
			}
			else if (OutcodeOut & 1)
			{
				Y = *Y0 + (*Y1 - *Y0) * (Rect->X - *X0) / (*X1 - *X0);
				X = Rect->X;
			}
			
			if (OutcodeOut == Outcode0) {
				*X0 = X;
				*Y0 = Y;
				Outcode0 = OutCode(Rect, *X0, *Y0);
			}
			else {
				*X1 = X;
				*Y1 = Y;
				Outcode1 = OutCode(Rect, *X1, *Y1);
			}
		}
	}
}


internal uint32
OutCode(rect2 *Rect, v2 A)
{
	/*
	const int INSIDE = 0; // 0000
	const int LEFT = 1;   // 0001
	const int RIGHT = 2;  // 0010
	const int BOTTOM = 4; // 0100
	const int TOP = 8;    // 1000
	*/


	uint32 Code = 0;

	if (A.X < Rect->P.X)
	{
		Code |= 1;
	}
	else if (A.X > Rect->P.X + Rect->Size.X)
	{
		Code |= 2;
	}
	if (A.Y < Rect->P.Y)
	{
		Code |= 4;
	}
	else if (A.Y > Rect->P.Y + Rect->Size.Y)
	{
		Code |= 8;
	}

	return Code;
}

internal void
ClipLine(rect2 *Rect, v2 *A, v2 *B)
{
	uint32 Outcode0 = OutCode(Rect, *A);
	uint32 Outcode1 = OutCode(Rect, *B);
	bool32 accept = false;

	while (true) {
		if (!(Outcode0 | Outcode1))
		{
			accept = true;
			break;
		}
		else if (Outcode0 & Outcode1)
		{
			A->X = 0.0f;
			A->Y = 0.0f;
			B->X = 0.0f;
			B->Y = 0.0f;
			break;
		}
		else
		{
			v2 C;
			uint32 OutcodeOut = Outcode0 ? Outcode0 : Outcode1;

			if (OutcodeOut & 8)
			{
				C.X = A->X + (B->X - A->X) * ((Rect->P.Y + Rect->Size.Y) - A->Y) / (B->Y - A->Y);
				C.Y = (Rect->P.Y + Rect->Size.Y);
			}
			else if (OutcodeOut & 4)
			{
				C.X = A->X + (B->X - A->X) * (Rect->P.Y - A->Y) / (B->Y - A->Y);
				C.Y = Rect->P.Y;
			}
			else if (OutcodeOut & 2)
			{
				C.Y = A->Y + (B->Y - A->Y) * ((Rect->P.X + Rect->Size.X) - A->X) / (B->X - A->X);
				C.X = (Rect->P.X + Rect->Size.X);
			}
			else if (OutcodeOut & 1)
			{
				C.Y = A->Y + (B->Y - A->Y) * (Rect->P.X - A->X) / (B->X - A->X);
				C.X = Rect->X;
			}

			if (OutcodeOut == Outcode0) {
				*A = C;
				Outcode0 = OutCode(Rect, *A);
			}
			else {
				*B = C;
				Outcode1 = OutCode(Rect, *B);
			}
		}
	}
}




inline void
SetPixel(uint8 *Pixels, int32 Pitch, int32 X, int32 Y,
	uint8 R, uint8 G, uint8 B, uint8 A)
{
	Pixels += ((Y*Pitch) + X * 4);
	*Pixels++ = A; // B 
	*Pixels++ = G; // G
	*Pixels++ = B; // R
	//*Pixels++ = R; // A
}

inline void
SetPixelSafe(uint8 *Pixels, int32 Pitch, render_dim *RenderRect, int32 X, int32 Y,
	uint8 R, uint8 G, uint8 B, uint8 A)
{
	if (WithinRenderRect(RenderRect, X, Y))
	{
		SetPixel(Pixels, Pitch, X, Y, R, G, B, A);
	}
}

inline void
ConvertCoordToPixelSpace(rect2 *ScreenRect, v2 P, render_dim *RenderRect,
	int64 *X, int64 *Y)
{
	*X = (int64)((P.X - ScreenRect->P.X) * (real32)(RenderRect->W) / ScreenRect->Size.X) + (int64)RenderRect->X;
	*Y = (int64)((P.Y - ScreenRect->P.Y) * (real32)(RenderRect->H) / ScreenRect->Size.Y) + (int64)RenderRect->Y;
}

inline void
ConvertDistToPixelDist(rect2 *ScreenRect, real32 Dist, render_dim *RenderRect, int64 *PixelDist)
{
	*PixelDist = (int64)(Dist * (real32)(RenderRect->H) / ScreenRect->Size.Y);
}

inline void
ConvertCoordToPixelSpace(rect2 *ScreenRect, v2 P, render_dim *RenderRect,
	int32 *X, int32 *Y)
{
	*X = (int32)((P.X - ScreenRect->P.X) * (real32)(RenderRect->W) / ScreenRect->Size.X) + RenderRect->X;
	*Y = (int32)((P.Y - ScreenRect->P.Y) * (real32)(RenderRect->H) / ScreenRect->Size.Y) + RenderRect->Y;
}

inline void
ConvertDistToPixelDist(rect2 *ScreenRect, real32 Dist, render_dim *RenderRect, int32 *PixelDist)
{
	*PixelDist = (int32)(Dist * (real32)(RenderRect->H - RenderRect->Y) / ScreenRect->Size.Y);
}


inline void
AddIntersection(v2 *Points, int32 *Num, real32 Radius, real32 Value, real32 Start, real32 End, bool32 RootIsPositive, bool32 ValueIsX)
{
	real32 InsideRoot = Square(Radius) - Square(Value);
	if (InsideRoot > 0.0f)
	{
		real32 Root = SqRt(InsideRoot);
		if (RootIsPositive)
		{
			if (Root >= Start && Root < End)
			{
				if (ValueIsX)
				{
					Points[(*Num)++] = v2{ Value,Root };
				}
				else
				{
					Points[(*Num)++] = v2{ Root,Value };
				}
			}
		}
		else
		{
			if (-Root >= Start && -Root < End)
			{
				if (ValueIsX)
				{
					Points[(*Num)++] = v2{ Value,-Root };
				}
				else
				{
					Points[(*Num)++] = v2{ -Root,Value };
				}
			}
		}
	}
}



internal void
ClipCircle(v2 *Points, int32 *Num, rect2 *Rect, real32 Radius)
{

	real32 MinX = Rect->P.X;
	real32 MinY = Rect->P.Y;
	real32 MaxX = Rect->P.X + Rect->Size.X;
	real32 MaxY = Rect->P.Y + Rect->Size.Y;

	uint32 Flags = ((MinX > 0.0f) << 3) + ((MinY > 0.0f) << 2) + ((MaxX < 0.0f) << 1) + ((MaxY < 0.0f) << 0);


	switch (Flags)
	{
		case 0: //Quadrants I,II,III & IV
		{

			AddIntersection(Points, Num, Radius, MaxX, MinY, MaxY, true, true);

			AddIntersection(Points, Num, Radius, MaxY, MinX, MaxX, true, false);

			AddIntersection(Points, Num, Radius, MaxY, MinX, MaxX, false, false);

			AddIntersection(Points, Num, Radius, MinX, MinY, MaxY, true, true);

			AddIntersection(Points, Num, Radius, MinX, MinY, MaxY, false, true);

			AddIntersection(Points, Num, Radius, MinY, MinX, MaxX, false, false);

			AddIntersection(Points, Num, Radius, MinY, MinX, MaxX, true, false);

			AddIntersection(Points, Num, Radius, MaxX, MinY, MaxY, false, true);


		}break;

		case 12: //Quadrant I
		{
			
			AddIntersection(Points, Num, Radius, MinY, MinX, MaxX, true, false);

			AddIntersection(Points, Num, Radius, MaxX, MinY, MaxY, true, true);

			AddIntersection(Points, Num, Radius, MaxY, MinX, MaxX, true, false);

			AddIntersection(Points, Num, Radius, MinX, MinY, MaxY, true, true);


		}break;

		case 6: //Quadrant II
		{
			AddIntersection(Points, Num, Radius, MaxX, MinY, MaxY, true, true);

			AddIntersection(Points, Num, Radius, MaxY, MinX, MaxX, false, false);

			AddIntersection(Points, Num, Radius, MinX, MinY, MaxY, true, true);

			AddIntersection(Points, Num, Radius, MinY, MinX, MaxX, false, false);

		}break;

		case 3: // Quadrant III
		{
			AddIntersection(Points, Num, Radius, MinX, MinY, MaxY, false, true);

			AddIntersection(Points, Num, Radius, MaxY, MinX, MaxX, false, false);

			AddIntersection(Points, Num, Radius, MaxX, MinY, MaxY, false, true);		

			AddIntersection(Points, Num, Radius, MinY, MinX, MaxX, false, false);
		

		}break;
	
		case 9: //Quadrant IV
		{
			AddIntersection(Points, Num, Radius, MinX, MinY, MaxY, false, true);

			AddIntersection(Points, Num, Radius, MinY, MinX, MaxX, true, false);

			AddIntersection(Points, Num, Radius, MaxX, MinY, MaxY, false, true);

			AddIntersection(Points, Num, Radius, MaxY, MinX, MaxX, true, false);

			

		}break;

		case 4: //Quadrants I,II
		{
			AddIntersection(Points, Num, Radius, MinY, MinX, MaxX, true, false);

			AddIntersection(Points, Num, Radius, MaxX, MinY, MaxY, true, true);

			AddIntersection(Points, Num, Radius, MaxY, MinX, MaxX, true, false);

			AddIntersection(Points, Num, Radius, MaxY, MinX, MaxX, false, false);

			AddIntersection(Points, Num, Radius, MinX, MinY, MaxY, true, true);

			AddIntersection(Points, Num, Radius, MinY, MinX, MaxX, false, false);


		}break;

		case 2: //Quadrants II,III
		{

			AddIntersection(Points, Num, Radius, MaxX, MinY, MaxY, true, true);

			AddIntersection(Points, Num, Radius, MaxY, MinX, MaxX, false, false);

			AddIntersection(Points, Num, Radius, MinX, MinY, MaxY, true, true);

			AddIntersection(Points, Num, Radius, MinX, MinY, MaxY, false, true);

			AddIntersection(Points, Num, Radius, MaxX, MinY, MaxY, false, true);

			AddIntersection(Points, Num, Radius, MinY, MinX, MaxX, false, false);
		
		}break;

		case 1: //Quadrants III,IV
		{
			AddIntersection(Points, Num, Radius, MinX, MinY, MaxY, false, true);

			AddIntersection(Points, Num, Radius, MaxY, MinX, MaxX, false, false);

			AddIntersection(Points, Num, Radius, MinY, MinX, MaxX, false, false);

			AddIntersection(Points, Num, Radius, MinY, MinX, MaxX, true, false);

			AddIntersection(Points, Num, Radius, MaxX, MinY, MaxY, false, true);

			AddIntersection(Points, Num, Radius, MaxY, MinX, MaxX, true, false);

		}break;

		case 8: //Quadrants IV,I
		{
			AddIntersection(Points, Num, Radius, MinX, MinY, MaxY, false, true);

			AddIntersection(Points, Num, Radius, MinY, MinX, MaxX, true, false);

			AddIntersection(Points, Num, Radius, MaxX, MinY, MaxY, false, true);

			AddIntersection(Points, Num, Radius, MaxX, MinY, MaxY, true, true);

			AddIntersection(Points, Num, Radius, MaxY, MinX, MaxX, true, false);

			AddIntersection(Points, Num, Radius, MinX, MinY, MaxY, true, true);

		}break;

	}
}



inline bool32
CirclesOverlap(v2 AP, real32 AR, v2 BP, real32 BR)
{
	bool32 Result = (DistanceSq(AP, BP) < Square(AR + BR));

	return Result;
}


inline bool32
CirclesOverlap(circle A, circle B)
{
	bool32 Result = (DistanceSq(A.P, B.P) < Square(A.R + B.R));
	
	return Result;
}

inline bool32
CirclesOverlap(circle Mover, v2 dP,circle Collider)
{
	bool32 Result = (DistanceSq(Mover.P + dP, Collider.P) < Square(Mover.R + Collider.R));

	return Result;
}

inline bool32
NegativeCircleOverlap(v2 PositiveP, real32 PositiveR, v2 NegativeP, real32 NegativeR)
{
	bool32 Result = (LengthSq(PositiveP - NegativeP) > Square(NegativeR - PositiveR));

	return Result;
}

inline bool32
NegativeCircleOverlap(circle Positive, circle Negative)
{
	bool32 Result = (LengthSq(Positive.P - Negative.P) > Square(Negative.R - Positive.R));

	return Result;
}

inline bool32
NegativeCircleOverlap(circle Positive, v2 dP, circle Negative)
{
	bool32 Result = (LengthSq(Positive.P + dP - Negative.P) > Square(Negative.R - Positive.R));

	return Result;
}

internal void
GetCircleIntersection(circle A, circle B, v2 *Intersect0, v2 *Intersect1, uint32 *Num)
{
	*Intersect0 = v2{ 0.0f,0.0f };
	*Intersect1 = *Intersect0;
	*Num = 0;

	real32 D = Distance(A.P, B.P);
	real32 X = (Square(A.R) + Square(D) - Square(B.R)) / (2.0f * D);
	real32 InsideRoot = Square(A.R) - Square(X);

	if (InsideRoot > 0.0f)
	{
		real32 Y = SqRt(InsideRoot);
		v2 IntersectOne = ComplexMult(v2{ X,Y }, B.P - A.P);
		v2 IntersectTwo = ComplexMult(v2{ X,-Y }, B.P - A.P);
		*Intersect0 = (IntersectOne / D) + A.P;
		*Intersect1 = (IntersectTwo / D) + A.P;
		*Num = 2;
	}
	else if (InsideRoot == 0.0f)
	{
		v2 Intersect = ComplexMult(v2{ X,0.0f }, B.P - A.P);
		*Intersect0 = (Intersect / D) + A.P;
		*Num = 1;
	}
}

inline v2
MoveCircleSafely(circle Mover, v2 dP, circle Collider)
{
	v2 Result = dP;

	if (CirclesOverlap(Mover, dP, Collider))
	{
		circle MovingPositions = { v2{ 0.0f },Length(dP) };
		Collider.P = Collider.P -  Mover.P;
		Collider.R = Collider.R + Mover.R;
		v2 FirstIntersect, SecondIntersect;
		uint32 Num;
		GetCircleIntersection(MovingPositions, Collider, &FirstIntersect, &SecondIntersect, &Num);
		if (Num == 2)
		{
			if (DistanceSq(FirstIntersect, dP) < DistanceSq(SecondIntersect, dP))
			{
				Result = FirstIntersect;
			}
			else
			{
				Result = SecondIntersect;
			}
		}
	}

	return Result;
}

inline v2
MoveCircleSafelyNegative(circle Mover, v2 dP, circle Collider)
{
	v2 Result = dP;

	if (NegativeCircleOverlap(Mover, dP, Collider))
	{
		circle MovingPositions = { Mover.P - Collider.P,Length(dP) };
		Collider.P = v2{ 0.0f,0.0f };
		Collider.R -= Mover.R;
		v2 FirstIntersect, SecondIntersect;
		uint32 Num;
		GetCircleIntersection(MovingPositions, Collider, &FirstIntersect, &SecondIntersect, &Num);
		if (Num == 2)
		{
			if (DistanceSq(FirstIntersect, dP) < DistanceSq(SecondIntersect, dP))
			{
				Result = FirstIntersect - MovingPositions.P;
			}
			else
			{
				Result = SecondIntersect - MovingPositions.P;
			}
		}
	}

	return Result;
}


internal real32
OuterCircleCircleCollision(circle Mover, v2 dP, circle Collider)
{
	//TODO(ian): Maybe allow a Zero dP to account for some other circle pushing a stationary circle
	real32 G = 1.0f;

	v2 CollidingCircleP = Collider.P - Mover.P;
	real32 CombinedRadius = Mover.R + Collider.R;

	if (DistanceSq(dP, CollidingCircleP) <= Square(CombinedRadius))
	{
		real32 A = LengthSq(dP);
		real32 B = -2.0f*Inner(dP, CollidingCircleP);
		real32 C = LengthSq(CollidingCircleP) - Square(CombinedRadius);
		real32 InsideRoot = Square(B) - 4.0f * A * C;
		if (InsideRoot > 0.0f)
		{
			real32 Root = SqRt(InsideRoot);
			real32 FirstG = (-B + Root) / (2.0f * A);
			real32 SecondG = (-B - Root) / (2.0f * A);
			G = FirstG < SecondG ? FirstG : SecondG;
		}
		else if (InsideRoot == 0.0f)
		{
			G = -B / (2.0f * A);
		}

		//Assert(G >= 0.0f && G <= 1.0f);

		dP = G * dP;
	}

	return G;
}

internal real32
InnerCircleCircleCollision(circle Mover, v2 dP, circle Collider)
{

	//TODO(ian): Maybe allow a Zero dP to account for some other circle pushing a stationary circle
	real32 G = 1.0f;

	v2 InnerCircleP = Mover.P - Collider.P;
	real32 CombinedRadius = Collider.R - Mover.R;
	if (LengthSq(InnerCircleP + dP) >= CombinedRadius)
	{
		real32 A = LengthSq(dP);
		real32 B = 2.0f * Inner(dP, InnerCircleP);
		real32 C = LengthSq(InnerCircleP) - Square(CombinedRadius);
		real32 InsideRoot = Square(B) - 4.0f * A * C;
		if (InsideRoot > 0.0f)
		{
			real32 Root = SqRt(InsideRoot);
			real32 FirstG = (-B + Root) / (2.0f * A);
			real32 SecondG = (-B - Root) / (2.0f * A);
			G = FirstG > SecondG ? FirstG : SecondG;
		}
		else if (InsideRoot == 0.0f)
		{
			G = -B / (2.0f * A);
		}

		//Assert(G >= 0.0f && G <= 1.0f);

		dP = G * dP;
	}

	return G;
}


	

enum circle_rect_filter
{
	NOT_IN_RECT,
	WHOLLY_WITHIN_RECT,
	NEEDS_CLIPPING
};

internal circle_rect_filter
FilterCircle(rect2 *Rect, circle Circle)
{
	circle_rect_filter Result = NOT_IN_RECT;

	rect2 CircleRect = CircleExtent(Circle);
	if (Intersects(Rect, &CircleRect))
	{

		rect2 Intersection = GetIntersection(Rect, &CircleRect);
		if (Intersection.P.X == CircleRect.P.X &&
			Intersection.P.Y == CircleRect.P.Y &&
			Intersection.Size.X == CircleRect.Size.X &&
			Intersection.Size.Y == CircleRect.Size.Y)
		{
			Result = WHOLLY_WITHIN_RECT;
		}
		else
		{

			//TODO(ian): Replace this code with a function to compute whether two circles are intersecting?
			real32 SquareDist = Length(Rect->Size * 0.5f);
			real32 FromCenterSq = LengthSq(Rect->P + (0.5f * Rect->Size) - Circle.P);
			if (((Square(Circle.R - SquareDist) <= FromCenterSq) && Square(Circle.R + SquareDist) > FromCenterSq) || Circle.R < SquareDist)
			{
					Result = NEEDS_CLIPPING;
			}
		}
	}


	return Result;
}

inline bool32
EqualPixelColors(uint8 *Pixels, int32 Pitch, int32 X, int32 Y,
	uint8 R, uint8 G, uint8 B, uint8 A)
{
	bool32 Result = false;
	int32 PixelIndex = Y * Pitch + (4 * X);
	if (*(Pixels + PixelIndex + 0) == R &&
		*(Pixels + PixelIndex + 1) == G &&
		*(Pixels + PixelIndex + 2) == B &&
		*(Pixels + PixelIndex + 3) == A)
	{
		Result = true;
	}
	return Result;
}

internal void
PseudoFloodFill(uint8 *Pixels, int32 Pitch, render_dim *RenderRect, int32 X, int32 Y,
	uint8 R, uint8 G, uint8 B, uint8 A, uint8 bR, uint8 bG, uint8 bB, uint8 bA)
{
	int32 CurrentY = Y;

	while (CurrentY >= RenderRect->Y)
	{
		if (EqualPixelColors(Pixels, Pitch, X, CurrentY, bR, bG, bB, bA))
		{
			break;
		}

		for (int32 LeftX = X;
			LeftX >= RenderRect->X &&
			!EqualPixelColors(Pixels, Pitch, LeftX, CurrentY, bR, bG, bB, bA);
			--LeftX)
		{
			SetPixel(Pixels, Pitch, LeftX, CurrentY, R, G, B, A);
		}
			
		for (int32 RightX = X+1;
			RightX < RenderRect->W &&
			!EqualPixelColors(Pixels, Pitch, RightX, CurrentY, bR, bG, bB, bA);
			++RightX)
		{
			SetPixel(Pixels, Pitch, RightX, CurrentY, R, G, B, A);
		}

		--CurrentY;
	}


	CurrentY = Y + 1;

	while (CurrentY < RenderRect->H)
	{
		if (EqualPixelColors(Pixels, Pitch, X, CurrentY, bR, bG, bB, bA))
		{
			break;
		}

		for (int32 LeftX = X;
			LeftX >= RenderRect->X &&
			!EqualPixelColors(Pixels, Pitch, LeftX, CurrentY, bR, bG, bB, bA);
			--LeftX)
		{
			SetPixel(Pixels, Pitch, LeftX, CurrentY, R, G, B, A);
		}

		for (int32 RightX = X+1;
			RightX < RenderRect->W &&
			!EqualPixelColors(Pixels, Pitch, RightX, CurrentY, bR, bG, bB, bA);
			++RightX)
		{
			SetPixel(Pixels, Pitch, RightX, CurrentY, R, G, B, A);
		}

		++CurrentY;
	}
}

internal void 
RasterizeLine(uint8 *Pixels, int32 Pitch, int32 X0, int32 Y0, int32 X1, int32 Y1,
	uint8 R, uint8 G, uint8 B, uint8 A)
{
	int32 dX = Abs(X1 - X0);
	int32 dY = -Abs(Y1 - Y0);
	int32 sX = X0<X1 ? 1 : -1;
	int32 sY = Y0<Y1 ? 1 : -1;
	int32 Error = dX + dY;
	int32 Error2; /* error value e_xy */
	int32 StartX = X0;
	int32 StartY = Y0;

	while(true)
	{
		SetPixel(Pixels, Pitch, X0, Y0, R, G, B, A);
		if (X0 == X1 && Y0 == Y1) break;

		Error2 = 2 * Error;

		if (Error2 >= dY) /* e_xy+e_x > 0 */
		{ 
			Error += dY; 
			X0 += sX; 
		} 

		if (Error2 <= dX) /* e_xy+e_y < 0 */
		{ 
			Error += dX; 
			Y0 += sY; 
		} 
	}
}

internal void
ClipAndDrawLine(uint8 *Pixels, int32 Pitch, render_dim *RenderRect, rect2 *ScreenRect,
	v2 Start, v2 End, uint8 R, uint8 G, uint8 B, uint8 A)
{

	ClipLine(ScreenRect, &Start, &End);

	int32 X0, Y0;
	ConvertCoordToPixelSpace(ScreenRect, Start, RenderRect, &X0, &Y0);
	X0 = Clip(RenderRect->X, X0, RenderRect->W + RenderRect->X - 1);
	Y0 = Clip(RenderRect->Y, Y0, RenderRect->H + RenderRect->Y - 1);

	int32 X1, Y1;
	ConvertCoordToPixelSpace(ScreenRect, End, RenderRect, &X1, &Y1);
	X1 = Clip(RenderRect->X, X1, RenderRect->W + RenderRect->X - 1);
	Y1 = Clip(RenderRect->Y, Y1, RenderRect->H + RenderRect->Y - 1);

	RasterizeLine(Pixels, Pitch, X0, Y0, X1, Y1, R, G, B, A);
}


internal void
RasterizeCircle(uint8* Pixels, int32 Pitch, int64 mX, int64 mY, int64 Radius, 
	uint8 R, uint8 G, uint8 B, uint8 A)
{
	int64 X = -Radius;
	int64 Y = 0;
	int64 Error = 2 - 2 * Radius;
	int64 Error2;
	do
	{
		SetPixel(Pixels, Pitch, mX - X, mY + Y, R, G, B, A); //I.Quadrant
		SetPixel(Pixels, Pitch, mX - Y, mY - X, R, G, B, A); //II.Quadrant
		SetPixel(Pixels, Pitch, mX + X, mY - Y, R, G, B, A); //III.Quadrant
		SetPixel(Pixels, Pitch, mX + Y, mY + X, R, G, B, A); //IV.Quadrant
		Error2 = Error;
		if (Error2 <= Y) Error += ++Y * 2 + 1;
		if (Error2 > X || Error > Y) Error += ++X * 2 + 1;
	} while (X < 0);
}



internal void
RasterizeCircleSafe(uint8* Pixels, int32 Pitch, render_dim *RenderRect, int64 mX, int64 mY, int64 Radius,
	uint8 R, uint8 G, uint8 B, uint8 A)
{
	int64 X = -Radius;
	int64 Y = 0;
	int64 Error = 2 - 2 * Radius;
	int64 Error2;
	do
	{
		SetPixelSafe(Pixels, Pitch, RenderRect, mX - X, mY + Y, R, G, B, A); //I.Quadrant
		SetPixelSafe(Pixels, Pitch, RenderRect, mX - Y, mY - X, R, G, B, A); //II.Quadrant
		SetPixelSafe(Pixels, Pitch, RenderRect, mX + X, mY - Y, R, G, B, A); //III.Quadrant
		SetPixelSafe(Pixels, Pitch, RenderRect, mX + Y, mY + X, R, G, B, A); //IV.Quadrant
		Error2 = Error;
		if (Error2 <= Y) Error += ++Y * 2 + 1;
		if (Error2 > X || Error > Y) Error += ++X * 2 + 1;
	} while (X < 0);
}

inline void
RenderCircle(uint8 *Pixels, int32 Pitch, render_dim *RenderRect, rect2 *Rect, circle Circle,
	uint8 R, uint8 G, uint8 B, uint8 A)
{
	int64 CX, CY;
	ConvertCoordToPixelSpace(Rect, Circle.P, RenderRect, &CX, &CY);
	int64 PixelRadius;
	ConvertDistToPixelDist(Rect, Circle.R, RenderRect, &PixelRadius);

	RasterizeCircle(Pixels, Pitch, CX, CY, PixelRadius, R, G, B, A);

}

inline void
RenderCircleSafe(uint8 *Pixels, int32 Pitch, render_dim *RenderRect, rect2 *Rect, circle Circle,
	uint8 R, uint8 G, uint8 B, uint8 A)
{
	int64 CX, CY;
	ConvertCoordToPixelSpace(Rect, Circle.P, RenderRect, &CX, &CY);
	int64 PixelRadius;
	ConvertDistToPixelDist(Rect, Circle.R, RenderRect, &PixelRadius);

	RasterizeCircleSafe(Pixels, Pitch, RenderRect, CX, CY, PixelRadius, R, G, B, A);

}

internal void
RasterizeCircleWithOct(uint8 *Pixels, int32 Pitch, render_dim *RenderRect, int64 X, int64 Y, int64 Radius,
	uint8 R, uint8 G, uint8 B, uint8 A)
{
	int64 SinY, Cos45;
	int64 SquareRadius, SquareY;
	int64 NewSquareY, DeltaY;

	Cos45 = (Radius * 185363) >> 18;
	SinY = Radius;

	SquareRadius = SquareY = SinY * SinY;
	DeltaY = (2 * SinY) - 1;
	NewSquareY = SquareRadius + 3;

	for (int64 CosX = 0;
		CosX <= Cos45 + 1;
		++CosX)
	{
		NewSquareY -= (2 * CosX) - 3;

		if ((SquareY - NewSquareY) >= DeltaY)
		{
			SquareY -= DeltaY;
			SinY -= 1;
			DeltaY -= 2;
		}

		SetPixel(Pixels, Pitch, CosX + X, SinY + Y, R, G, B, A);
		SetPixel(Pixels, Pitch, CosX + X, -SinY + Y, R, G, B, A);
		SetPixel(Pixels, Pitch, -CosX + X, SinY + Y, R, G, B, A);
		SetPixel(Pixels, Pitch, -CosX + X, -SinY + Y, R, G, B, A);

		SetPixel(Pixels, Pitch, SinY + X, CosX + Y, R, G, B, A);
		SetPixel(Pixels, Pitch, SinY + X, -CosX + Y, R, G, B, A);
		SetPixel(Pixels, Pitch, -SinY + X, CosX + Y, R, G, B, A);
		SetPixel(Pixels, Pitch, -SinY + X, -CosX + Y, R, G, B, A);
	}
}


/*
internal void
RasterizeCircleQuadrant(uint8 *Pixels, int32 Pitch, int64 mX, int64 mY, int64 Radius, int32 Quad,
	uint8 R, uint8 G, uint8 B, uint8 A)
{
	switch (Quad)
	{
		case 1:
		{
			int64 X = -Radius;
			int64 Y = 0;
			int64 Error = 2 - 2 * Radius;
			int64 Error2;
			do
			{
				SetPixel(Pixels, Pitch, mX - X, mY + Y, R, G, B, A); //I.Quadrant
				Error2 = Error;
				if (Error2 <= Y) Error += ++Y * 2 + 1;
				if (Error2 > X || Error > Y) Error += ++X * 2 + 1;
			} while (X < 0);
		}break;

		case 2:
		{
			int64 X = -Radius;
			int64 Y = 0;
			int64 Error = 2 - 2 * Radius;
			int64 Error2;
			do
			{
				SetPixel(Pixels, Pitch, mX - Y, mY - X, R, G, B, A); //II.Quadrant
				Error2 = Error;
				if (Error2 <= Y) Error += ++Y * 2 + 1;
				if (Error2 > X || Error > Y) Error += ++X * 2 + 1;
			} while (X < 0);
		}break;

		case 3:
		{
			int64 X = -Radius;
			int64 Y = 0;
			int64 Error = 2 - 2 * Radius;
			int64 Error2;
			do
			{
				SetPixel(Pixels, Pitch, mX + X, mY - Y, R, G, B, A); //III.Quadrant
				Error2 = Error;
				if (Error2 <= Y) Error += ++Y * 2 + 1;
				if (Error2 > X || Error > Y) Error += ++X * 2 + 1;
			} while (X < 0);
		}break;

		case 4:
		{
			int64 X = -Radius;
			int64 Y = 0;
			int64 Error = 2 - 2 * Radius;
			int64 Error2;
			do
			{
				SetPixel(Pixels, Pitch, mX + Y, mY + X, R, G, B, A); //IV.Quadrant
				Error2 = Error;
				if (Error2 <= Y) Error += ++Y * 2 + 1;
				if (Error2 > X || Error > Y) Error += ++X * 2 + 1;
			} while (X < 0);
		}break;
	}
}
*/

internal void
RasterizeCircleSegment(uint8 *Pixels, int32 Pitch, int64 mX, int64 mY, int64 Radius, int32 Quad,
	int64 StartX, int64 StartY, int64 EndX, int64 EndY, uint8 R, uint8 G, uint8 B, uint8 A)
{

	//TODO(ian): Think of a more robust system for drawing circle segments
	//The problem now is the tradeoff between drawing inside the texture and 
	//drawing a complete segment (both of which are bad!!!!)

	int64 BeginX = StartX - mX;
	int64 BeginY = StartY - mY;
	int64 FinalX = EndX - mX;
	int64 FinalY = EndY - mY;

	switch (Quad)
	{
		case 1:
		{
			int64 X = -BeginX;
			int64 Y = BeginY;
			int64 Error = Square(X + 1) + Square(Y + 1) - Square(Radius);
			int64 Error2;
			do
			{
				SetPixel(Pixels, Pitch, mX - X, mY + Y, R, G, B, A); //I.Quadrant
				Error2 = Error;
				if (Error2 <= Y) Error += ++Y * 2 + 1;
				if (Error2 > X || Error > Y) Error += ++X * 2 + 1;
			} while (X < -FinalX && Y < FinalY);
			int64 NewX = Clip(EndX, mX - X, StartX);
			int64 NewY = Clip(StartY, mY + Y, EndY);
			RasterizeLine(Pixels, Pitch, NewX, NewY, EndX, EndY, R, G, B, A);

		}break;

		case 2:
		{
			int64 X = -BeginY;
			int64 Y = -BeginX;
			int64 Error = Square(X + 1) + Square(Y + 1) - Square(Radius);
			int64 Error2;
			do
			{
				SetPixel(Pixels, Pitch, mX - Y, mY - X, R, G, B, A); //II.Quadrant
				Error2 = Error;
				if (Error2 <= Y) Error += ++Y * 2 + 1;
				if (Error2 > X || Error > Y) Error += ++X * 2 + 1;
			} while (X < -FinalY && Y < -FinalX);

			int64 NewX = Clip(EndX, mX - Y, StartX);
			int64 NewY = Clip(EndY, mY - X, StartY);
			RasterizeLine(Pixels, Pitch, NewX, NewY, EndX, EndY, R, G, B, A);
		}break;

		case 3:
		{
			int64 X = BeginX;
			int64 Y = -BeginY;
			int64 Error = Square(X + 1) + Square(Y + 1) - Square(Radius);
			int64 Error2;
			do
			{
				SetPixel(Pixels, Pitch, mX + X, mY - Y, R, G, B, A); //III.Quadrant
				Error2 = Error;
				if (Error2 <= Y) Error += ++Y * 2 + 1;
				if (Error2 > X || Error > Y) Error += ++X * 2 + 1;
			} while (X < FinalX && Y < -FinalY);

			int64 NewX = Clip(StartX, mX + X, EndX);
			int64 NewY = Clip(EndY, mY - Y, StartY);
			RasterizeLine(Pixels, Pitch, NewX, NewY, EndX, EndY, R, G, B, A);
		}break;

		case 4:
		{
			int64 X = BeginY;
			int64 Y = BeginX;
			int64 Error = Square(X + 1) + Square(Y + 1) - Square(Radius);
			int64 Error2;
			do
			{
				SetPixel(Pixels, Pitch, mX + Y, mY + X, R, G, B, A); //IV.Quadrant
				Error2 = Error;
				if (Error2 <= Y) Error += ++Y * 2 + 1;
				if (Error2 > X || Error > Y) Error += ++X * 2 + 1;
			} while (X < FinalY && Y < FinalX);

			int64 NewX = Clip(StartX, mX + Y, EndX);
			int64 NewY = Clip(StartY, mY + X, EndY);
			RasterizeLine(Pixels, Pitch, NewX, NewY, EndX, EndY, R, G, B, A);
		}break;
	}
}


inline void
RenderCircleSegment(uint8 *Pixels, int32 Pitch, render_dim *RenderRect, rect2 *Rect, v2 CircleP, real32 Radius,
	v2 Start, v2 End, int32 Quad, uint8 R, uint8 G, uint8 B, uint8 A)
{
	int64 CX, CY;
	ConvertCoordToPixelSpace(Rect, CircleP, RenderRect, &CX, &CY);
	int64 AX, AY;
	ConvertCoordToPixelSpace(Rect, Start, RenderRect, &AX, &AY);
	int64 X0 = Clip((int64)RenderRect->X, AX, (int64)(RenderRect->W + RenderRect->X) - 1);
	int64 Y0 = Clip((int64)RenderRect->Y, AY, (int64)(RenderRect->H + RenderRect->Y) - 1);
	int64 BX, BY;
	ConvertCoordToPixelSpace(Rect, End, RenderRect, &BX, &BY);
	int64 X1 = Clip((int64)RenderRect->X, BX, (int64)(RenderRect->W + RenderRect->X) - 1);
	int64 Y1 = Clip((int64)RenderRect->Y, BY, (int64)(RenderRect->H + RenderRect->Y) - 1);
	int64 PixelRadius;
	ConvertDistToPixelDist(Rect, Radius, RenderRect, &PixelRadius);

	RasterizeCircleSegment(Pixels, Pitch, CX, CY, PixelRadius, Quad, X0, Y0, X1, Y1, R, G, B, A);

}

/*
internal void
RasterizeDisc(uint8 *Pixels, int32 Pitch, render_dim *RenderRect, int32 X, int32 Y, int32 Radius,
	uint8 R, uint8 G, uint8 B, uint8 A)
{
	for (int32 CircleY = -Radius;
		CircleY <= Radius;
		++CircleY)
	{
		for (int32 CircleX = -Radius;
			CircleX <= Radius;
			++CircleX)
		{
			if ((CircleX * CircleX) + (CircleY * CircleY) <= (Radius * Radius))
			{
				SetPixel(Pixels, Pitch, CircleX + X, CircleY + Y, R, G, B, A);
			}
		}
	}
}
*/


internal void
RasterizeDiscSafe(uint8 *Pixels, int32 Pitch, render_dim *RenderRect, int32 X, int32 Y, int32 Radius,
	uint8 R, uint8 G, uint8 B, uint8 A)
{
	for (int32 CircleY = -Radius;
		CircleY <= Radius;
		++CircleY)
	{
		for (int32 CircleX = -Radius;
			CircleX <= Radius;
			++CircleX)
		{
			if ((CircleX * CircleX) + (CircleY * CircleY) <= (Radius * Radius))
			{
				SetPixelSafe(Pixels, Pitch, RenderRect, CircleX + X, CircleY + Y, R, G, B, A);
			}
		}
	}
}

internal void
RenderDiscSafe(uint8 *Pixels, int32 Pitch, render_dim *RenderRect, rect2 *ScreenRect,
	v2 CircleP, real32 Radius, uint8 R, uint8 G, uint8 B, uint8 A)
{

	int32 X, Y;
	ConvertCoordToPixelSpace(ScreenRect, CircleP, RenderRect, &X, &Y);
	int32 PixelRadius;
	ConvertDistToPixelDist(ScreenRect, Radius, RenderRect, &PixelRadius);

	RasterizeDiscSafe(Pixels, Pitch, RenderRect, X, Y, PixelRadius,
		R, G, B, A);
}

internal void
DrawCircleRangeRaw(uint8*Pixels, int32 Pitch, render_dim *RenderRect,
	rect2 *Rect, v2 Point0, v2 Point1, real32 Radius, v2 *Cross, real32 AngleShift,
	uint8 R, uint8 G, uint8 B, uint8 A)
{
	v2 Start = Point0;
	v2 End = Point1;
	int32 StartQuad = GetQuadrant(Start);
	int32 EndQuad = GetQuadrant(End);

	v2 CurrentPoint = Start;
	int32 CurrentQuad = StartQuad;

	while (true)
	{
		v2 NextPoint;
		if (CurrentQuad != EndQuad || AngleShift >= TAU32 * 0.75)
		{
			NextPoint = Cross[CurrentQuad - 1] * Radius;
			RenderCircleSegment(Pixels, Pitch, RenderRect,
				Rect, v2{ 0.0f,0.0f }, Radius, CurrentPoint, NextPoint, CurrentQuad, R, G, B, A);
			AngleShift = 0.0f;
		}
		else
		{
			RenderCircleSegment(Pixels, Pitch, RenderRect,
				Rect, v2{ 0.0f,0.0f }, Radius, CurrentPoint, End, CurrentQuad, R, G, B, A);
			break;
		}
		CurrentPoint = NextPoint;
		CurrentQuad = (CurrentQuad & 3) + 1;
	}
}

inline void
DrawCircleRange(uint8*Pixels, int32 Pitch, render_dim *RenderRect,
	rect2 *ScreenRect, v2 Point0, v2 Point1, circle Circle,
	uint8 R, uint8 G, uint8 B, uint8 A)
{
	rect2 Rect = *ScreenRect;
	Rect.P = Rect.P - Circle.P;
	Point0 = Point0 - Circle.P;
	Point1 = Point1 - Circle.P;

	real32 FirstAngle = GetAngle(Point0);
	real32 SecondAngle = GetAngle(Point1);
	real32 AngleShift = SecondAngle - FirstAngle;
	AngleShift = (AngleShift < 0.0f ? AngleShift + TAU32 : AngleShift);

	v2 Cross[4];
	Cross[0] = v2{ 0.0f, 1.0f };
	Cross[1] = v2{ -1.0f, 0.0f };
	Cross[2] = v2{ 0.0f, -1.0f };
	Cross[3] = v2{ 1.0f, 0.0f };

	DrawCircleRangeRaw(Pixels, Pitch, RenderRect,
		&Rect, Point0, Point1, Circle.R, Cross, AngleShift, R, G, B, A);
}

internal void
ClipAndDrawCircle(uint8 *Pixels, int32 Pitch, render_dim *RenderRect,
	rect2 *ScreenRect, circle Circle, uint8 R, uint8 G, uint8 B, uint8 A)
{
	switch (FilterCircle(ScreenRect, Circle))
	{
		case NOT_IN_RECT:
		{
			//NOTE(ian): Do nothing
		}break;
		case WHOLLY_WITHIN_RECT:
		{
			RenderCircleSafe(Pixels, Pitch, RenderRect, ScreenRect, Circle, R, G, B, A);
		}break;
		case NEEDS_CLIPPING:
		{
			rect2 Rect = *ScreenRect;
			Rect.P = (ScreenRect->P - Circle.P);


			v2 Points[8];
			int32 Num = 0;

			ClipCircle(Points, &Num, &Rect, Circle.R);

			if (Num > 1)
			{
				real32 FirstAngle = GetAngle(Points[0]);
				real32 SecondAngle = GetAngle(Points[1]);
				real32 AngleShift = SecondAngle - FirstAngle;
				AngleShift = (AngleShift < 0.0f ? AngleShift + TAU32 : AngleShift);
				real32 Angle = FirstAngle + AngleShift*0.5;
				v2 NewPoint = { Cosine(Angle),Sine(Angle) };
				NewPoint = NewPoint * Circle.R;

				int32 StartIndex;
				if (InsideRect(&Rect, NewPoint))
				{
					StartIndex = 0;
				}
				else
				{
					StartIndex = 1;
				}



				v2 Cross[4];
				Cross[0] = v2{ 0.0f, 1.0f };
				Cross[1] = v2{ -1.0f, 0.0f };
				Cross[2] = v2{ 0.0f, -1.0f };
				Cross[3] = v2{ 1.0f, 0.0f };
				for (int32 Index = StartIndex;
					Index < Num;
					Index += 2)
				{
					DrawCircleRangeRaw(Pixels, Pitch, RenderRect,
						&Rect, Points[Index], Points[(Index + 1) % Num], Circle.R, Cross, AngleShift, R, G, B, A);
				}

			}
		}break;
		default:
		{
			//NOTE(ian): ERROR
		}break;
	}
}

internal void
ClipAndDrawCroppedCircle(uint8 *Pixels, int32 Pitch, render_dim *RenderRect,
	rect2 *ScreenRect, circle Circle, v2 Point0, v2 Point1, uint8 R, uint8 G, uint8 B, uint8 A)
{
	switch (FilterCircle(ScreenRect, Circle))
	{
		case NOT_IN_RECT:
		{
			//NOTE(ian): Do nothing
		}break;
		case WHOLLY_WITHIN_RECT:
		{
			DrawCircleRange(Pixels, Pitch, RenderRect, ScreenRect, Point0, Point1, Circle, R, G, B, A);
		}break;
		case NEEDS_CLIPPING:
		{
			rect2 Rect = *ScreenRect;
			Rect.P = (ScreenRect->P - Circle.P);
			Point0 = Point0 - Circle.P;
			Point1 = Point1 - Circle.P;


			v2 Points[8];
			int32 Num = 0;

			ClipCircle(Points, &Num, &Rect, Circle.R);

			if (Num > 1)
			{
				real32 FirstAngle = GetAngle(Points[0]);
				real32 SecondAngle = GetAngle(Points[1]);
				real32 AngleShift = SecondAngle - FirstAngle;
				AngleShift = (AngleShift < 0.0f ? AngleShift + TAU32 : AngleShift);
				real32 Angle = FirstAngle + AngleShift*0.5;
				v2 NewPoint = { Cosine(Angle),Sine(Angle) };
				NewPoint = NewPoint * Circle.R;

				int32 StartIndex;
				if (InsideRect(&Rect, NewPoint))
				{
					StartIndex = 0;
				}
				else
				{
					StartIndex = 1;
				}


				v2 Cross[4];
				Cross[0] = v2{ 0.0f, 1.0f };
				Cross[1] = v2{ -1.0f, 0.0f };
				Cross[2] = v2{ 0.0f, -1.0f };
				Cross[3] = v2{ 1.0f, 0.0f };

				real32 Point0Angle = GetAngle(Point0);
				real32 Point1Angle = GetAngle(Point1);
				real32 TotalAngle = Point1Angle - Point0Angle;


				for (int32 Index = StartIndex;
					Index < Num;
					Index += 2)
				{

					v2 Start = Points[Index];
					v2 End = Points[(Index + 1) % Num];

					real32 StartAngle = GetAngle(Start);
					real32 EndAngle = GetAngle(End);

					bool32 StartInRange;
					bool32 EndInRange;

					if (TotalAngle > 0.0f)
					{
						StartInRange = (StartAngle > Point0Angle && StartAngle < Point1Angle);

						EndInRange = (EndAngle > Point0Angle && EndAngle < Point1Angle);
					}
					else
					{
						StartInRange = !(StartAngle > Point1Angle && StartAngle < Point0Angle);

						EndInRange = !(EndAngle > Point1Angle && EndAngle < Point0Angle);
					}

					if (StartInRange && EndInRange)
					{

						bool32 Point0InRange;
						bool32 Point1InRange;

						if (EndAngle - StartAngle > 0.0f)
						{
							Point0InRange = (Point0Angle > StartAngle && Point0Angle < EndAngle);
							Point1InRange = (Point1Angle > StartAngle && Point1Angle < EndAngle);
						}
						else
						{
							Point0InRange = !(Point0Angle > EndAngle && Point0Angle < StartAngle);
							Point1InRange = !(Point1Angle > EndAngle && Point1Angle < StartAngle);
						}

						if (Point0InRange && Point1InRange)
						{
							DrawCircleRangeRaw(Pixels, Pitch, RenderRect,
								&Rect, Start, Point1, Circle.R, Cross, AngleShift, R, G, B, A);
							DrawCircleRangeRaw(Pixels, Pitch, RenderRect,
								&Rect, Point0, End, Circle.R, Cross, AngleShift, R, G, B, A);
						}
						else
						{
							DrawCircleRangeRaw(Pixels, Pitch, RenderRect,
								&Rect, Start, End, Circle.R, Cross, AngleShift, R, G, B, A);

						}
					}
					else if (StartInRange)
					{
						bool32 Point0InRange;

						if (EndAngle - StartAngle > 0.0f)
						{
							Point0InRange = (Point0Angle > StartAngle && Point0Angle < EndAngle);
						}
						else
						{
							Point0InRange = !(Point0Angle > EndAngle && Point0Angle < StartAngle);
						}

						if (Point0InRange)
						{
							DrawCircleRangeRaw(Pixels, Pitch, RenderRect,
								&Rect, Start, Point0, Circle.R, Cross, AngleShift, R, G, B, A);
						}
						else
						{
							DrawCircleRangeRaw(Pixels, Pitch, RenderRect,
								&Rect, Start, Point1, Circle.R, Cross, AngleShift, R, G, B, A);
						}
					}
					else if (EndInRange)
					{
						bool32 Point1InRange;

						if (EndAngle - StartAngle > 0.0f)
						{
							Point1InRange = (Point1Angle > StartAngle && Point1Angle < EndAngle);
						}
						else
						{
							Point1InRange = !(Point1Angle > EndAngle && Point1Angle < StartAngle);
						}

						if (Point1InRange)
						{
							DrawCircleRangeRaw(Pixels, Pitch, RenderRect,
								&Rect, Point1, End, Circle.R, Cross, AngleShift, R, G, B, A);
						}
						else
						{
							DrawCircleRangeRaw(Pixels, Pitch, RenderRect,
								&Rect, Point0, End, Circle.R, Cross, AngleShift, R, G, B, A);
						}
					}
					
				}
			}
		}break;
		default:
		{
			//NOTE(ian): ERROR
		}break;
	}
}

internal void
ClipAndDrawDisc(uint8 *Pixels, int32 Pitch, render_dim *RenderRect,
	rect2 *ScreenRect, circle Circle, uint8 R, uint8 G, uint8 B, uint8 A)
{
	rect2 CircleRect = CircleExtent(Circle);

	if (Intersects(ScreenRect, &CircleRect))
	{
		rect2 Intersection = GetIntersection(ScreenRect, &CircleRect);

		int64 MinX, MinY;
		ConvertCoordToPixelSpace(ScreenRect, Intersection.P, RenderRect, &MinX, &MinY);
		if (MinX < RenderRect->X)
		{
			MinX = RenderRect->X;
		}
		if (MinY < RenderRect->Y)
		{
			MinY = RenderRect->Y;
		}

		int64 MaxX, MaxY;
		ConvertCoordToPixelSpace(ScreenRect, Intersection.P + Intersection.Size, RenderRect, &MaxX, &MaxY);
		if (MaxX > RenderRect->X + RenderRect->W - 1)
		{
			MaxX = RenderRect->X + RenderRect->W - 1;
		}
		if (MaxY > RenderRect->Y + RenderRect->H - 1)
		{
			MaxY = RenderRect->Y + RenderRect->H - 1;
		}

		int64 CX, CY;
		ConvertCoordToPixelSpace(ScreenRect, Circle.P, RenderRect, &CX, &CY);

		int64 PixelRadius;
		ConvertDistToPixelDist(ScreenRect, Circle.R, RenderRect, &PixelRadius);

		for (int64 CircleY = MinY-CY;
			CircleY <= MaxY-CY;
			++CircleY)
		{
			for (int64 CircleX = MinX-CX;
				CircleX <= MaxX-CX;
				++CircleX)
			{
				if ((CircleX * CircleX) + (CircleY * CircleY) <= (PixelRadius * PixelRadius))
				{
					SetPixel(Pixels, Pitch, CircleX + CX, CircleY + CY, R, G, B, A);
				}
			}
		}
	}
}

internal void
DrawGridLines(uint8 *Pixels, int32 Pitch, render_dim *RenderRect, rect2 *CameraRect,
	uint8 R, uint8 G, uint8 B, uint8 A)
{
	for (real32 GridIndex = (CameraRect->X >= 0.0f ? ceilf(CameraRect->X) : -floorf(-CameraRect->X));
		GridIndex < CameraRect->X + CameraRect->W;
		GridIndex += 1.0f)
	{
		v2 Start = v2{ -1.0f,-1.0f };
		Start.X = GridIndex;
		Start.Y = CameraRect->Y;
		v2 End;
		End.X = GridIndex;
		End.Y = CameraRect->Y + CameraRect->H;
		ClipAndDrawLine(Pixels, Pitch, RenderRect, CameraRect,
			Start, End, R, G, B, A);
	}

	for (real32 GridIndex = (CameraRect->Y >= 0.0f ? ceilf(CameraRect->Y) : -floorf(-CameraRect->Y));
		GridIndex < CameraRect->Y + CameraRect->H;
		GridIndex += 1.0f)
	{
		v2 Start;
		Start.X = CameraRect->X;
		Start.Y = GridIndex;
		v2 End;
		End.X = CameraRect->X + CameraRect->W;
		End.Y = GridIndex;
		ClipAndDrawLine(Pixels, Pitch, RenderRect, CameraRect,
			Start, End, R, G, B, A);
	}
}

internal void
ClipAndDrawNegativeDisc(uint8 *Pixels, int32 Pitch, render_dim *RenderRect, rect2 *CameraRect, circle Circle,
	uint8 R, uint8 G, uint8 B, uint8 A)
{
	int32 MinX = RenderRect->X, MinY = RenderRect->Y;

	int32 MaxX = RenderRect->X + RenderRect->W - 1, MaxY = RenderRect->Y + RenderRect->H - 1;

	int32 CX, CY;
	ConvertCoordToPixelSpace(CameraRect, Circle.P, RenderRect, &CX, &CY);

	int32 PixelRadius;
	ConvertDistToPixelDist(CameraRect, Circle.R, RenderRect, &PixelRadius);

	for (int32 CircleY = MinY - CY;
		CircleY <= MaxY - CY;
		++CircleY)
	{
		for (int32 CircleX = MinX - CX;
			CircleX <= MaxX - CX;
			++CircleX)
		{
			if ((CircleX * CircleX) + (CircleY * CircleY) > (PixelRadius * PixelRadius))
			{
				SetPixel(Pixels, Pitch, CircleX + CX, CircleY + CY, R, G, B, A);
			}
		}
	}
}


internal void
RenderScene(game_state *GameState, uint8 *Pixels, int32 Pitch, render_dim *RenderRect, rect2 *CameraRect, circle InvertedPlayer,
	circle InvertedAttack, circle InvertedBoundary, circle InvertedCuriosity, v2 IntersectOne, v2 IntersectTwo, bool32 InvertedCamera)
{

	real32 CameraZoom = 1.0f;
#define FOLLOWING_LEVEL 16.0f
#if 0
	if (InvertedCamera)
	{
		real32 PlayerRadiusRatio = 0.125f;
		if (InvertedPlayer.R < PlayerRadiusRatio)
		{
			CameraZoom = PlayerRadiusRatio / InvertedPlayer.R;
			CameraRect->Size = CameraRect->Size / CameraZoom;

			real32 t;
			if (CameraZoom >= 1.0 && CameraZoom < FOLLOWING_LEVEL)
			{
				t = (CameraZoom - 1.0f) / (FOLLOWING_LEVEL - 1.0f);
			}
			else
			{
				t = 1.0f;
			}

			v2 Start = { 0.0f,0.0f };
			v2 End = InvertedPlayer.P;
			CameraRect->P = Berp(Start, Start, t, End, End) - (CameraRect->Size * 0.5f);
		}
	}
	else
	{
		CameraRect->P = CameraRect->P + GameState->Player.P;
		
	}
#endif

#if 0
	if (InvertedCamera)
	{
		real32 Zoom = 0.5f / InvertedCuriosity.R;
		CameraRect->Size = CameraRect->Size / Zoom;
		CameraRect->P = CameraRect->Size * -0.5f;
		CameraRect->P = CameraRect->P + InvertedCuriosity.P;
	}
	else
	{
		real32 Zoom = 0.5f / GameState->Curiosity.R;
		CameraRect->Size = CameraRect->Size / Zoom;
		CameraRect->P = CameraRect->Size * -0.5f;
		CameraRect->P = CameraRect->P + GameState->Curiosity.P;
	}
#endif

#if 1
	if (InvertedCamera)
	{
		CameraRect->P = CameraRect->P + GameState->Player.P;
	}
	else
	{
		real32 Zoom = 0.5f / GameState->Curiosity.R;
		CameraRect->Size = CameraRect->Size / Zoom;
		CameraRect->P = CameraRect->Size * -0.5f;
		CameraRect->P = CameraRect->P + GameState->Curiosity.P;
	}
#endif

	DrawGridLines(Pixels, Pitch, RenderRect, CameraRect, 25, 105, 155, 255);

	real32 ShiftDistance;
	real32 MaxCircles;
	if (CameraZoom == 1.0f)
	{
		ShiftDistance = 1.0f;
		MaxCircles = 15.0;

		for (real32 ShiftIndex = 1.0;
			ShiftIndex < MaxCircles;
			ShiftIndex += 1.0)
		{
			circle Circle;
			Circle.R = Square(GameState->Inversion.R)
				/ (2.0f * (ShiftDistance * ShiftIndex));



			Circle.P = v2{ -Circle.R,0.0f };
			ClipAndDrawCircle(Pixels, Pitch, RenderRect, CameraRect, Circle, 25, 105, 155, 255);

			Circle.P = v2{ Circle.R,0.0f };
			ClipAndDrawCircle(Pixels, Pitch, RenderRect, CameraRect, Circle, 25, 105, 155, 255);

			Circle.P = v2{ 0.0f, -Circle.R };
			ClipAndDrawCircle(Pixels, Pitch, RenderRect, CameraRect, Circle, 25, 105, 155, 255);

			Circle.P = v2{ 0.0f, Circle.R };
			ClipAndDrawCircle(Pixels, Pitch, RenderRect, CameraRect, Circle, 25, 105, 155, 255);
		}

	}
	else
	{

		ShiftDistance = 1.0f;
		MaxCircles = 60.0f;
		real32 RatioConst = 2.0f;
		//real32 t = (CameraZoom - 1.0f) / (FOLLOWING_LEVEL - 1.0f);
		//real32 MaxCircles = Berp(100.0f, 100.0f, t, 100.0f, 500.0f);

		if (CameraZoom >= FOLLOWING_LEVEL)
		{
			if (InsideRect(CameraRect, GameState->Inversion.P))
			{
				MaxCircles = 100.0f;
				RatioConst = 0.0f;
			}
			else
			{
				MaxCircles = 115.0f;
				RatioConst = 0.0f;
			}
		}


		for (real32 ShiftIndex = 1.0;
			ShiftIndex < MaxCircles;
			ShiftIndex += 1.0)
		{

			circle Circle;
			Circle.R = 1.0f / (2.0f * (ShiftDistance * ShiftIndex));


			real32 CircleRatio = Circle.R / CameraRect->Size.Y;

			Circle.P = v2{ -Circle.R,0.0f };
			ClipAndDrawCircle(Pixels, Pitch, RenderRect, CameraRect, Circle, 25, 105, 155, 255);

			Circle.P = v2{ Circle.R,0.0f };
			ClipAndDrawCircle(Pixels, Pitch, RenderRect, CameraRect, Circle, 25, 105, 155, 255);


			Circle.P = v2{ 0.0f, -Circle.R };
			ClipAndDrawCircle(Pixels, Pitch, RenderRect, CameraRect, Circle, 25, 105, 155, 255);

			Circle.P = v2{ 0.0f, Circle.R };
			ClipAndDrawCircle(Pixels, Pitch, RenderRect, CameraRect, Circle, 25, 105, 155, 255);
		}

	}

	ClipAndDrawCircle(Pixels, Pitch, RenderRect, CameraRect, GameState->Inversion, 255, 255, 255, 255);






	if (InsideRect(CameraRect, GameState->Inversion.P))
	{
		circle Circle;
		Circle.R = Square(GameState->Inversion.R) / (2.0f * (ShiftDistance*MaxCircles));

		Circle.P = v2{ -Circle.R,0.0f };
		ClipAndDrawDisc(Pixels, Pitch, RenderRect, CameraRect, Circle, 25, 105, 155, 255);

		Circle.P = v2{ Circle.R,0.0f };
		ClipAndDrawDisc(Pixels, Pitch, RenderRect, CameraRect, Circle, 25, 105, 155, 255);

		Circle.P = v2{ 0.0f,-Circle.R };
		ClipAndDrawDisc(Pixels, Pitch, RenderRect, CameraRect, Circle, 25, 105, 155, 255);

		Circle.P = v2{ 0.0f,Circle.R };
		ClipAndDrawDisc(Pixels, Pitch, RenderRect, CameraRect, Circle, 25, 105, 155, 255);
	}

	ClipAndDrawDisc(Pixels, Pitch, RenderRect, CameraRect, InvertedBoundary, 123, 34, 213, 255);

	v2 CameraCenter = CameraRect->P + (0.5f * CameraRect->Size);
	real32 CameraRadius = Distance(CameraRect->P, CameraCenter);

	if (CameraRadius < GameState->Boundary.R)
	{
		if (NegativeCircleOverlap(CameraCenter, CameraRadius, GameState->Boundary.P, GameState->Boundary.R) && !InvertedCamera)
		{
			ClipAndDrawNegativeDisc(Pixels, Pitch, RenderRect, CameraRect, GameState->Boundary, 123, 34, 213, 255);
		}
	}
	else
	{
		ClipAndDrawNegativeDisc(Pixels, Pitch, RenderRect, CameraRect, GameState->Boundary, 123, 34, 213, 255);
	}


	ClipAndDrawDisc(Pixels, Pitch, RenderRect, CameraRect, GameState->Curiosity, 123, 234, 13, 255);

	ClipAndDrawDisc(Pixels, Pitch, RenderRect, CameraRect, InvertedCuriosity, 123, 234, 13, 255);



	ClipAndDrawDisc(Pixels, Pitch, RenderRect, CameraRect, GameState->Player, 123, 234, 213, 255);

	ClipAndDrawDisc(Pixels, Pitch, RenderRect, CameraRect, InvertedPlayer, 123, 234, 213, 255);


	if (GameState->Attack.R > 0.0f)
	{
		if (IntersectOne.X != IntersectTwo.X || IntersectOne.Y != IntersectTwo.Y)
		{
			ClipAndDrawCroppedCircle(Pixels, Pitch, RenderRect, CameraRect, GameState->Attack, IntersectOne, IntersectTwo, 0, 255, 255, 0);

			ClipAndDrawCroppedCircle(Pixels, Pitch, RenderRect, CameraRect, InvertedAttack, IntersectTwo, IntersectOne, 0, 255, 255, 0);
		}
		else
		{
			ClipAndDrawCircle(Pixels, Pitch, RenderRect, CameraRect, GameState->Attack, 0, 255, 255, 0);

			ClipAndDrawCircle(Pixels, Pitch, RenderRect, CameraRect, InvertedAttack, 0, 255, 255, 0);
		}
	}


}







internal void
GameUpdateAndRender(game_state *GameState, user_input *Input, uint8* Pixels, int32 Pitch, 
			uint8 *SoundData, int32 BytesToWrite, uint16 Samples)
{

	int32 KeysDown = 0;

	real32 Speed = 10.0f;
	v2 dP = v2{ 0.0f,0.0f };

	if (Input->Up)
	{
		dP.Y += -1.0f;
		++KeysDown;
	}

	if (Input->Down)
	{
		dP.Y += 1.0f;
		++KeysDown;
	}

	if (Input->Left)
	{
		dP.X += -1.0f;
		--GameState->SoundOffset;
		++KeysDown;
	}

	if (Input->Right)
	{
		dP.X += 1.0f;
		++GameState->SoundOffset;
		++KeysDown;
	}

	bool32 NewAttack = true;
	if (Input->Action)
	{
		NewAttack = true;
		++KeysDown;
	}

	if (Input->Toggle)
	{
		GameState->Toggle = (GameState->Toggle + 1) & 3;
	}

	if (dP.X != 0.0f || dP.Y != 0.0f)
	{
		dP = dP * (Speed * Input->dt / Length(dP));
	}


	circle InvertedBoundary = InvertCircleFromUnit(GameState->Boundary);

	v2 CuriositydP = dP / Speed;
	if (LengthSq(CuriositydP) > 0.0f)
	{

		CuriositydP = MoveCircleSafely(GameState->Curiosity, CuriositydP, InvertedBoundary);

		CuriositydP = MoveCircleSafelyNegative(GameState->Curiosity, CuriositydP, GameState->Inversion);

		circle TempInvertedPlayer = InvertCircleFromUnit(GameState->Player);

		CuriositydP = MoveCircleSafely(GameState->Curiosity, CuriositydP, TempInvertedPlayer);

	}

	GameState->Curiosity.P = GameState->Curiosity.P + CuriositydP;

	circle InvertedCuriosity = InvertCircleFromUnit(GameState->Curiosity);

	if(LengthSq(dP) > 0.0f)
	{	
		uint32 PossiblePointsSize = 0;
		v2 PossiblePoints[10] = {};
		uint32 NeighborsSize = 0;
		circle Neighbors[5] = {};
		circle Mover = GameState->Player;
		Mover.R = Length(dP);

		if (CirclesOverlap(GameState->Player, dP, GameState->Inversion))
		{

			circle Collider = GameState->Inversion;
			Collider.R += GameState->Player.R;

			uint32 NumIntersect;
			GetCircleIntersection(Mover, Collider, &PossiblePoints[PossiblePointsSize], &PossiblePoints[PossiblePointsSize + 1],&NumIntersect);

			if (NumIntersect == 2)
			{
				PossiblePointsSize += 2;
				Neighbors[NeighborsSize++] = GameState->Inversion;
			}
			else if (NumIntersect == 1)
			{
				++PossiblePointsSize;
				Neighbors[NeighborsSize++] = GameState->Inversion;
			}
		}

		if (CirclesOverlap(GameState->Player, dP, InvertedCuriosity))
		{

			circle Collider = InvertedCuriosity;
			Collider.R += GameState->Player.R;

			uint32 NumIntersect;
			GetCircleIntersection(Mover, Collider, &PossiblePoints[PossiblePointsSize], &PossiblePoints[PossiblePointsSize + 1], &NumIntersect);

			if (NumIntersect == 2)
			{
				PossiblePointsSize += 2;
				Neighbors[NeighborsSize++] = InvertedCuriosity;
			}
			else if (NumIntersect == 1)
			{
				++PossiblePointsSize;
				Neighbors[NeighborsSize++] = InvertedCuriosity;
			}
		}

		if (NegativeCircleOverlap(GameState->Player, dP, GameState->Boundary))
		{

			circle Collider = GameState->Boundary;
			Collider.R += -GameState->Player.R;

			uint32 NumIntersect;
			GetCircleIntersection(Mover, Collider, &PossiblePoints[PossiblePointsSize], &PossiblePoints[PossiblePointsSize + 1], &NumIntersect);

			if (NumIntersect == 2)
			{
				PossiblePointsSize += 2;
				Neighbors[NeighborsSize++] = GameState->Boundary;
			}
			else if (NumIntersect == 1)
			{
				++PossiblePointsSize;
				Neighbors[NeighborsSize++] = GameState->Boundary;
			}
		}


		if (PossiblePointsSize == 0)
		{
			GameState->Player.P = GameState->Player.P + dP;
		}
		else
		{
			v2 LegalMoves[10] = {};
			uint32 LegalMovesSize = 0;

			for (uint32 AllowedMovesIndex = 0;
				AllowedMovesIndex < PossiblePointsSize;
				++AllowedMovesIndex)
			
			{
				bool32 FreePoint = true;
				for (uint32 NeighborsIndex = 0;
					NeighborsIndex < NeighborsSize;
					++NeighborsIndex)
				{
					real32 Epsilon = 0.01;
					if (Square(PossiblePoints[AllowedMovesIndex].X - Neighbors[NeighborsIndex].P.X) + Square(PossiblePoints[AllowedMovesIndex].Y - Neighbors[NeighborsIndex].P.Y) < Square(Neighbors[NeighborsIndex].R + GameState->Player.R - Epsilon))
					{
						FreePoint = false;
					}
				}

				if (FreePoint)
				{
					LegalMoves[LegalMovesSize++] = PossiblePoints[AllowedMovesIndex];
				}
			}


			if (LegalMovesSize > 0)
			{
				uint32 FinalMoveIndex = 0;
				real32 Shortest = 1000000.0f;
				v2 IdealMove = GameState->Player.P + dP;
				for (uint32 LegalMovesIndex = 0;
					LegalMovesIndex < LegalMovesSize;
					++LegalMovesIndex)
				{

					real32 DistanceSquared = DistanceSq(LegalMoves[LegalMovesIndex], IdealMove);
					if (DistanceSquared < Shortest)
					{
						FinalMoveIndex = LegalMovesIndex;
						Shortest = DistanceSquared;
					}
				}

				GameState->Player.P = LegalMoves[FinalMoveIndex];
			}
		}
	}


	


	if (GameState->SoundOffset > 30000)
	{
		GameState->SoundOffset = 30000;
	}
	if (GameState->SoundOffset < 1)
	{
		GameState->SoundOffset = 1;
	}

	real64 Volume = 0.0;
	real64 dt = ((real64)GameState->SoundOffset) / ((real64)Samples);
	if (BytesToWrite > 0)
	{
		for (int32 SoundIndex = 0;
			SoundIndex < BytesToWrite;
			++SoundIndex)
		{
			real64 Value = (Sine(GameState->tSine) + 1.0)*Volume;
			SoundData[SoundIndex] = ((uint8)Value);
			GameState->tSine += dt;
		}
	}




	circle InvertedPlayer = InvertCircleFromUnit(GameState->Player);


	if (NewAttack && GameState->Attack.R == 0.0f)
	{
		GameState->Attack.P = GameState->Player.P;
		GameState->Attack.R = GameState->Player.R;
	}
	else
	{
		                             
		if (GameState->Attack.R > 0.0f)
		{
			GameState->Attack.R += (GameState->Attack.R * 1.0f * Input->dt);

			if (GameState->Attack.R > 2.0f * GameState->Player.R)
			{
				GameState->Attack.R = 0.0f;
			}
		}
	}

	circle InvertedAttack = InvertCircleFromUnit(GameState->Attack);


	v2 IntersectOne = v2{ 0.0f,0.0f }, IntersectTwo = v2{ 0.0f,0.0f };
	uint32 AttackIntersections;
	if (GameState->Attack.R > 0.0f)
	{
		
		GetCircleIntersection(GameState->Attack, InvertedAttack, &IntersectOne, &IntersectTwo, &AttackIntersections);
	}


	uint32 *CurrentPixel = (uint32*)Pixels;
	for (int32 Y = GameState->RenderRect.Y;
		Y < GameState->RenderRect.H + GameState->RenderRect.Y;
		++Y)
	{
		for (int32 X = GameState->RenderRect.X;
			X < GameState->RenderRect.W + GameState->RenderRect.X;
			++X)

		{
			*CurrentPixel++ = 0;
		}
	}

	if (GameState->Toggle == 0)
	{
		rect2 CameraRect = GameState->Screen;
		RenderScene(GameState, Pixels, Pitch, &GameState->RenderRect, &CameraRect, InvertedPlayer, InvertedAttack, InvertedBoundary, InvertedCuriosity, IntersectOne, IntersectTwo, true);
	}
	else if (GameState->Toggle == 1)
	{
		rect2 CameraRect = GameState->Screen;
		CameraRect.P = CameraRect.P * 1.5f;
		CameraRect.Size = CameraRect.Size * 1.5f;
		RenderScene(GameState, Pixels, Pitch, &GameState->RenderRect, &CameraRect, InvertedPlayer, InvertedAttack, InvertedBoundary, InvertedCuriosity, IntersectOne, IntersectTwo, false);
	}
	else if (GameState->Toggle == 2)
	{
		render_dim LeftRenderRect = GameState->RenderRect;
		LeftRenderRect.W = (GameState->RenderRect.W / 2) - 1;
		real32 ScreenRatio = ((real32)LeftRenderRect.W) / ((real32)LeftRenderRect.H);
		rect2 LeftCamera;
		LeftCamera.P.X = -ScreenRatio * 3.0f;
		LeftCamera.P.Y = -3.0f;
		LeftCamera.Size.X = 6.0f * ScreenRatio;
		LeftCamera.Size.Y = 6.0f;
		RenderScene(GameState, Pixels, Pitch, &LeftRenderRect, &LeftCamera, InvertedPlayer, InvertedAttack, InvertedBoundary, InvertedCuriosity, IntersectOne, IntersectTwo, false);

		render_dim RightRenderRect = GameState->RenderRect;
		RightRenderRect.W = LeftRenderRect.W;
		RightRenderRect.X = LeftRenderRect.W+1;
		ScreenRatio = ((real32)LeftRenderRect.W) / ((real32)LeftRenderRect.H);
		rect2 RightCamera;
		RightCamera.P.X = -ScreenRatio * 2.0f;
		RightCamera.P.Y = -2.0f;
		RightCamera.Size.X = 4.0f * ScreenRatio;
		RightCamera.Size.Y = 4.0f;
		RenderScene(GameState, Pixels, Pitch, &RightRenderRect, &RightCamera, InvertedPlayer, InvertedAttack, InvertedBoundary, InvertedCuriosity,  IntersectOne, IntersectTwo, true);
	}
	else
	{
		render_dim LeftRenderRect = GameState->RenderRect;
		LeftRenderRect.W = (GameState->RenderRect.W / 2) - 1;
		real32 ScreenRatio = ((real32)LeftRenderRect.W) / ((real32)LeftRenderRect.H);
		rect2 LeftCamera;
		LeftCamera.P.X = -ScreenRatio * 2.0f;
		LeftCamera.P.Y = -2.0f;
		LeftCamera.Size.X = 4.0f * ScreenRatio;
		LeftCamera.Size.Y = 4.0f;
		RenderScene(GameState, Pixels, Pitch, &LeftRenderRect, &LeftCamera, InvertedPlayer, InvertedAttack, InvertedBoundary, InvertedCuriosity, IntersectOne, IntersectTwo, true);

		render_dim RightRenderRect = GameState->RenderRect;
		RightRenderRect.W = LeftRenderRect.W;
		RightRenderRect.X = LeftRenderRect.W+1;
		ScreenRatio = ((real32)LeftRenderRect.W) / ((real32)LeftRenderRect.H);
		rect2 RightCamera;
		RightCamera.P.X = -ScreenRatio * 3.0f;
		RightCamera.P.Y = -3.0f;
		RightCamera.Size.X = 6.0f * ScreenRatio;
		RightCamera.Size.Y = 6.0f;
		RenderScene(GameState, Pixels, Pitch, &RightRenderRect, &RightCamera, InvertedPlayer, InvertedAttack, InvertedBoundary, InvertedCuriosity, IntersectOne, IntersectTwo, false);

	}
}

#if 0
typedef struct {
	union {
		struct {
			uint64 A;
			uint64 B;
		};
		uint64 Num[2];
	};
}big_number;

inline big_number
operator+(big_number A, big_number B)
{
	big_number Result;

	Result.A = A.A + B.A;
	Result.B = A.B + B.B;
	if (Result.B < A.B || Result.B < B.B)
	{
		++Result.A;
	}

	return Result;
}

inline big_number
operator*(big_number A, big_number B)
{
	big_number Result;

	Result.A = A.A + B.A;
	Result.B = A.B + B.B;
	if (Result.B < A.B || Result.B < B.B)
	{
		++Result.A;
	}

	return Result;
}

inline big_number
operator>>(big_number A, uint64 B)
{
	big_number Result;

	Result.A = A.A;
	Result.B = A.B;
	Result.A >>= B;
	Result.B >>= B;
	if (A.A & 1)
	{
		Result.B |= 0x8000000000000000;
	}

	return Result;
}
#endif



int
main(int argc, char* argv[]) 
{

	game_state GameState = {};
	GameState.RenderRect.X = 0;
	GameState.RenderRect.Y = 0;
	GameState.RenderRect.W = 910;
	GameState.RenderRect.H = 512;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

	SDL_Window *Window = SDL_CreateWindow("Rooms", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, GameState.RenderRect.W, GameState.RenderRect.H, 0);


	SDL_Surface *Surface = SDL_GetWindowSurface(Window);

	uint32 Format = Surface->format->format;

	printf("The Pixel Format is %s\n", SDL_GetPixelFormatName(Surface->format->format));
	printf("Red mask is %x\n", Surface->format->Rmask);
	printf("Green mask is %x\n", Surface->format->Gmask);
	printf("Blue mask is %x\n", Surface->format->Bmask);
	printf("Alpha mask is %x\n", Surface->format->Amask);

	//Pixel Mask is (in hex) XX RR GG BB

	

	SDL_AudioSpec DesiredAudio = {}, ObtainedAudio;
	DesiredAudio.freq = 48000;
	DesiredAudio.format = AUDIO_U8;
	DesiredAudio.channels = 1;
	DesiredAudio.samples = 4096;
	DesiredAudio.callback = 0;

	SDL_AudioDeviceID AudioDeviceID = SDL_OpenAudioDevice(0, 0, &DesiredAudio,
		&ObtainedAudio, SDL_AUDIO_ALLOW_ANY_CHANGE);


	if (DesiredAudio.format != ObtainedAudio.format) {
		printf("Audio not in desired format!!!\n");
		if (SDL_AUDIO_ISSIGNED(ObtainedAudio.format))
		{
			printf("Audio format is signed\n");
		}
		if (SDL_AUDIO_ISINT(ObtainedAudio.format))
		{
			printf("Audio format is an int\n");
		}
		if (SDL_AUDIO_MASK_BITSIZE & ObtainedAudio.format)
		{
			printf("Audio format is %d long\n", (int)(SDL_AUDIO_MASK_BITSIZE & ObtainedAudio.format));
		}
		return 5;
	}
	
	{
		Seed(&GameState.RNG, 100);
		real32 ScreenRatio = ((real32)GameState.RenderRect.W) / ((real32)GameState.RenderRect.H);
		GameState.Screen.P.X = -ScreenRatio * 2.0f;
		GameState.Screen.P.Y = -2.0f;
		GameState.Screen.Size.X = 4.0f * ScreenRatio;
		GameState.Screen.Size.Y = 4.0f;
		GameState.Player.P.X = -2.0f;
		GameState.Player.P.Y = 0.0f;
		GameState.Player.R = 0.5f;
		GameState.Inversion.P.X = 0.0f;
		GameState.Inversion.P.Y = 0.0f;
		GameState.Inversion.R = 1.0f;
		GameState.Attack.P = v2{ 0.0f,0.0f };
		GameState.Attack.R = 0.0f;
		GameState.Boundary.P = v2{ 0.0f,0.0f };
		GameState.Boundary.R = 110.0f;
		GameState.SoundOffset = 100;
		GameState.Toggle = 0;
		GameState.tSine = 0.0;
		GameState.Curiosity.P = v2{ 0.5f,0.0f };
		GameState.Curiosity.R = 0.25f;
	}

#if 0
	//big_number Start = { 0x0000000001ffffff,0xffffffffffffffff };
	big_number Start = { 0x000fffffffffffff,0xffffffffffffffff };
	big_number X = Start;
	uint64 Num = 0;
	big_number One = { 0,1 };
	while (!(X.A == 0 && X.B == 1))
	{
		if (!(X.B & 1))
		{
			X = X >> 1;
			++Num;
		}
		else
		{ 
			X = (X + X + X + One) >> 1;
			Num += 2;
		}
	}
	printf("It took 1422 times for 2^89-1 to go down to one\n");
	printf("It took %d times for 2^116-1 to go down to one\n", Num);
#endif

	uint8 SoundData[8192];
	bool32 Running = true;
	user_input Input = { 0 };
	Input.dt = 1.0f / 60.0f;
	uint64 BeginFrame = SDL_GetPerformanceCounter();
	uint64 CountFrequency = SDL_GetPerformanceFrequency();
	bool32 PreviousToggle = false;
	SDL_PauseAudioDevice(AudioDeviceID, 0);
	while (Running)
	{
		if(SDL_QuitRequested())
		{
			Running = false;
		}
		const uint8 *KeyboardState = SDL_GetKeyboardState(0);
		Input.Action = KeyboardState[SDL_SCANCODE_SPACE];
		Input.Up = (KeyboardState[SDL_SCANCODE_UP] || KeyboardState[SDL_SCANCODE_W]);
		Input.Down = (KeyboardState[SDL_SCANCODE_DOWN] || KeyboardState[SDL_SCANCODE_S]);
		Input.Left = (KeyboardState[SDL_SCANCODE_LEFT] || KeyboardState[SDL_SCANCODE_A]);
		Input.Right = (KeyboardState[SDL_SCANCODE_RIGHT] || KeyboardState[SDL_SCANCODE_D]);
		Input.Toggle = KeyboardState[SDL_SCANCODE_0] && !PreviousToggle;

		
		int32 BytesToWrite = (int32)sizeof(SoundData) - (int32)SDL_GetQueuedAudioSize(AudioDeviceID);

		GameUpdateAndRender(&GameState, &Input, (uint8*)Surface->pixels, (int32)Surface->pitch, SoundData, BytesToWrite, 
			ObtainedAudio.samples);

		uint64 MidFrame = SDL_GetPerformanceCounter();
		uint64 UpdateTime = ((MidFrame - BeginFrame) * 1000) / CountFrequency;
		int32 WaitTime = 8 - (int32)UpdateTime;
		if (WaitTime > 0)
		{
			SDL_Delay(WaitTime);
		}

		SDL_UpdateWindowSurface(Window);
		SDL_QueueAudio(AudioDeviceID, (void*)SoundData, (uint32)BytesToWrite);


		uint64 EndFrame = SDL_GetPerformanceCounter();
		uint64 FrameTime = ((EndFrame - BeginFrame) * 1000000) / CountFrequency;
		Input.dt = (real32)FrameTime / 1000000.0f;
		//printf("FrameTime = %d . %d ms\n",FrameTime/1000, FrameTime%1000);
		//printf("FPS = %f\n", 1.0f / Input.dt);
		BeginFrame = EndFrame;
		PreviousToggle = KeyboardState[SDL_SCANCODE_0];
	}

	SDL_DestroyWindow(Window);
	SDL_Quit();
	return 0;
}
