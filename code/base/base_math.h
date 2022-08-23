
#ifndef BASE_MATH_H
#define BASE_MATH_H

#ifdef COMPILER_CL
/* MSVC supports SSE in amd64 mode or _M_IX86_FP >= 1 (2 means SSE2) */
# if defined(ARCH_X64) || ( defined(_M_IX86_FP) && _M_IX86_FP >= 1 )
#  define MATH__USE_SSE 1
# endif
#else /* not MSVC, probably GCC, clang, icc or something that doesn't support SSE anyway */
# ifdef __SSE__ /* they #define __SSE__ if it's supported */
#  define MATH__USE_SSE 1
# endif /*  __SSE__ */
#endif /*   */


#ifdef MATH__USE_SSE
#include <xmmintrin.h>
#endif

#include <math.h>

#define PI32 3.14159265359f
#define PI 3.14159265358979323846

#define ClampTop(x,a) MIN(x,a)
#define ClampBot(a,x) MAX(a,x)
#define AbsVal(a) (((a) >= 0)? (a):-(a))

#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define ABS(a) ((a) > 0 ? (a) : -(a))
#define MOD(a, m) (((a) % (m)) >= 0 ? ((a) % (m)) : (((a) % (m)) + (m)))
#define SQUARE(x) ((x) * (x))

typedef union vec2
{
    struct
    {
        r32 X, Y;
    };
    
    struct
    {
        r32 U, V;
    };
    
    struct
    {
        r32 Left, Right;
    };
    
    struct
    {
        r32 Width, Height;
    };
    
    r32 Elements[2];
    
#ifdef __cplusplus
    inline r32 &operator[](const int &Index)
    {
        return Elements[Index];
    }
#endif
} vec2;

typedef union vec2i32
{
    struct
    {
        i32 X, Y;
    };
    
    struct
    {
        i32 U, V;
    };
    
    struct
    {
        i32 Left, Right;
    };
    
    struct
    {
        i32 Width, Height;
    };
    
    i32 Elements[2];
    
#ifdef __cplusplus
    inline i32 &operator[](const int &Index)
    {
        return Elements[Index];
    }
#endif
} vec2i32;

typedef union vec3
{
    struct
    {
        r32 X, Y, Z;
    };
    
    struct
    {
        r32 U, V, W;
    };
    
    struct
    {
        r32 R, G, B;
    };
    
    struct
    {
        vec2 XY;
        r32 Ignored0_;
    };
    
    struct
    {
        r32 Ignored1_;
        vec2 YZ;
    };
    
    struct
    {
        vec2 UV;
        r32 Ignored2_;
    };
    
    struct
    {
        r32 Ignored3_;
        vec2 VW;
    };
    
    r32 Elements[3];
    
#ifdef __cplusplus
    inline r32 &operator[](const int &Index)
    {
        return Elements[Index];
    }
#endif
} vec3;

typedef union vec3i32
{
    struct
    {
        i32 X, Y, Z;
    };
    
    struct
    {
        i32 U, V, W;
    };
    
    struct
    {
        i32 R, G, B;
    };
    
    struct
    {
        vec2i32 XY;
        i32 Ignored0_;
    };
    
    struct
    {
        i32 Ignored1_;
        vec2i32 YZ;
    };
    
    struct
    {
        vec2i32 UV;
        i32 Ignored2_;
    };
    
    struct
    {
        i32 Ignored3_;
        vec2i32 VW;
    };
    
    i32 Elements[3];
    
#ifdef __cplusplus
    inline i32 &operator[](const int &Index)
    {
        return Elements[Index];
    }
#endif
} vec3i32;

typedef union vec4
{
    struct
    {
        union
        {
            vec3 XYZ;
            struct
            {
                r32 X, Y, Z;
            };
        };
        
        r32 W;
    };
    struct
    {
        union
        {
            vec3 RGB;
            struct
            {
                r32 R, G, B;
            };
        };
        
        r32 A;
    };
    
    struct
    {
        vec2 XY;
        r32 Ignored0_;
        r32 Ignored1_;
    };
    
    struct
    {
        r32 Ignored2_;
        vec2 YZ;
        r32 Ignored3_;
    };
    
    struct
    {
        r32 Ignored4_;
        r32 Ignored5_;
        vec2 ZW;
    };
    
    r32 Elements[4];
    
#ifdef MATH__USE_SSE
    __m128 InternalElementsSSE;
#endif
    
#ifdef __cplusplus
    inline r32 &operator[](const int &Index)
    {
        return Elements[Index];
    }
#endif
} vec4;

typedef union mat4
{
    r32 Elements[4][4];
    
#ifdef MATH__USE_SSE
    __m128 Columns[4];
#endif
    
#ifdef __cplusplus
    inline vec4 operator[](const int &Index)
    {
        vec4 Result;
        r32* Column = Elements[Index];
        
        
        Result.Elements[0] = Column[0];
        Result.Elements[1] = Column[1];
        Result.Elements[2] = Column[2];
        Result.Elements[3] = Column[3];
        
        return Result;
    }
#endif
} mat4;

typedef union quaternion
{
    struct
    {
        union
        {
            vec3 XYZ;
            struct
            {
                r32 X, Y, Z;
            };
        };
        
        r32 W;
    };
    
    r32 Elements[4];
    
#ifdef MATH__USE_SSE
    __m128 InternalElementsSSE;
#endif
} quaternion;


typedef vec2 v2r32;
typedef vec3 v3r32;
typedef vec2i32 v2i32;
typedef vec3i32 v3i32;
typedef vec4 v4r32;
typedef mat4 m4r32;

typedef union rectangle_2d
{
    struct
    {
        v2r32 MinP;
        v2r32 MaxP;
    };
    
    v4r32 CompactRect;
    
    struct
    {
        r32 X0; r32 Y0;
        r32 X1; r32 Y1;
    };
    
    r32 Elements[4];
} rectangle_2d;


/*
 * Floating-point math functions
 */


inline_internal r32
SinF(r32 Radians)
{
    
    r32 Result = sinf(Radians);
    
    return (Result);
}

inline_internal r32
CosF(r32 Radians)
{
    
    r32 Result = cosf(Radians);
    
    return (Result);
}

inline_internal r32
TanF(r32 Radians)
{
    
    r32 Result = tanf(Radians);
    
    return (Result);
}

inline_internal r32
ACosF(r32 Radians)
{
    
    r32 Result = acosf(Radians);
    
    return (Result);
}

inline_internal r32
ATanF(r32 Radians)
{
    
    r32 Result = atanf(Radians);
    
    return (Result);
}

inline_internal r32
ATan2F(r32 Left, r32 Right)
{
    
    r32 Result = atan2f(Left, Right);
    
    return (Result);
}

inline_internal r32
ExpF(r32 Float)
{
    
    r32 Result = expf(Float);
    
    return (Result);
}

inline_internal r32
LogF(r32 Float)
{
    
    r32 Result = logf(Float);
    
    return (Result);
}

inline_internal r32
SquareRootF(r32 Float)
{
    
    r32 Result;
    
#ifdef MATH__USE_SSE
    __m128 In = _mm_set_ss(Float);
    __m128 Out = _mm_sqrt_ss(In);
    Result = _mm_cvtss_f32(Out);
#else
    Result = sqrtf(Float);
#endif
    
    return(Result);
}

inline_internal r32
RSquareRootF(r32 Float)
{
    
    r32 Result;
    
#ifdef MATH__USE_SSE
    __m128 In = _mm_set_ss(Float);
    __m128 Out = _mm_rsqrt_ss(In);
    Result = _mm_cvtss_f32(Out);
#else
    Result = 1.0f/PREFIX(SquareRootF)(Float);
#endif
    
    return(Result);
}

inline_internal r32
Power(r32 Base, int Exponent)
{
    
    r32 Result = 1.0f;
    r32 Mul = Exponent < 0 ? 1.f / Base : Base;
    int X = Exponent < 0 ? -Exponent : Exponent;
    while (X)
    {
        if (X & 1)
        {
            
            Result *= Mul;
        }
        
        Mul *= Mul;
        X >>= 1;
    }
    
    return (Result);
}

inline_internal r32
PowerF(r32 Base, r32 Exponent)
{
    
    r32 Result = expf(Exponent * logf(Base));
    
    return (Result);
}


/*
 * Utility functions
 */

inline_internal r32
ToRadians(r32 Degrees)
{
    
    r32 Result = Degrees * (PI32 / 180.0f);
    
    return (Result);
}

inline_internal r32
Lerp(r32 A, r32 Time, r32 B)
{
    
    r32 Result = (1.0f - Time) * A + Time * B;
    
    return (Result);
}

