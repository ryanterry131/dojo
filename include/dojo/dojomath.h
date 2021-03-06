/*
 *  dojomath.h
 *  NinjaTraining
 *
 *  Created by Tommaso Checchi on 5/15/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "dojo_common_header.h"

#include "Vector.h"
#include "Radians.h"

namespace Dojo {
	class Math {
	public:
		static const Radians HALF_PI;
		static const Radians PI; ///well, pi
		static const Radians TAU; ///2*pi

		template<class T, std::size_t N>
		static constexpr size_t array_len(const T(& array)[N]) {
			return N;
		}

		template<class T, std::size_t N>
		static constexpr size_t array_size(const T(& array)[N]) {
			return N * sizeof(T);
		}

		///clamps n between max and min
		template <typename T>
		static T clamp(T n, T min, T max) {
			DEBUG_ASSERT( max >= min, "clamp: max must be >= than min" );

			return std::min(max, std::max(min, n));
		}

		///gets the nth bit in the mask i
		static bool getBit(int i, uint8_t n) {
			DEBUG_ASSERT( n < 32, "getBit: uint8_t number was more than 32" );

			return (i >> n) & 0x1;
		}

		///sets the nth bit of the mask i
		static int setBit(int i, int n, bool state) {
			DEBUG_ASSERT( n < 32, "setBit: uint8_t number was more than 32" );

			if (state) {
				return i | (0x1 << n);
			}
			else {
				return i & (~(0x1 << n));
			}
		}

		///clamps in [0..1[
		static float saturate(float n) {
			return clamp(n, 1.f, 0.f);
		}

		///returns the sign of val
		static float sign(float val) {
			if (val < 0) {
				return -1.f;
			}
			else if (val > 0) {
				return 1.f;
			}
			else {
				return 0;
			}
		}

		///converts float to int by flooring
		static int floorInt(float f) {
			return (int)std::floor(f);
		}

		static float lerp(float to, float from, float s) {
			return to * s + from * (1.f - s);
		}

		static Vector lerp(const Vector& to, const Vector& from, float s) {
			return Vector(
							lerp(to.x, from.x, s),
							lerp(to.y, from.y, s),
							lerp(to.z, from.z, s));
		}

		///returns if a is inside ]b-bias, b+bias[
		static bool isNear(float a, float b, float bias) {
			return a < b + bias and a > b - bias;
		}

		static float abs(float a) {
			return (a >= 0) ? a : -a;
		}

		static bool AABBContains(const Vector& max, const Vector& min, const Vector& point) {
			return max.x >= point.x and max.y >= point.y and max.z >= point.z and min.x <= point.x and min.y <= point.y and min.z <= point.z;
		}

		static bool AABBContains2D(const Vector& max, const Vector& min, const Vector& point) {
			return max.x >= point.x and max.y >= point.y and min.x <= point.x and min.y <= point.y;
		}

		static bool AABBContainsAABB(const Vector& maxA, const Vector& minA, const Vector& maxB, const Vector& minB) {
			return AABBContains(maxA, minA, maxB) and AABBContains(maxA, minA, minB);
		}

		static bool segmentsOverlap(float x2, float x1, float y2, float y1) {
			return x2 >= y1 and y2 >= x1;
		}

		static bool AABBsCollide(const Vector& maxA, const Vector& minA, const Vector& maxB, const Vector& minB) {
			return
				segmentsOverlap(maxA.x, minA.x, maxB.x, minB.x) and
				segmentsOverlap(maxA.y, minA.y, maxB.y, minB.y) and
				segmentsOverlap(maxA.z, minA.z, maxB.z, minB.z);
		}

		static bool AABBsCollide2D(const Vector& maxA, const Vector& minA, const Vector& maxB, const Vector& minB) {
			return
				segmentsOverlap(maxA.x, minA.x, maxB.x, minB.x) and
				segmentsOverlap(maxA.y, minA.y, maxB.y, minB.y);
		}

		///does a bitwise shift-with-rotation left on the uint8_t n
		static unsigned char rotateLeft(unsigned char n, unsigned char i) {
			return (n << i) | (n >> (8 - i));
		}

		///does a bitwise shift-with-rotation right on the uint8_t n
		static unsigned char rotateRight(unsigned char n, unsigned char i) {
			return (n >> i) | (n << (8 - i));
		}

		static Radians atan2(float y, float x) {
			return Radians(::atan2f(y, x));
		}

		///Simulate an harmonic oscillator
		/**
		x     - value             (input/output)
		v     - velocity          (input/output)
		xt    - target value      (input)
		zeta  - damping ratio     (input)
		omega - angular frequency (input)
		h     - time step         (input)
		*/
		template<class T>
		static void simulateSpring(
			T& x, T& v, T xt,
			float zeta, float omega, float dt
		) {
			const auto f = 1.0f + 2.0f * dt * zeta * omega;
			const auto oo = omega * omega;
			const auto hoo = dt * oo;
			const auto hhoo = dt * hoo;
			const auto detInv = 1.0f / (f + hhoo);
			const auto detX = f * x + dt * v + hhoo * xt;
			const auto detV = v + hoo * (xt - x);
			x = detX * detInv;
			v = detV * detInv;
		}

		template<typename T>
		static T packNormalized(float x, T max = std::numeric_limits<T>::max()) {
			return (T)std::ceil(x * max);
		}

		static std::pair<bool, Vector> segmentsIntersect(const Vector& A1, const Vector& A2, const Vector& B1, const Vector& B2);
		
		static std::pair<std::vector<Vector>, std::vector<Vector>> cutConvexShape(std::vector<Vector>& shape, const Vector& cutSegmentStart, const Vector& cutSegmentEnd);
	
		static Vector findCentroid(const std::vector<Vector>& contour);
	};
}
