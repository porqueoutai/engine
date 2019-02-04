// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================

#ifndef TB_COLOR_H
#define TB_COLOR_H

#include "tb_types.h"

namespace tb {

/** TBColor contains a 32bit color. */

class TBColor
{
public:
	constexpr TBColor() : b(0), g(0), r(0), a(255) {}
	constexpr TBColor(int r, int g, int b, int a = 255) : b(b), g(g), r(r), a(a) {}

	uint8_t b, g, r, a;

	void Set(const TBColor &color) { *this = color; }

	/** Set the color from string in any of the following formats:
		"#rrggbbaa", "#rrggbb", "#rgba", "#rgb" */
	void SetFromString(const char *str, int len);

	inline operator uint32_t () const		{ return *((uint32_t*)this); }
	inline bool operator == (const TBColor &c) const { return *this == (uint32_t)c; }
	inline bool operator != (const TBColor &c) const { return !(*this == c); }

	/** Premultiply alpha on the r, g, b components */
	inline void Premultiply() {
		const uint32_t a32 = a;
		r = (r * a32 + 1) >> 8;
		g = (g * a32 + 1) >> 8;
		b = (b * a32 + 1) >> 8;
	}

	/** Unpremultiply alpha on the r, g, b components */
	inline void Unpremultiply() {
		const uint32_t a32 = a;
		if (a32) {
			r = r * 255 / a32;
			g = g * 255 / a32;
			b = b * 255 / a32;
		}
	}
};

} // namespace tb

#endif // TB_COLOR_H