inline_internal r32
Clamp(r32 Min, r32 Value, r32 Max)
{
    
    r32 Result = Value;
    
    if(Result < Min)
    {
        Result = Min;
    }
    
    if(Result > Max)
    {
        Result = Max;
    }
    
    return (Result);
}


/*
 * Vector initialization
 */

inline_internal vec2
Vec2(r32 X, r32 Y)
{
    
    vec2 Result;
    
    Result.X = X;
    Result.Y = Y;
    
    return (Result);
}

inline_internal vec2
Vec2i(int X, int Y)
{
    
    vec2 Result;
    
    Result.X = (r32)X;
    Result.Y = (r32)Y;
    
    return (Result);
}

inline_internal vec2i32
Vec2I32(i32 X, i32 Y)
{
    
    vec2i32 Result;
    
    Result.X = X;
    Result.Y = Y;
    
    return (Result);
}

inline_internal vec3
Vec3(r32 X, r32 Y, r32 Z)
{
    
    vec3 Result;
    
    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;
    
    return (Result);
}

inline_internal vec3
Vec3i(int X, int Y, int Z)
{
    
    vec3 Result;
    
    Result.X = (r32)X;
    Result.Y = (r32)Y;
    Result.Z = (r32)Z;
    
    return (Result);
}

inline_internal vec3
Vec3v(vec2 Vector, r32 Z)
{
    
    vec3 Result;
    
    Result.XY = Vector;
    Result.Z = Z;
    
    return (Result);
}

inline_internal vec4
Vec4(r32 X, r32 Y, r32 Z, r32 W)
{
    
    vec4 Result;
    
#ifdef MATH__USE_SSE
    Result.InternalElementsSSE = _mm_setr_ps(X, Y, Z, W);
#else
    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;
    Result.W = W;
#endif
    
    return (Result);
}

inline_internal vec4
Vec4i(int X, int Y, int Z, int W)
{
    
    vec4 Result;
    
#ifdef MATH__USE_SSE
    Result.InternalElementsSSE = _mm_setr_ps((r32)X, (r32)Y, (r32)Z, (r32)W);
#else
    Result.X = (r32)X;
    Result.Y = (r32)Y;
    Result.Z = (r32)Z;
    Result.W = (r32)W;
#endif
    
    return (Result);
}

inline_internal vec4
Vec4v(vec3 Vector, r32 W)
{
    
    vec4 Result;
    
#ifdef MATH__USE_SSE
    Result.InternalElementsSSE = _mm_setr_ps(Vector.X, Vector.Y, Vector.Z, W);
#else
    Result.XYZ = Vector;
    Result.W = W;
#endif
    
    return (Result);
}


/*
 * Binary vector operations
 */

inline_internal vec2
AddVec2(vec2 Left, vec2 Right)
{
    
    vec2 Result;
    
    Result.X = Left.X + Right.X;
    Result.Y = Left.Y + Right.Y;
    
    return (Result);
}

inline_internal vec3
AddVec3(vec3 Left, vec3 Right)
{
    
    vec3 Result;
    
    Result.X = Left.X + Right.X;
    Result.Y = Left.Y + Right.Y;
    Result.Z = Left.Z + Right.Z;
    
    return (Result);
}

inline_internal vec4
AddVec4(vec4 Left, vec4 Right)
{
    
    vec4 Result;
    
#ifdef MATH__USE_SSE
    Result.InternalElementsSSE = _mm_add_ps(Left.InternalElementsSSE, Right.InternalElementsSSE);
#else
    Result.X = Left.X + Right.X;
    Result.Y = Left.Y + Right.Y;
    Result.Z = Left.Z + Right.Z;
    Result.W = Left.W + Right.W;
#endif
    
    return (Result);
}

inline_internal vec2
SubtractVec2(vec2 Left, vec2 Right)
{
    
    vec2 Result;
    
    Result.X = Left.X - Right.X;
    Result.Y = Left.Y - Right.Y;
    
    return (Result);
}

inline_internal vec3
SubtractVec3(vec3 Left, vec3 Right)
{
    
    vec3 Result;
    
    Result.X = Left.X - Right.X;
    Result.Y = Left.Y - Right.Y;
    Result.Z = Left.Z - Right.Z;
    
    return (Result);
}

inline_internal vec4
SubtractVec4(vec4 Left, vec4 Right)
{
    
    vec4 Result;
    
#ifdef MATH__USE_SSE
    Result.InternalElementsSSE = _mm_sub_ps(Left.InternalElementsSSE, Right.InternalElementsSSE);
#else
    Result.X = Left.X - Right.X;
    Result.Y = Left.Y - Right.Y;
    Result.Z = Left.Z - Right.Z;
    Result.W = Left.W - Right.W;
#endif
    
    return (Result);
}

inline_internal vec2
HadamardMultiplyVec2(vec2 Left, vec2 Right)
{
    
    vec2 Result;
    
    Result.X = Left.X * Right.X;
    Result.Y = Left.Y * Right.Y;
    
    return (Result);
}

inline_internal vec2
MultiplyVec2f(vec2 Left, r32 Right)
{
    
    vec2 Result;
    
    Result.X = Left.X * Right;
    Result.Y = Left.Y * Right;
    
    return (Result);
}

inline_internal vec3
HadamardMultiplyVec3(vec3 Left, vec3 Right)
{
    
    vec3 Result;
    
    Result.X = Left.X * Right.X;
    Result.Y = Left.Y * Right.Y;
    Result.Z = Left.Z * Right.Z;
    
    return (Result);
}

inline_internal vec3
MultiplyVec3f(vec3 Left, r32 Right)
{
    
    vec3 Result;
    
    Result.X = Left.X * Right;
    Result.Y = Left.Y * Right;
    Result.Z = Left.Z * Right;
    
    return (Result);
}

inline_internal vec4
HadamardMultiplyVec4(vec4 Left, vec4 Right)
{
    
    vec4 Result;
    
#ifdef MATH__USE_SSE
    Result.InternalElementsSSE = _mm_mul_ps(Left.InternalElementsSSE, Right.InternalElementsSSE);
#else
    Result.X = Left.X * Right.X;
    Result.Y = Left.Y * Right.Y;
    Result.Z = Left.Z * Right.Z;
    Result.W = Left.W * Right.W;
#endif
    
    return (Result);
}

inline_internal vec4
MultiplyVec4f(vec4 Left, r32 Right)
{
    
    vec4 Result;
    
#ifdef MATH__USE_SSE
    __m128 Scalar = _mm_set1_ps(Right);
    Result.InternalElementsSSE = _mm_mul_ps(Left.InternalElementsSSE, Scalar);
#else
    Result.X = Left.X * Right;
    Result.Y = Left.Y * Right;
    Result.Z = Left.Z * Right;
    Result.W = Left.W * Right;
#endif
    
    return (Result);
}

inline_internal vec2
DivideVec2f(vec2 Left, r32 Right)
{
    
    vec2 Result;
    
    Result.X = Left.X / Right;
    Result.Y = Left.Y / Right;
    
    return (Result);
}

inline_internal vec3
DivideVec3f(vec3 Left, r32 Right)
{
    
    vec3 Result;
    
    Result.X = Left.X / Right;
    Result.Y = Left.Y / Right;
    Result.Z = Left.Z / Right;
    
    return (Result);
}

inline_internal vec4
DivideVec4f(vec4 Left, r32 Right)
{
    
    vec4 Result;
    
#ifdef MATH__USE_SSE
    __m128 Scalar = _mm_set1_ps(Right);
    Result.InternalElementsSSE = _mm_div_ps(Left.InternalElementsSSE, Scalar);
#else
    Result.X = Left.X / Right;
    Result.Y = Left.Y / Right;
    Result.Z = Left.Z / Right;
    Result.W = Left.W / Right;
#endif
    
    return (Result);
}

inline_internal b32
EqualsVec2(vec2 Left, vec2 Right)
{
    
    b32 Result = (Left.X == Right.X && Left.Y == Right.Y);
    
    return (Result);
}

inline_internal b32
EqualsVec3(vec3 Left, vec3 Right)
{
    
    b32 Result = (Left.X == Right.X && Left.Y == Right.Y && Left.Z == Right.Z);
    
    return (Result);
}

inline_internal b32
EqualsVec4(vec4 Left, vec4 Right)
{
    
    b32 Result = (Left.X == Right.X && Left.Y == Right.Y && Left.Z == Right.Z && Left.W == Right.W);
    
    return (Result);
}

inline_internal r32
DotVec2(vec2 VecOne, vec2 VecTwo)
{
    
    r32 Result = (VecOne.X * VecTwo.X) + (VecOne.Y * VecTwo.Y);
    
    return (Result);
}

inline_internal r32
DotVec3(vec3 VecOne, vec3 VecTwo)
{
    
    r32 Result = (VecOne.X * VecTwo.X) + (VecOne.Y * VecTwo.Y) + (VecOne.Z * VecTwo.Z);
    
    return (Result);
}

