/*
 *  Color.h
 *  NinjaTraining
 *
 *  Created by Tommaso Checchi on 4/25/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "dojo_common_header.h"

#include "Vector.h"

namespace Dojo {
	class Color {
	public:

		typedef unsigned int RGBAPixel;

		static const Color Red, Green, Blue, Black, White, Gray, Yellow, Cyan, Purple, None;

		static Color fromARGB(uint32_t pixel) {
			byte* ch = (byte*)&pixel;
			return{
				ch[2] / 255.f,
				ch[1] / 255.f,
				ch[0] / 255.f,
				ch[3] / 255.f
			};
		}

		float r, g, b, a;

		Color() :
			r(0),
			g(0),
			b(0),
			a(0) {

		}

		Color(float r, float g, float b, float a = 1.f) {
			this->r = r;
			this->g = g;
			this->b = b;
			this->a = a;
		}

		explicit Color(const Vector& v, float a = 1.f) {
			this->r = v.x;
			this->g = v.y;
			this->b = v.z;
			this->a = a;
		}

		///creates from a single A8R8G8B8 32-bit value

		void set(byte r, byte g, byte b, byte a = 255) {
			this->r = (float)r / 255.f;
			this->g = (float)g / 255.f;
			this->b = (float)b / 255.f;
			this->a = (float)a / 255.f;
		}

		///creates a single 32-bit hex value representing the color (will degrade precision in HDR colors)
		RGBAPixel toRGBA() const {
			RGBAPixel p;
			byte* ch = (byte*)&p;

			ch[0] = (byte)(r * 255);
			ch[1] = (byte)(g * 255);
			ch[2] = (byte)(b * 255);
			ch[3] = (byte)(a * 255);

			return p;
		}

		///scales each color's component except alpha
		Color operator *(float s) const {
			return Color(r * s, g * s, b * s, a);
		}

		///multiplies two colors component-wise
		Color operator *(const Color& c) const {
			return Color(r * c.r, g * c.g, b * c.b, a * c.a);
		}

		///sums two colors component-wise
		Color operator +(float s) const {
			return Color(r + s, g + s, b + s, a + s);
		}

		Color operator +(const Color& c) const {
			return{c.r + r, c.g + g, c.b + b, c.a + a};
		}

		///linearly interpolates two colors
		Color lerp(float s, const Color& c) const {
			float invs = 1.f - s;
			return Color(
						r * invs + c.r * s,
						g * invs + c.g * s,
						b * invs + c.b * s,
						a * invs + c.a * s);
		}

		void operator *=(float s) {
			r *= s;
			g *= s;
			b *= s;
		}

		void operator +=(const Color& c) {
			r += c.r;
			g += c.g;
			b += c.b;
		}

	protected:
	};
}
