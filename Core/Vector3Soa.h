//----------------------------------------------------------------------------------------------------------------------
// Vector3Soa.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_VECTOR3_SOA_H
#define LUNAR_CORE_VECTOR3_SOA_H

#include "Core/Vector3.h"

namespace Lunar
{
    /// SIMD-optimized structure-of-arrays 3-component vector.
    L_SIMD_ALIGN_PRE class LUNAR_CORE_API Vector3Soa
    {
    public:
        /// X components.
        SimdVector m_x;
        /// Y components.
        SimdVector m_y;
        /// Z components.
        SimdVector m_z;

        /// @name Construction/Destruction
        //@{
        inline Vector3Soa();
        inline Vector3Soa( const SimdVector& rX, const SimdVector& rY, const SimdVector& rZ );
        inline Vector3Soa( const float32_t* pX, const float32_t* pY, const float32_t* pZ );
        inline explicit Vector3Soa( const Vector3& rVector );
        //@}

        /// @name Loading and Storage
        //@{
        inline void Load( const float32_t* pX, const float32_t* pY, const float32_t* pZ );
        inline void Load4Splat( const float32_t* pX, const float32_t* pY, const float32_t* pZ );
        inline void Load1Splat( const float32_t* pX, const float32_t* pY, const float32_t* pZ );

        inline void Store( float32_t* pX, float32_t* pY, float32_t* pZ ) const;
        inline void Store4( float32_t* pX, float32_t* pY, float32_t* pZ ) const;
        inline void Store1( float32_t* pX, float32_t* pY, float32_t* pZ ) const;

        inline void Splat( const Vector3& rVector );
        //@}

        /// @name Math
        //@{
        inline Vector3Soa Add( const Vector3Soa& rVector ) const;
        inline Vector3Soa Subtract( const Vector3Soa& rVector ) const;
        inline Vector3Soa Multiply( const Vector3Soa& rVector ) const;
        inline Vector3Soa Divide( const Vector3Soa& rVector ) const;

        inline void AddSet( const Vector3Soa& rVector0, const Vector3Soa& rVector1 );
        inline void SubtractSet( const Vector3Soa& rVector0, const Vector3Soa& rVector1 );
        inline void MultiplySet( const Vector3Soa& rVector0, const Vector3Soa& rVector1 );
        inline void DivideSet( const Vector3Soa& rVector0, const Vector3Soa& rVector1 );

        inline void MultiplyAddSet(
            const Vector3Soa& rVectorMul0, const Vector3Soa& rVectorMul1, const Vector3Soa& rVectorAdd );

        inline Vector3Soa GetScaled( const SimdVector& rScale ) const;
        inline void Scale( const SimdVector& rScale );

        inline SimdVector Dot( const Vector3Soa& rVector ) const;

        inline Vector3Soa Cross( const Vector3Soa& rVector ) const;
        inline void CrossSet( const Vector3Soa& rVector0, const Vector3Soa& rVector1 );

        inline SimdVector GetMagnitude() const;
        inline SimdVector GetMagnitudeSquared() const;

        inline Vector3Soa GetNormalized( const SimdVector& rEpsilon = Simd::EPSILON ) const;
        inline void Normalize( const SimdVector& rEpsilon = Simd::EPSILON );

        inline Vector3Soa GetNegated() const;
        inline void GetNegated( Vector3Soa& rResult ) const;
        inline void Negate();
        //@}

        /// @name Comparison
        //@{
        inline SimdMask Equals( const Vector3Soa& rVector, const SimdVector& rEpsilon = Simd::EPSILON ) const;
        inline SimdMask NotEquals( const Vector3Soa& rVector, const SimdVector& rEpsilon = Simd::EPSILON ) const;
        //@}

        /// @name Overloaded Operators
        //@{
        inline Vector3Soa operator-() const;

        inline Vector3Soa operator+( const Vector3Soa& rVector ) const;
        inline Vector3Soa operator-( const Vector3Soa& rVector ) const;
        inline Vector3Soa operator*( const Vector3Soa& rVector ) const;
        inline Vector3Soa operator/( const Vector3Soa& rVector ) const;

        inline Vector3Soa& operator+=( const Vector3Soa& rVector );
        inline Vector3Soa& operator-=( const Vector3Soa& rVector );
        inline Vector3Soa& operator*=( const Vector3Soa& rVector );
        inline Vector3Soa& operator/=( const Vector3Soa& rVector );

        inline Vector3Soa operator*( const SimdVector& rScale ) const;
        inline Vector3Soa& operator*=( const SimdVector& rScale );

        inline SimdMask operator==( const Vector3Soa& rVector ) const;
        inline SimdMask operator!=( const Vector3Soa& rVector ) const;
        //@}

        /// @name Friend Functions
        //@{
        inline friend Vector3Soa operator*( const SimdVector& rScale, const Vector3Soa& rVector );
        //@}
    } L_SIMD_ALIGN_POST;
}

#include "Core/Vector3Soa.inl"

#if L_SIMD_LRBNI
#include "Core/Vector3SoaLrbni.inl"
#elif L_SIMD_SSE
#include "Core/Vector3SoaSse.inl"
#endif

#endif  // LUNAR_CORE_VECTOR3_SOA_H