inline_internal r32
DotVec4(vec4 VecOne, vec4 VecTwo)
{
    
    r32 Result;
    
    // NOTE(zak): IN the future if we wanna check what version SSE is support
    // we can use _mm_dp_ps (4.3) but for now we will use the old way.
    // Or a r = _mm_mul_ps(v1, v2), r = _mm_hadd_ps(r, r), r = _mm_hadd_ps(r, r) for SSE3
#ifdef MATH__USE_SSE
    __m128 SSEResultOne = _mm_mul_ps(VecOne.InternalElementsSSE, VecTwo.InternalElementsSSE);
    __m128 SSEResultTwo = _mm_shuffle_ps(SSEResultOne, SSEResultOne, _MM_SHUFFLE(2, 3, 0, 1));
    SSEResultOne = _mm_add_ps(SSEResultOne, SSEResultTwo);
    SSEResultTwo = _mm_shuffle_ps(SSEResultOne, SSEResultOne, _MM_SHUFFLE(0, 1, 2, 3));
    SSEResultOne = _mm_add_ps(SSEResultOne, SSEResultTwo);
    _mm_store_ss(&Result, SSEResultOne);
#else
    Result = (VecOne.X * VecTwo.X) + (VecOne.Y * VecTwo.Y) + (VecOne.Z * VecTwo.Z) + (VecOne.W * VecTwo.W);
#endif
    
    return (Result);
}

inline_internal vec3
Cross(vec3 VecOne, vec3 VecTwo)
{
    
    vec3 Result;
    
    Result.X = (VecOne.Y * VecTwo.Z) - (VecOne.Z * VecTwo.Y);
    Result.Y = (VecOne.Z * VecTwo.X) - (VecOne.X * VecTwo.Z);
    Result.Z = (VecOne.X * VecTwo.Y) - (VecOne.Y * VecTwo.X);
    
    return (Result);
}


/*
 * Unary vector operations
 */

inline_internal r32
LengthSquaredVec2(vec2 A)
{
    
    r32 Result = DotVec2(A, A);
    
    return (Result);
}

inline_internal r32
LengthSquaredVec3(vec3 A)
{
    
    r32 Result = DotVec3(A, A);
    
    return (Result);
}

inline_internal r32
LengthSquaredVec4(vec4 A)
{
    
    r32 Result = DotVec4(A, A);
    
    return (Result);
}

inline_internal r32
LengthVec2(vec2 A)
{
    
    r32 Result = SquareRootF(LengthSquaredVec2(A));
    
    return (Result);
}

inline_internal r32
LengthVec3(vec3 A)
{
    
    r32 Result = SquareRootF(LengthSquaredVec3(A));
    
    return (Result);
}

inline_internal r32
LengthVec4(vec4 A)
{
    
    r32 Result = SquareRootF(LengthSquaredVec4(A));
    
    return(Result);
}

inline_internal vec2
NormalizeVec2(vec2 A)
{
    
    vec2 Result = {0};
    
    r32 VectorLength = LengthVec2(A);
    
    /* NOTE(kiljacken): We need a zero check to not divide-by-zero */
    if (VectorLength != 0.0f)
    {
        
        Result.X = A.X * (1.0f / VectorLength);
        Result.Y = A.Y * (1.0f / VectorLength);
    }
    
    return (Result);
}

inline_internal vec3
NormalizeVec3(vec3 A)
{
    
    vec3 Result = {0};
    
    r32 VectorLength = LengthVec3(A);
    
    /* NOTE(kiljacken): We need a zero check to not divide-by-zero */
    if (VectorLength != 0.0f)
    {
        
        Result.X = A.X * (1.0f / VectorLength);
        Result.Y = A.Y * (1.0f / VectorLength);
        Result.Z = A.Z * (1.0f / VectorLength);
    }
    
    return (Result);
}

inline_internal vec4
NormalizeVec4(vec4 A)
{
    
    vec4 Result = {0};
    
    r32 VectorLength = LengthVec4(A);
    
    /* NOTE(kiljacken): We need a zero check to not divide-by-zero */
    if (VectorLength != 0.0f)
    {
        
        r32 Multiplier = 1.0f / VectorLength;
        
#ifdef MATH__USE_SSE
        __m128 SSEMultiplier = _mm_set1_ps(Multiplier);
        Result.InternalElementsSSE = _mm_mul_ps(A.InternalElementsSSE, SSEMultiplier);
#else
        Result.X = A.X * Multiplier;
        Result.Y = A.Y * Multiplier;
        Result.Z = A.Z * Multiplier;
        Result.W = A.W * Multiplier;
#endif
    }
    
    return (Result);
}

inline_internal vec2
FastNormalizeVec2(vec2 A)
{
    
    return MultiplyVec2f(A, RSquareRootF(DotVec2(A, A)));
}

inline_internal vec3
FastNormalizeVec3(vec3 A)
{
    
    return MultiplyVec3f(A, RSquareRootF(DotVec3(A, A)));
}

inline_internal vec4
FastNormalizeVec4(vec4 A)
{
    
    return MultiplyVec4f(A, RSquareRootF(DotVec4(A, A)));
}


/*
 * SSE stuff
 */

#ifdef MATH__USE_SSE
inline_internal __m128
LinearCombineSSE(__m128 Left, mat4 Right)
{
    
    __m128 Result;
    Result = _mm_mul_ps(_mm_shuffle_ps(Left, Left, 0x00), Right.Columns[0]);
    Result = _mm_add_ps(Result, _mm_mul_ps(_mm_shuffle_ps(Left, Left, 0x55), Right.Columns[1]));
    Result = _mm_add_ps(Result, _mm_mul_ps(_mm_shuffle_ps(Left, Left, 0xaa), Right.Columns[2]));
    Result = _mm_add_ps(Result, _mm_mul_ps(_mm_shuffle_ps(Left, Left, 0xff), Right.Columns[3]));
    
    return (Result);
}
#endif


/*
 * Matrix functions
 */

inline_internal mat4
Mat4(void)
{
    
    mat4 Result = {0};
    
    return (Result);
}

inline_internal mat4
Mat4d(r32 Diagonal)
{
    
    mat4 Result = Mat4();
    
    Result.Elements[0][0] = Diagonal;
    Result.Elements[1][1] = Diagonal;
    Result.Elements[2][2] = Diagonal;
    Result.Elements[3][3] = Diagonal;
    
    return (Result);
}

inline_internal mat4
Transpose(mat4 Matrix)
{
    
    mat4 Result = Matrix;
    
#ifdef MATH__USE_SSE
    _MM_TRANSPOSE4_PS(Result.Columns[0], Result.Columns[1], Result.Columns[2], Result.Columns[3]);
#else
    int Columns;
    for(Columns = 0; Columns < 4; ++Columns)
    {
        int Rows;
        for(Rows = 0; Rows < 4; ++Rows)
        {
            Result.Elements[Rows][Columns] = Matrix.Elements[Columns][Rows];
        }
    }
#endif
    
    
    return (Result);
}

inline_internal mat4
AddMat4(mat4 Left, mat4 Right)
{
    
    mat4 Result;
    
#ifdef MATH__USE_SSE
    Result.Columns[0] = _mm_add_ps(Left.Columns[0], Right.Columns[0]);
    Result.Columns[1] = _mm_add_ps(Left.Columns[1], Right.Columns[1]);
    Result.Columns[2] = _mm_add_ps(Left.Columns[2], Right.Columns[2]);
    Result.Columns[3] = _mm_add_ps(Left.Columns[3], Right.Columns[3]);
#else
    int Columns;
    for(Columns = 0; Columns < 4; ++Columns)
    {
        int Rows;
        for(Rows = 0; Rows < 4; ++Rows)
        {
            Result.Elements[Columns][Rows] = Left.Elements[Columns][Rows] + Right.Elements[Columns][Rows];
        }
    }
#endif
    
    
    return (Result);
}

inline_internal mat4
SubtractMat4(mat4 Left, mat4 Right)
{
    
    mat4 Result;
    
#ifdef MATH__USE_SSE
    Result.Columns[0] = _mm_sub_ps(Left.Columns[0], Right.Columns[0]);
    Result.Columns[1] = _mm_sub_ps(Left.Columns[1], Right.Columns[1]);
    Result.Columns[2] = _mm_sub_ps(Left.Columns[2], Right.Columns[2]);
    Result.Columns[3] = _mm_sub_ps(Left.Columns[3], Right.Columns[3]);
#else
    int Columns;
    for(Columns = 0; Columns < 4; ++Columns)
    {
        int Rows;
        for(Rows = 0; Rows < 4; ++Rows)
        {
            Result.Elements[Columns][Rows] = Left.Elements[Columns][Rows] - Right.Elements[Columns][Rows];
        }
    }
#endif
    
    return (Result);
}

inline_internal mat4
MultiplyMat4(mat4 Left, mat4 Right)
{
    
    mat4 Result;
    
#ifdef MATH__USE_SSE
    Result.Columns[0] = LinearCombineSSE(Right.Columns[0], Left);
    Result.Columns[1] = LinearCombineSSE(Right.Columns[1], Left);
    Result.Columns[2] = LinearCombineSSE(Right.Columns[2], Left);
    Result.Columns[3] = LinearCombineSSE(Right.Columns[3], Left);
#else
    int Columns;
    for(Columns = 0; Columns < 4; ++Columns)
    {
        int Rows;
        for(Rows = 0; Rows < 4; ++Rows)
        {
            r32 Sum = 0;
            int CurrentMatrice;
            for(CurrentMatrice = 0; CurrentMatrice < 4; ++CurrentMatrice)
            {
                Sum += Left.Elements[CurrentMatrice][Rows] * Right.Elements[Columns][CurrentMatrice];
            }
            
            Result.Elements[Columns][Rows] = Sum;
        }
    }
#endif
    
    return (Result);
}


inline_internal mat4
MultiplyMat4f(mat4 Matrix, r32 Scalar)
{
    
    mat4 Result;
    
#ifdef MATH__USE_SSE
    __m128 SSEScalar = _mm_set1_ps(Scalar);
    Result.Columns[0] = _mm_mul_ps(Matrix.Columns[0], SSEScalar);
    Result.Columns[1] = _mm_mul_ps(Matrix.Columns[1], SSEScalar);
    Result.Columns[2] = _mm_mul_ps(Matrix.Columns[2], SSEScalar);
    Result.Columns[3] = _mm_mul_ps(Matrix.Columns[3], SSEScalar);
#else
    int Columns;
    for(Columns = 0; Columns < 4; ++Columns)
    {
        int Rows;
        for(Rows = 0; Rows < 4; ++Rows)
        {
            Result.Elements[Columns][Rows] = Matrix.Elements[Columns][Rows] * Scalar;
        }
    }
#endif
    
    return (Result);
}

inline_internal vec4
MultiplyMat4ByVec4(mat4 Matrix, vec4 Vector)
{
    
    vec4 Result;
    
#ifdef MATH__USE_SSE
    Result.InternalElementsSSE = LinearCombineSSE(Vector.InternalElementsSSE, Matrix);
#else
    int Columns, Rows;
    for(Rows = 0; Rows < 4; ++Rows)
    {
        r32 Sum = 0;
        for(Columns = 0; Columns < 4; ++Columns)
        {
            Sum += Matrix.Elements[Columns][Rows] * Vector.Elements[Columns];
        }
        
        Result.Elements[Rows] = Sum;
    }
#endif
    
    return (Result);
}


inline_internal mat4
DivideMat4f(mat4 Matrix, r32 Scalar)
{
    
    mat4 Result;
    
#ifdef MATH__USE_SSE
    __m128 SSEScalar = _mm_set1_ps(Scalar);
    Result.Columns[0] = _mm_div_ps(Matrix.Columns[0], SSEScalar);
    Result.Columns[1] = _mm_div_ps(Matrix.Columns[1], SSEScalar);
    Result.Columns[2] = _mm_div_ps(Matrix.Columns[2], SSEScalar);
    Result.Columns[3] = _mm_div_ps(Matrix.Columns[3], SSEScalar);
#else
    int Columns;
    for(Columns = 0; Columns < 4; ++Columns)
    {
        int Rows;
        for(Rows = 0; Rows < 4; ++Rows)
        {
            Result.Elements[Columns][Rows] = Matrix.Elements[Columns][Rows] / Scalar;
        }
    }
#endif
    
    return (Result);
}

/*
 * Common graphics transformations
 */

inline_internal mat4
Orthographic(r32 Left, r32 Right, r32 Bottom, r32 Top, r32 Near, r32 Far)
{
    
    mat4 Result = Mat4();
    
    Result.Elements[0][0] = 2.0f / (Right - Left);
    Result.Elements[1][1] = 2.0f / (Top - Bottom);
    Result.Elements[2][2] = 2.0f / (Near - Far);
    Result.Elements[3][3] = 1.0f;
    
    Result.Elements[3][0] = (Left + Right) / (Left - Right);
    Result.Elements[3][1] = (Bottom + Top) / (Bottom - Top);
    Result.Elements[3][2] = (Far + Near) / (Near - Far);
    
    return (Result);
}

inline_internal mat4
Perspective(r32 FOV, r32 AspectRatio, r32 Near, r32 Far)
{
    
    mat4 Result = Mat4();
    
    // See https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml
    
    r32 Cotangent = 1.0f / TanF(FOV * (PI32 / 360.0f));
    
    Result.Elements[0][0] = Cotangent / AspectRatio;
    Result.Elements[1][1] = Cotangent;
    Result.Elements[2][3] = -1.0f;
    Result.Elements[2][2] = (Near + Far) / (Near - Far);
    Result.Elements[3][2] = (2.0f * Near * Far) / (Near - Far);
    Result.Elements[3][3] = 0.0f;
    
    return (Result);
}

inline_internal mat4
Translate(vec3 Translation)
{
    
    mat4 Result = Mat4d(1.0f);
    
    Result.Elements[3][0] = Translation.X;
    Result.Elements[3][1] = Translation.Y;
    Result.Elements[3][2] = Translation.Z;
    
    return (Result);
}

inline_internal mat4
Rotate(r32 Angle, vec3 Axis)
{
    
    mat4 Result = Mat4d(1.0f);
    
    Axis = NormalizeVec3(Axis);
    
    r32 SinTheta = SinF(ToRadians(Angle));
    r32 CosTheta = CosF(ToRadians(Angle));
    r32 CosValue = 1.0f - CosTheta;
    
    Result.Elements[0][0] = (Axis.X * Axis.X * CosValue) + CosTheta;
    Result.Elements[0][1] = (Axis.X * Axis.Y * CosValue) + (Axis.Z * SinTheta);
    Result.Elements[0][2] = (Axis.X * Axis.Z * CosValue) - (Axis.Y * SinTheta);
    
    Result.Elements[1][0] = (Axis.Y * Axis.X * CosValue) - (Axis.Z * SinTheta);
    Result.Elements[1][1] = (Axis.Y * Axis.Y * CosValue) + CosTheta;
    Result.Elements[1][2] = (Axis.Y * Axis.Z * CosValue) + (Axis.X * SinTheta);
    
    Result.Elements[2][0] = (Axis.Z * Axis.X * CosValue) + (Axis.Y * SinTheta);
    Result.Elements[2][1] = (Axis.Z * Axis.Y * CosValue) - (Axis.X * SinTheta);
    Result.Elements[2][2] = (Axis.Z * Axis.Z * CosValue) + CosTheta;
    
    return (Result);
}



inline_internal mat4
ScaleBy(vec3 Scale)
{
    
    mat4 Result = Mat4d(1.0f);
    
    Result.Elements[0][0] = Scale.X;
    Result.Elements[1][1] = Scale.Y;
    Result.Elements[2][2] = Scale.Z;
    
    return (Result);
}

inline_internal mat4
LookAt(vec3 Eye, vec3 Center, vec3 Up)
{
    
    mat4 Result;
    
    vec3 F = NormalizeVec3(SubtractVec3(Center, Eye));
    vec3 S = NormalizeVec3(Cross(F, Up));
    vec3 U = Cross(S, F);
    
    Result.Elements[0][0] = S.X;
    Result.Elements[0][1] = U.X;
    Result.Elements[0][2] = -F.X;
    Result.Elements[0][3] = 0.0f;
    
    Result.Elements[1][0] = S.Y;
    Result.Elements[1][1] = U.Y;
    Result.Elements[1][2] = -F.Y;
    Result.Elements[1][3] = 0.0f;
    
    Result.Elements[2][0] = S.Z;
    Result.Elements[2][1] = U.Z;
    Result.Elements[2][2] = -F.Z;
    Result.Elements[2][3] = 0.0f;
    
    Result.Elements[3][0] = -DotVec3(S, Eye);
    Result.Elements[3][1] = -DotVec3(U, Eye);
    Result.Elements[3][2] = DotVec3(F, Eye);
    Result.Elements[3][3] = 1.0f;
    
    return (Result);
}

/*
 * Quaternion operations
 */

inline_internal quaternion
Quaternion(r32 X, r32 Y, r32 Z, r32 W)
{
    
    quaternion Result;
    
#ifdef MATH__USE_SSE
    Result.InternalElementsSSE = _mm_setr_ps(X, Y, Z, W);
#else
    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;
    Result.W = W;
#endif
    
    return (Result);
}

inline_internal quaternion
QuaternionV4(vec4 Vector)
{
    
    quaternion Result;
    
#ifdef MATH__USE_SSE
    Result.InternalElementsSSE = Vector.InternalElementsSSE;
#else
    Result.X = Vector.X;
    Result.Y = Vector.Y;
    Result.Z = Vector.Z;
    Result.W = Vector.W;
#endif
    
    return (Result);
}

inline_internal quaternion
AddQuaternion(quaternion Left, quaternion Right)
{
    
    quaternion Result;
    
#ifdef MATH__USE_SSE
    Result.InternalElementsSSE = _mm_add_ps(Left.InternalElementsSSE, Right.InternalElementsSSE);
#else
    
    Result.X = Left.X + Right.X;
    Result.Y = Left.Y + Right.Y;
    Result.Z = Left.Z + Right.Z;
    Result.W = Left.W + Right.W;
#endif
    
    return (Result);
}

inline_internal quaternion
SubtractQuaternion(quaternion Left, quaternion Right)
{
    
    quaternion Result;
    
#ifdef MATH__USE_SSE
    Result.InternalElementsSSE = _mm_sub_ps(Left.InternalElementsSSE, Right.InternalElementsSSE);
#else
    
    Result.X = Left.X - Right.X;
    Result.Y = Left.Y - Right.Y;
    Result.Z = Left.Z - Right.Z;
    Result.W = Left.W - Right.W;
#endif
    
    return (Result);
}

inline_internal quaternion
MultiplyQuaternion(quaternion Left, quaternion Right)
{
    
    quaternion Result;
    
#ifdef MATH__USE_SSE
    __m128 SSEResultOne = _mm_xor_ps(_mm_shuffle_ps(Left.InternalElementsSSE, Left.InternalElementsSSE, _MM_SHUFFLE(0, 0, 0, 0)), _mm_setr_ps(0.f, -0.f, 0.f, -0.f));
    __m128 SSEResultTwo = _mm_shuffle_ps(Right.InternalElementsSSE, Right.InternalElementsSSE, _MM_SHUFFLE(0, 1, 2, 3));
    __m128 SSEResultThree = _mm_mul_ps(SSEResultTwo, SSEResultOne);
    
    SSEResultOne = _mm_xor_ps(_mm_shuffle_ps(Left.InternalElementsSSE, Left.InternalElementsSSE, _MM_SHUFFLE(1, 1, 1, 1)) , _mm_setr_ps(0.f, 0.f, -0.f, -0.f));
    SSEResultTwo = _mm_shuffle_ps(Right.InternalElementsSSE, Right.InternalElementsSSE, _MM_SHUFFLE(1, 0, 3, 2));
    SSEResultThree = _mm_add_ps(SSEResultThree, _mm_mul_ps(SSEResultTwo, SSEResultOne));
    
    SSEResultOne = _mm_xor_ps(_mm_shuffle_ps(Left.InternalElementsSSE, Left.InternalElementsSSE, _MM_SHUFFLE(2, 2, 2, 2)), _mm_setr_ps(-0.f, 0.f, 0.f, -0.f));
    SSEResultTwo = _mm_shuffle_ps(Right.InternalElementsSSE, Right.InternalElementsSSE, _MM_SHUFFLE(2, 3, 0, 1));
    SSEResultThree = _mm_add_ps(SSEResultThree, _mm_mul_ps(SSEResultTwo, SSEResultOne));
    
    SSEResultOne = _mm_shuffle_ps(Left.InternalElementsSSE, Left.InternalElementsSSE, _MM_SHUFFLE(3, 3, 3, 3));
    SSEResultTwo = _mm_shuffle_ps(Right.InternalElementsSSE, Right.InternalElementsSSE, _MM_SHUFFLE(3, 2, 1, 0));
    Result.InternalElementsSSE = _mm_add_ps(SSEResultThree, _mm_mul_ps(SSEResultTwo, SSEResultOne));
#else
    Result.X = (Left.X * Right.W) + (Left.Y * Right.Z) - (Left.Z * Right.Y) + (Left.W * Right.X);
    Result.Y = (-Left.X * Right.Z) + (Left.Y * Right.W) + (Left.Z * Right.X) + (Left.W * Right.Y);
    Result.Z = (Left.X * Right.Y) - (Left.Y * Right.X) + (Left.Z * Right.W) + (Left.W * Right.Z);
    Result.W = (-Left.X * Right.X) - (Left.Y * Right.Y) - (Left.Z * Right.Z) + (Left.W * Right.W);
#endif
    
    return (Result);
}

inline_internal quaternion
MultiplyQuaternionF(quaternion Left, r32 Multiplicative)
{
    
    quaternion Result;
    
#ifdef MATH__USE_SSE
    __m128 Scalar = _mm_set1_ps(Multiplicative);
    Result.InternalElementsSSE = _mm_mul_ps(Left.InternalElementsSSE, Scalar);
#else
    Result.X = Left.X * Multiplicative;
    Result.Y = Left.Y * Multiplicative;
    Result.Z = Left.Z * Multiplicative;
    Result.W = Left.W * Multiplicative;
#endif
    
    return (Result);
}

inline_internal quaternion
DivideQuaternionF(quaternion Left, r32 Dividend)
{
    
    quaternion Result;
    
#ifdef MATH__USE_SSE
    __m128 Scalar = _mm_set1_ps(Dividend);
    Result.InternalElementsSSE = _mm_div_ps(Left.InternalElementsSSE, Scalar);
#else
    Result.X = Left.X / Dividend;
    Result.Y = Left.Y / Dividend;
    Result.Z = Left.Z / Dividend;
    Result.W = Left.W / Dividend;
#endif
    
    return (Result);
}

inline_internal r32
DotQuaternion(quaternion Left, quaternion Right)
{
    
    r32 Result;
    
#ifdef MATH__USE_SSE
    __m128 SSEResultOne = _mm_mul_ps(Left.InternalElementsSSE, Right.InternalElementsSSE);
    __m128 SSEResultTwo = _mm_shuffle_ps(SSEResultOne, SSEResultOne, _MM_SHUFFLE(2, 3, 0, 1));
    SSEResultOne = _mm_add_ps(SSEResultOne, SSEResultTwo);
    SSEResultTwo = _mm_shuffle_ps(SSEResultOne, SSEResultOne, _MM_SHUFFLE(0, 1, 2, 3));
    SSEResultOne = _mm_add_ps(SSEResultOne, SSEResultTwo);
    _mm_store_ss(&Result, SSEResultOne);
#else
    Result = (Left.X * Right.X) + (Left.Y * Right.Y) + (Left.Z * Right.Z) + (Left.W * Right.W);
#endif
    
    return (Result);
}


inline_internal quaternion
InverseQuaternion(quaternion Left)
{
    
    quaternion Result;
    
    Result.X = -Left.X;
    Result.Y = -Left.Y;
    Result.Z = -Left.Z;
    Result.W = Left.W;
    
    Result = DivideQuaternionF(Result, (DotQuaternion(Left, Left)));
    
    return (Result);
}


inline_internal quaternion
NormalizeQuaternion(quaternion Left)
{
    
    quaternion Result;
    
    r32 Length = SquareRootF(DotQuaternion(Left, Left));
    Result = DivideQuaternionF(Left, Length);
    
    return (Result);
}

inline_internal quaternion
NLerp(quaternion Left, r32 Time, quaternion Right)
{
    
    quaternion Result;
    
#ifdef MATH__USE_SSE
    __m128 ScalarLeft = _mm_set1_ps(1.0f - Time);
    __m128 ScalarRight = _mm_set1_ps(Time);
    __m128 SSEResultOne = _mm_mul_ps(Left.InternalElementsSSE, ScalarLeft);
    __m128 SSEResultTwo = _mm_mul_ps(Right.InternalElementsSSE, ScalarRight);
    Result.InternalElementsSSE = _mm_add_ps(SSEResultOne, SSEResultTwo);
#else
    Result.X = Lerp(Left.X, Time, Right.X);
    Result.Y = Lerp(Left.Y, Time, Right.Y);
    Result.Z = Lerp(Left.Z, Time, Right.Z);
    Result.W = Lerp(Left.W, Time, Right.W);
#endif
    Result = NormalizeQuaternion(Result);
    
    return (Result);
}

inline_internal quaternion
Slerp(quaternion Left, r32 Time, quaternion Right)
{
    
    quaternion Result;
    quaternion QuaternionLeft;
    quaternion QuaternionRight;
    
    r32 Cos_Theta = DotQuaternion(Left, Right);
    r32 Angle = ACosF(Cos_Theta);
    
    r32 S1 = SinF((1.0f - Time) * Angle);
    r32 S2 = SinF(Time * Angle);
    r32 Is = 1.0f / SinF(Angle);
    
    QuaternionLeft = MultiplyQuaternionF(Left, S1);
    QuaternionRight = MultiplyQuaternionF(Right, S2);
    
    Result = AddQuaternion(QuaternionLeft, QuaternionRight);
    Result = MultiplyQuaternionF(Result, Is);
    
    return (Result);
}

inline_internal mat4
QuaternionToMat4(quaternion Left)
{
    
    mat4 Result;
    
    quaternion NormalizedQuaternion = NormalizeQuaternion(Left);
    
    r32 XX, YY, ZZ,
    XY, XZ, YZ,
    WX, WY, WZ;
    
    XX = NormalizedQuaternion.X * NormalizedQuaternion.X;
    YY = NormalizedQuaternion.Y * NormalizedQuaternion.Y;
    ZZ = NormalizedQuaternion.Z * NormalizedQuaternion.Z;
    XY = NormalizedQuaternion.X * NormalizedQuaternion.Y;
    XZ = NormalizedQuaternion.X * NormalizedQuaternion.Z;
    YZ = NormalizedQuaternion.Y * NormalizedQuaternion.Z;
    WX = NormalizedQuaternion.W * NormalizedQuaternion.X;
    WY = NormalizedQuaternion.W * NormalizedQuaternion.Y;
    WZ = NormalizedQuaternion.W * NormalizedQuaternion.Z;
    
    Result.Elements[0][0] = 1.0f - 2.0f * (YY + ZZ);
    Result.Elements[0][1] = 2.0f * (XY + WZ);
    Result.Elements[0][2] = 2.0f * (XZ - WY);
    Result.Elements[0][3] = 0.0f;
    
    Result.Elements[1][0] = 2.0f * (XY - WZ);
    Result.Elements[1][1] = 1.0f - 2.0f * (XX + ZZ);
    Result.Elements[1][2] = 2.0f * (YZ + WX);
    Result.Elements[1][3] = 0.0f;
    
    Result.Elements[2][0] = 2.0f * (XZ + WY);
    Result.Elements[2][1] = 2.0f * (YZ - WX);
    Result.Elements[2][2] = 1.0f - 2.0f * (XX + YY);
    Result.Elements[2][3] = 0.0f;
    
    Result.Elements[3][0] = 0.0f;
    Result.Elements[3][1] = 0.0f;
    Result.Elements[3][2] = 0.0f;
    Result.Elements[3][3] = 1.0f;
    
    return (Result);
}

// This method taken from Mike Day at Insomniac Games.
// https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf
//
// Note that as mentioned at the top of the paper, the paper assumes the matrix
// would be *post*-multiplied to a vector to rotate it, meaning the matrix is
// the transpose of what we're dealing with. But, because our matrices are
// stored in column-major order, the indices *appear* to match the paper.
//
// For example, m12 in the paper is row 1, column 2. We need to transpose it to
// row 2, column 1. But, because the column comes first when referencing
// elements, it looks like M.Elements[1][2].
//
// Don't be confused! Or if you must be confused, at least trust this
// comment. :)
inline_internal quaternion
Mat4ToQuaternion(mat4 M)
{
    r32 T;
    quaternion Q;
    
    if (M.Elements[2][2] < 0.0f) {
        if (M.Elements[0][0] > M.Elements[1][1]) {
            
            T = 1 + M.Elements[0][0] - M.Elements[1][1] - M.Elements[2][2];
            Q = Quaternion(
                           T,
                           M.Elements[0][1] + M.Elements[1][0],
                           M.Elements[2][0] + M.Elements[0][2],
                           M.Elements[1][2] - M.Elements[2][1]
                           );
        } else {
            
            T = 1 - M.Elements[0][0] + M.Elements[1][1] - M.Elements[2][2];
            Q = Quaternion(
                           M.Elements[0][1] + M.Elements[1][0],
                           T,
                           M.Elements[1][2] + M.Elements[2][1],
                           M.Elements[2][0] - M.Elements[0][2]
                           );
        }
    } else {
        if (M.Elements[0][0] < -M.Elements[1][1]) {
            
            T = 1 - M.Elements[0][0] - M.Elements[1][1] + M.Elements[2][2];
            Q = Quaternion(
                           M.Elements[2][0] + M.Elements[0][2],
                           M.Elements[1][2] + M.Elements[2][1],
                           T,
                           M.Elements[0][1] - M.Elements[1][0]
                           );
        } else {
            
            T = 1 + M.Elements[0][0] + M.Elements[1][1] + M.Elements[2][2];
            Q = Quaternion(
                           M.Elements[1][2] - M.Elements[2][1],
                           M.Elements[2][0] - M.Elements[0][2],
                           M.Elements[0][1] - M.Elements[1][0],
                           T
                           );
        }
    }
    
    Q = MultiplyQuaternionF(Q, 0.5f / SquareRootF(T));
    
    return Q;
}

inline_internal quaternion
QuaternionFromAxisAngle(vec3 Axis, r32 AngleOfRotation)
{
    
    quaternion Result;
    
    vec3 AxisNormalized = NormalizeVec3(Axis);
    r32 SineOfRotation = SinF(AngleOfRotation / 2.0f);
    
    Result.XYZ = MultiplyVec3f(AxisNormalized, SineOfRotation);
    Result.W = CosF(AngleOfRotation / 2.0f);
    
    return (Result);
}


//- NOTE(fakhri): rectangle Initializations 

internal inline rectangle_2d
RectMinMax(v2r32 MinP, v2r32 MaxP)
{
    rectangle_2d result;
    result.MinP = MinP;
    result.MaxP = MaxP;
    return result;
}

internal inline rectangle_2d
RectTwoPoints(v2r32 a, v2r32 b)
{
    rectangle_2d result;
    result.MinP = Vec2(MIN(a.X, b.X), MIN(a.Y, b.Y));
    result.MaxP = Vec2(MAX(a.X, b.X), MAX(a.Y, b.Y));
    return result;
}

internal inline rectangle_2d
RectCentHalfDim(v2r32 center, v2r32 half_dim)
{
    rectangle_2d result = RectMinMax(SubtractVec2(center, half_dim), 
                                     SubtractVec2(center, half_dim));
    return result;
}

internal inline rectangle_2d
RectCentDim(v2r32 center, v2r32 dim)
{
    rectangle_2d result = RectCentHalfDim(center, MultiplyVec2f(dim, 0.5f));
    return result;
}

internal inline b32
IsInsideRect(rectangle_2d rect, v2r32 pos)
{
    b32 result = ((pos.X >= rect.MinP.X && pos.X < rect.MaxP.X) &&
                  (pos.Y >= rect.MinP.Y && pos.Y < rect.MaxP.Y));
    return result;
}

internal inline v2r32
ClampInsideRect(rectangle_2d rect, v2r32 p)
{
    v2r32 result;
    result.X = Clamp(rect.MinP.X, p.X, rect.MaxP.X);
    result.Y = Clamp(rect.MinP.Y, p.Y, rect.MaxP.Y);
    return result;
}

internal inline v2r32
RectDim(rectangle_2d rect)
{
    v2r32 result = Vec2(AbsVal(rect.MaxP.X - rect.MinP.X),
                        AbsVal(rect.MaxP.Y - rect.MinP.Y));
    return result;
}



//~
// Converting
//

inline_internal vec2
Vec2FromVec2i32(vec2i32 a)
{
    vec2 result = Vec2((r32)a.X, (r32)a.Y);
    return result;
}


inline_internal r32
MoveTowards(r32 start, r32 end, r32 amount)
{
    if (start > end)
    {
        amount = -amount;
    }
    r32 result = start + amount;
    result = Clamp(MIN(start, end), result, MAX(start, end));
    return result;
}

inline_internal r32
MapIntoRange01(r32 Min, r32 Value, r32 Max)
{
    r32 Result = (Value - Min) / (Max - Min);
    return Result;
}

#if LANG_CPP

inline_internal r32
Length(vec2 A)
{
    
    r32 Result = LengthVec2(A);
    return (Result);
}

inline_internal r32
Length(vec3 A)
{
    
    r32 Result = LengthVec3(A);
    
    return (Result);
}

inline_internal r32
Length(vec4 A)
{
    
    r32 Result = LengthVec4(A);
    
    return (Result);
}

inline_internal r32
LengthSquared(vec2 A)
{
    
    r32 Result = LengthSquaredVec2(A);
    
    return (Result);
}

inline_internal r32
LengthSquared(vec3 A)
{
    
    r32 Result = LengthSquaredVec3(A);
    
    return (Result);
}

inline_internal r32
LengthSquared(vec4 A)
{
    
    r32 Result = LengthSquaredVec4(A);
    
    return (Result);
}

inline_internal vec2
Normalize(vec2 A)
{
    
    vec2 Result = NormalizeVec2(A);
    
    return (Result);
}

inline_internal vec3
Normalize(vec3 A)
{
    
    vec3 Result = NormalizeVec3(A);
    
    return (Result);
}

inline_internal vec4
Normalize(vec4 A)
{
    
    vec4 Result = NormalizeVec4(A);
    
    return (Result);
}

inline_internal vec2
FastNormalize(vec2 A)
{
    
    vec2 Result = FastNormalizeVec2(A);
    
    return (Result);
}

inline_internal vec3
FastNormalize(vec3 A)
{
    
    vec3 Result = FastNormalizeVec3(A);
    
    return (Result);
}

inline_internal vec4
FastNormalize(vec4 A)
{
    
    vec4 Result = FastNormalizeVec4(A);
    
    return (Result);
}

inline_internal quaternion
Normalize(quaternion A)
{
    
    quaternion Result = NormalizeQuaternion(A);
    
    return (Result);
}

inline_internal r32
Dot(vec2 VecOne, vec2 VecTwo)
{
    
    r32 Result = DotVec2(VecOne, VecTwo);
    
    return (Result);
}

inline_internal r32
Dot(vec3 VecOne, vec3 VecTwo)
{
    
    r32 Result = DotVec3(VecOne, VecTwo);
    
    return (Result);
}

inline_internal r32
Dot(vec4 VecOne, vec4 VecTwo)
{
    
    r32 Result = DotVec4(VecOne, VecTwo);
    
    return (Result);
}

inline_internal r32
Dot(quaternion QuatOne, quaternion QuatTwo)
{
    
    r32 Result = DotQuaternion(QuatOne, QuatTwo);
    
    return (Result);
}

inline_internal vec2
Add(vec2 Left, vec2 Right)
{
    
    vec2 Result = AddVec2(Left, Right);
    
    return (Result);
}

inline_internal vec3
Add(vec3 Left, vec3 Right)
{
    
    vec3 Result = AddVec3(Left, Right);
    
    return (Result);
}

inline_internal vec4
Add(vec4 Left, vec4 Right)
{
    
    vec4 Result = AddVec4(Left, Right);
    
    return (Result);
}

inline_internal mat4
Add(mat4 Left, mat4 Right)
{
    
    mat4 Result = AddMat4(Left, Right);
    
    return (Result);
}

inline_internal quaternion
Add(quaternion Left, quaternion Right)
{
    
    quaternion Result = AddQuaternion(Left, Right);
    
    return (Result);
}

inline_internal vec2
Subtract(vec2 Left, vec2 Right)
{
    
    vec2 Result = SubtractVec2(Left, Right);
    
    return (Result);
}

inline_internal vec3
Subtract(vec3 Left, vec3 Right)
{
    
    vec3 Result = SubtractVec3(Left, Right);
    
    return (Result);
}

inline_internal vec4
Subtract(vec4 Left, vec4 Right)
{
    
    vec4 Result = SubtractVec4(Left, Right);
    
    return (Result);
}

inline_internal mat4
Subtract(mat4 Left, mat4 Right)
{
    
    mat4 Result = SubtractMat4(Left, Right);
    
    return (Result);
}

inline_internal quaternion
Subtract(quaternion Left, quaternion Right)
{
    
    quaternion Result = SubtractQuaternion(Left, Right);
    
    return (Result);
}

inline_internal vec2
Multiply(vec2 Left, vec2 Right)
{
    
    vec2 Result = MultiplyVec2(Left, Right);
    
    return (Result);
}

inline_internal vec2
Multiply(vec2 Left, r32 Right)
{
    
    vec2 Result = MultiplyVec2f(Left, Right);
    
    return (Result);
}

inline_internal vec3
Multiply(vec3 Left, vec3 Right)
{
    
    vec3 Result = MultiplyVec3(Left, Right);
    
    return (Result);
}

inline_internal vec3
Multiply(vec3 Left, r32 Right)
{
    
    vec3 Result = MultiplyVec3f(Left, Right);
    
    return (Result);
}

inline_internal vec4
Multiply(vec4 Left, vec4 Right)
{
    
    vec4 Result = MultiplyVec4(Left, Right);
    
    return (Result);
}

inline_internal vec4
Multiply(vec4 Left, r32 Right)
{
    
    vec4 Result = MultiplyVec4f(Left, Right);
    
    return (Result);
}

inline_internal mat4
Multiply(mat4 Left, mat4 Right)
{
    
    mat4 Result = MultiplyMat4(Left, Right);
    
    return (Result);
}

inline_internal mat4
Multiply(mat4 Left, r32 Right)
{
    
    mat4 Result = MultiplyMat4f(Left, Right);
    
    return (Result);
}

inline_internal vec4
Multiply(mat4 Matrix, vec4 Vector)
{
    
    vec4 Result = MultiplyMat4ByVec4(Matrix, Vector);
    
    return (Result);
}

inline_internal quaternion
Multiply(quaternion Left, quaternion Right)
{
    
    quaternion Result = MultiplyQuaternion(Left, Right);
    
    return (Result);
}

inline_internal quaternion
Multiply(quaternion Left, r32 Right)
{
    
    quaternion Result = MultiplyQuaternionF(Left, Right);
    
    return (Result);
}

inline_internal vec2
Divide(vec2 Left, vec2 Right)
{
    
    vec2 Result = DivideVec2(Left, Right);
    
    return (Result);
}

inline_internal vec2
Divide(vec2 Left, r32 Right)
{
    
    vec2 Result = DivideVec2f(Left, Right);
    
    return (Result);
}

inline_internal vec3
Divide(vec3 Left, vec3 Right)
{
    
    vec3 Result = DivideVec3(Left, Right);
    
    return (Result);
}

inline_internal vec3
Divide(vec3 Left, r32 Right)
{
    
    vec3 Result = DivideVec3f(Left, Right);
    
    return (Result);
}

inline_internal vec4
Divide(vec4 Left, vec4 Right)
{
    
    vec4 Result = DivideVec4(Left, Right);
    
    return (Result);
}

inline_internal vec4
Divide(vec4 Left, r32 Right)
{
    
    vec4 Result = DivideVec4f(Left, Right);
    
    return (Result);
}

inline_internal mat4
Divide(mat4 Left, r32 Right)
{
    
    mat4 Result = DivideMat4f(Left, Right);
    
    return (Result);
}

inline_internal quaternion
Divide(quaternion Left, r32 Right)
{
    
    quaternion Result = DivideQuaternionF(Left, Right);
    
    return (Result);
}

inline_internal b32
Equals(vec2 Left, vec2 Right)
{
    
    b32 Result = EqualsVec2(Left, Right);
    
    return (Result);
}

inline_internal b32
Equals(vec3 Left, vec3 Right)
{
    
    b32 Result = EqualsVec3(Left, Right);
    
    return (Result);
}

inline_internal b32
Equals(vec4 Left, vec4 Right)
{
    
    b32 Result = EqualsVec4(Left, Right);
    
    return (Result);
}

inline_internal vec2
operator+(vec2 Left, vec2 Right)
{
    
    vec2 Result = AddVec2(Left, Right);
    
    return (Result);
}

inline_internal vec3
operator+(vec3 Left, vec3 Right)
{
    
    vec3 Result = AddVec3(Left, Right);
    
    return (Result);
}

inline_internal vec4
operator+(vec4 Left, vec4 Right)
{
    
    vec4 Result = AddVec4(Left, Right);
    
    return (Result);
}

inline_internal mat4
operator+(mat4 Left, mat4 Right)
{
    
    mat4 Result = AddMat4(Left, Right);
    
    return (Result);
}

inline_internal quaternion
operator+(quaternion Left, quaternion Right)
{
    
    quaternion Result = AddQuaternion(Left, Right);
    
    return (Result);
}

inline_internal vec2
operator-(vec2 Left, vec2 Right)
{
    
    vec2 Result = SubtractVec2(Left, Right);
    
    return (Result);
}

inline_internal vec3
operator-(vec3 Left, vec3 Right)
{
    
    vec3 Result = SubtractVec3(Left, Right);
    
    return (Result);
}

inline_internal vec4
operator-(vec4 Left, vec4 Right)
{
    
    vec4 Result = SubtractVec4(Left, Right);
    
    return (Result);
}

inline_internal mat4
operator-(mat4 Left, mat4 Right)
{
    
    mat4 Result = SubtractMat4(Left, Right);
    
    return (Result);
}

inline_internal quaternion
operator-(quaternion Left, quaternion Right)
{
    
    quaternion Result = SubtractQuaternion(Left, Right);
    
    return (Result);
}

inline_internal vec2
operator*(vec2 Left, vec2 Right)
{
    
    vec2 Result = MultiplyVec2(Left, Right);
    
    return (Result);
}

inline_internal vec3
operator*(vec3 Left, vec3 Right)
{
    
    vec3 Result = MultiplyVec3(Left, Right);
    
    return (Result);
}

inline_internal vec4
operator*(vec4 Left, vec4 Right)
{
    
    vec4 Result = MultiplyVec4(Left, Right);
    
    return (Result);
}

inline_internal mat4
operator*(mat4 Left, mat4 Right)
{
    
    mat4 Result = MultiplyMat4(Left, Right);
    
    return (Result);
}

inline_internal quaternion
operator*(quaternion Left, quaternion Right)
{
    
    quaternion Result = MultiplyQuaternion(Left, Right);
    
    return (Result);
}

inline_internal vec2
operator*(vec2 Left, r32 Right)
{
    
    vec2 Result = MultiplyVec2f(Left, Right);
    
    return (Result);
}

inline_internal vec3
operator*(vec3 Left, r32 Right)
{
    
    vec3 Result = MultiplyVec3f(Left, Right);
    
    return (Result);
}

inline_internal vec4
operator*(vec4 Left, r32 Right)
{
    
    vec4 Result = MultiplyVec4f(Left, Right);
    
    return (Result);
}

inline_internal mat4
operator*(mat4 Left, r32 Right)
{
    
    mat4 Result = MultiplyMat4f(Left, Right);
    
    return (Result);
}

inline_internal quaternion
operator*(quaternion Left, r32 Right)
{
    
    quaternion Result = MultiplyQuaternionF(Left, Right);
    
    return (Result);
}

inline_internal vec2
operator*(r32 Left, vec2 Right)
{
    
    vec2 Result = MultiplyVec2f(Right, Left);
    
    return (Result);
}

inline_internal vec3
operator*(r32 Left, vec3 Right)
{
    
    vec3 Result = MultiplyVec3f(Right, Left);
    
    return (Result);
}

inline_internal vec4
operator*(r32 Left, vec4 Right)
{
    
    vec4 Result = MultiplyVec4f(Right, Left);
    
    return (Result);
}

inline_internal mat4
operator*(r32 Left, mat4 Right)
{
    
    mat4 Result = MultiplyMat4f(Right, Left);
    
    return (Result);
}

inline_internal quaternion
operator*(r32 Left, quaternion Right)
{
    
    quaternion Result = MultiplyQuaternionF(Right, Left);
    
    return (Result);
}

inline_internal vec4
operator*(mat4 Matrix, vec4 Vector)
{
    
    vec4 Result = MultiplyMat4ByVec4(Matrix, Vector);
    
    return (Result);
}

inline_internal vec2
operator/(vec2 Left, vec2 Right)
{
    
    vec2 Result = DivideVec2(Left, Right);
    
    return (Result);
}

inline_internal vec3
operator/(vec3 Left, vec3 Right)
{
    
    vec3 Result = DivideVec3(Left, Right);
    
    return (Result);
}

inline_internal vec4
operator/(vec4 Left, vec4 Right)
{
    
    vec4 Result = DivideVec4(Left, Right);
    
    return (Result);
}

inline_internal vec2
operator/(vec2 Left, r32 Right)
{
    
    vec2 Result = DivideVec2f(Left, Right);
    
    return (Result);
}

inline_internal vec3
operator/(vec3 Left, r32 Right)
{
    
    vec3 Result = DivideVec3f(Left, Right);
    
    return (Result);
}

inline_internal vec4
operator/(vec4 Left, r32 Right)
{
    
    vec4 Result = DivideVec4f(Left, Right);
    
    return (Result);
}

inline_internal mat4
operator/(mat4 Left, r32 Right)
{
    
    mat4 Result = DivideMat4f(Left, Right);
    
    return (Result);
}

inline_internal quaternion
operator/(quaternion Left, r32 Right)
{
    
    quaternion Result = DivideQuaternionF(Left, Right);
    
    return (Result);
}

inline_internal vec2
&operator+=(vec2 &Left, vec2 Right)
{
    
    return (Left = Left + Right);
}

inline_internal vec3
&operator+=(vec3 &Left, vec3 Right)
{
    
    return (Left = Left + Right);
}

inline_internal vec4
&operator+=(vec4 &Left, vec4 Right)
{
    
    return (Left = Left + Right);
}

inline_internal mat4
&operator+=(mat4 &Left, mat4 Right)
{
    
    return (Left = Left + Right);
}

inline_internal quaternion
&operator+=(quaternion &Left, quaternion Right)
{
    
    return (Left = Left + Right);
}

inline_internal vec2
&operator-=(vec2 &Left, vec2 Right)
{
    
    return (Left = Left - Right);
}

inline_internal vec3
&operator-=(vec3 &Left, vec3 Right)
{
    
    return (Left = Left - Right);
}

inline_internal vec4
&operator-=(vec4 &Left, vec4 Right)
{
    
    return (Left = Left - Right);
}

inline_internal mat4
&operator-=(mat4 &Left, mat4 Right)
{
    
    return (Left = Left - Right);
}

inline_internal quaternion
&operator-=(quaternion &Left, quaternion Right)
{
    
    return (Left = Left - Right);
}

inline_internal vec2
&operator*=(vec2 &Left, vec2 Right)
{
    
    return (Left = Left * Right);
}

inline_internal vec3
&operator*=(vec3 &Left, vec3 Right)
{
    
    return (Left = Left * Right);
}

inline_internal vec4
&operator*=(vec4 &Left, vec4 Right)
{
    
    return (Left = Left * Right);
}

inline_internal vec2
&operator*=(vec2 &Left, r32 Right)
{
    
    return (Left = Left * Right);
}

inline_internal vec3
&operator*=(vec3 &Left, r32 Right)
{
    
    return (Left = Left * Right);
}

inline_internal vec4
&operator*=(vec4 &Left, r32 Right)
{
    
    return (Left = Left * Right);
}

inline_internal mat4
&operator*=(mat4 &Left, r32 Right)
{
    
    return (Left = Left * Right);
}

inline_internal quaternion
&operator*=(quaternion &Left, r32 Right)
{
    
    return (Left = Left * Right);
}

inline_internal vec2
&operator/=(vec2 &Left, vec2 Right)
{
    
    return (Left = Left / Right);
}

inline_internal vec3
&operator/=(vec3 &Left, vec3 Right)
{
    
    return (Left = Left / Right);
}

inline_internal vec4
&operator/=(vec4 &Left, vec4 Right)
{
    
    return (Left = Left / Right);
}

inline_internal vec2
&operator/=(vec2 &Left, r32 Right)
{
    
    return (Left = Left / Right);
}

inline_internal vec3
&operator/=(vec3 &Left, r32 Right)
{
    
    return (Left = Left / Right);
}

inline_internal vec4
&operator/=(vec4 &Left, r32 Right)
{
    
    return (Left = Left / Right);
}

inline_internal mat4
&operator/=(mat4 &Left, r32 Right)
{
    
    return (Left = Left / Right);
}

inline_internal quaternion
&operator/=(quaternion &Left, r32 Right)
{
    
    return (Left = Left / Right);
}

inline_internal b32
operator==(vec2 Left, vec2 Right)
{
    
    return EqualsVec2(Left, Right);
}

inline_internal b32
operator==(vec3 Left, vec3 Right)
{
    
    return EqualsVec3(Left, Right);
}

inline_internal b32
operator==(vec4 Left, vec4 Right)
{
    
    return EqualsVec4(Left, Right);
}

inline_internal b32
operator!=(vec2 Left, vec2 Right)
{
    
    return !EqualsVec2(Left, Right);
}

inline_internal b32
operator!=(vec3 Left, vec3 Right)
{
    
    return !EqualsVec3(Left, Right);
}

inline_internal b32
operator!=(vec4 Left, vec4 Right)
{
    
    return !EqualsVec4(Left, Right);
}

inline_internal vec2
operator-(vec2 In)
{
    
    vec2 Result;
    Result.X = -In.X;
    Result.Y = -In.Y;
    return(Result);
}

inline_internal vec3
operator-(vec3 In)
{
    
    vec3 Result;
    Result.X = -In.X;
    Result.Y = -In.Y;
    Result.Z = -In.Z;
    return(Result);
}

inline_internal vec4
operator-(vec4 In)
{
    
    vec4 Result;
#if MATH__USE_SSE
    Result.InternalElementsSSE = _mm_xor_ps(In.InternalElementsSSE, _mm_set1_ps(-0.0f));
#else
    Result.X = -In.X;
    Result.Y = -In.Y;
    Result.Z = -In.Z;
    Result.W = -In.W;
#endif
    return(Result);
}

#endif /* LANG_CPP */

#endif /* BASE_MATH_H */