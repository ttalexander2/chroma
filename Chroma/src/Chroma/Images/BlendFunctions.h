#pragma once
#include "Color.h"

#include <algorithm>

// Adapted from https://github.com/aseprite/aseprite/blob/main/src/doc/blend_funcs.cpp

namespace Chroma
{

	#define MASK 0xff

	#define MUL_UN8(a, b, t) \
		((t) = (a) * (uint16_t)(b) + 0x80, ((((t) >> 8) + (t) ) >> 8))

	#define DIV_UN8(a, b)							\
		(((uint16_t) (a) * MASK + ((b) / 2)) / (b))

	#define blend_multiply(b, s, t)   (MUL_UN8((b), (s), (t)))
	#define blend_screen(b, s, t)     ((b) + (s) - MUL_UN8((b), (s), (t)))
	#define blend_overlay(b, s, t)    (blend_hard_light(s, b, t))
	#define blend_darken(b, s)        (Math::min((b), (s)))
	#define blend_lighten(b, s)       (Math::max((b), (s)))
	#define blend_hard_light(b, s, t) ((s) < 128 ?                          \
									   blend_multiply((b), (s)<<1, (t)):    \
									   blend_screen((b), ((s)<<1)-255, (t)))
	#define blend_difference(b, s)    (Math::abs((b) - (s)))
	#define blend_exclusion(b, s, t)  ((t) = MUL_UN8((b), (s), (t)), ((b) + (s) - 2*(t)))


	/// @brief Function to blend divide two uint32_t colors.
	/// @param b backdrop color
	/// @param s source color
	/// @return resulting color
	inline uint32_t blend_divide(uint32_t b, uint32_t s)
	{
		if (b == 0)
			return 0;
		else if (b >= s)
			return 255;
		else
			return DIV_UN8(b, s); // return b / s
	}

	/// @brief Function to blend color dodge two uint32_t colors.
	/// @param b backdrop color
	/// @param s source color
	/// @return resulting color
	inline uint32_t blend_color_dodge(uint32_t b, uint32_t s)
	{
		if (b == 0)
			return 0;

		s = (255 - s);
		if (b >= s)
			return 255;
		else
			return DIV_UN8(b, s); // return b / (1-s)
	}

	/// @brief Function to blend color burn two uint32_t colors.
	/// @param b backdrop color
	/// @param s source color
	/// @return resulting color
	inline uint32_t blend_color_burn(uint32_t b, uint32_t s)
	{
		if (b == 255)
			return 255;

		b = (255 - b);
		if (b >= s)
			return 0;
		else
			return 255 - DIV_UN8(b, s); // return 1 - ((1-b)/s)
	}

	/// @brief Function to blend soft light two uint32_t colors.
	/// @param b backdrop color
	/// @param s source color
	/// @return resulting color
	inline uint32_t blend_soft_light(uint32_t _b, uint32_t _s)
	{
		double b = _b / 255.0;
		double s = _s / 255.0;
		double r, d;

		if (b <= 0.25)
			d = ((16 * b - 12) * b + 4) * b;
		else
			d = std::sqrt(b);

		if (s <= 0.5)
			r = b - (1.0 - 2.0 * s) * b * (1.0 - b);
		else
			r = b + (2.0 * s - 1.0) * (d - b);

		return (uint32_t)(r * 255 + 0.5);
	}

	//HSV helper functions
	
	/// @brief Calculate luminescence of a color
	/// @param r Red color channel.
	/// @param g Green color channel.
	/// @param b Blue color channel.
	/// @return Resulting luminescence.
	inline double lum(double r, double g, double b)
	{
		return 0.3 * r + 0.59 * g + 0.11 * b;
	}

	/// @brief Calculate saturation of a color
	/// @param r Red color channel.
	/// @param g Green color channel.
	/// @param b Blue color channel.
	/// @return Resulting saturation.
	inline double sat(double r, double g, double b)
	{
		return std::max(r, std::max(g, b)) - std::min(r, std::min(g, b));
	}


	/// @brief Helper function for setting the luminescence of a color.
	/// @param r Red color channel.
	/// @param g Green color channel.
	/// @param b Blue color channel.
	inline void clip_color(double& r, double& g, double& b)
	{
		double l = lum(r, g, b);
		double n = std::min(r, std::min(g, b));
		double x = std::max(r, std::max(g, b));

		if (n < 0)
		{
			r = l + (((r - l) * l) / (l - n));
			g = l + (((g - l) * l) / (l - n));
			b = l + (((b - l) * l) / (l - n));
		}

		if (x > 1)
		{
			r = l + (((r - l) * (1 - l)) / (x - l));
			g = l + (((g - l) * (1 - l)) / (x - l));
			b = l + (((b - l) * (1 - l)) / (x - l));
		}
	}

	/// @brief Sets the luminescence of a color.
	/// @param r Red color channel.
	/// @param g Green color channel.
	/// @param b Blue color channel.
	/// @param l luminescence to set.
	inline void set_lum(double& r, double& g, double& b, double l)
	{
		double d = l - lum(r, g, b);
		r += d;
		g += d;
		b += d;
		clip_color(r, g, b);
	}

	/// @brief Sets the saturation of a color.
	/// @param r Red color channel.
	/// @param g Green color channel.
	/// @param b Blue color channel.
	/// @param l luminescence to set.
	/// 
	/// The Aseprite source code indicates that this could be done better.
	inline void set_sat(double& r, double& g, double& b, double s)
	{
	#undef MIN
	#undef MAX
	#undef MID
	#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
	#define MAX(x,y)     (((x) > (y)) ? (x) : (y))
	#define MID(x,y,z)   ((x) > (y) ? ((y) > (z) ? (y) : ((x) > (z) ?    \
						   (z) : (x))) : ((y) > (z) ? ((z) > (x) ? (z) : \
						   (x)): (y)))

		double& min = MIN(r, MIN(g, b));
		double& mid = MID(r, g, b);
		double& max = MAX(r, MAX(g, b));

		if (max > min)
		{
			mid = ((mid - min) * s) / (max - min);
			max = s;
		}
		else
			mid = max = 0;

		min = 0;
	}


	/// @brief Class of functions to blend two colors together.
	/// 
	/// These are the standard <a href="https://en.wikipedia.org/wiki/Blend_modes">blend modes</a> found supported by aseprite.
	/// These blend modes are found in most graphics editing programs such as photoshop.
	class BlendFunctions
	{
	public:
		/// @brief Blend the two colors normally.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendNormal(Color* backdrop, Color* src, int opacity)
		{
			int t;
			auto sa = MUL_UN8(src->a, opacity, t);
			auto ra = backdrop->a + sa - MUL_UN8(backdrop->a, sa, t);

			backdrop->r = (unsigned char)(backdrop->r + (src->r - backdrop->r) * sa / ra);
			backdrop->g = (unsigned char)(backdrop->g + (src->g - backdrop->g) * sa / ra);
			backdrop->b = (unsigned char)(backdrop->b + (src->b - backdrop->b) * sa / ra);
			backdrop->a = (unsigned char)ra;
		}

		/// @brief Multiply the two colors.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendMultiply(Color* backdrop, Color* src, int opacity)
		{
			int t;
			src->r = blend_multiply(backdrop->r, src->r, t);
			src->g = blend_multiply(backdrop->g, src->g, t);
			src->b = blend_multiply(backdrop->b, src->b, t);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Multiply the two colors.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendScreen(Color* backdrop, Color* src, int opacity)
		{
			int t;
			src->r = blend_screen(backdrop->r, src->r, t);
			src->g = blend_screen(backdrop->g, src->g, t);
			src->b = blend_screen(backdrop->b, src->b, t);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Blend the two colors with overlay.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendOverlay(Color* backdrop, Color* src, int opacity)
		{
			int t;
			src->r = blend_overlay(backdrop->r, src->r, t);
			src->g = blend_overlay(backdrop->g, src->g, t);
			src->b = blend_overlay(backdrop->b, src->b, t);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Blend the two colors with darken.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendDarken(Color* backdrop, Color* src, int opacity)
		{
			int t;
			src->r = blend_darken(backdrop->r, src->r, t);
			src->g = blend_darken(backdrop->g, src->g, t);
			src->b = blend_darken(backdrop->b, src->b, t);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Blend the two colors with lighten.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendLighten(Color* backdrop, Color* src, int opacity)
		{
			int t;
			src->r = blend_lighten(backdrop->r, src->r, t);
			src->g = blend_lighten(backdrop->g, src->g, t);
			src->b = blend_lighten(backdrop->b, src->b, t);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Blend the two colors with color dodge.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendColorDodge(Color* backdrop, Color* src, int opacity)
		{
			src->r = blend_color_dodge(backdrop->r, src->r);
			src->g = blend_color_dodge(backdrop->g, src->g);
			src->b = blend_color_dodge(backdrop->b, src->b);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Blend the two colors with color burn.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendColorBurn(Color* backdrop, Color* src, int opacity)
		{
			int t;
			src->r = blend_color_burn(backdrop->r, src->r);
			src->g = blend_color_burn(backdrop->g, src->g);
			src->b = blend_color_burn(backdrop->b, src->b);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Blend the two colors with hard light.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendHardLight(Color* backdrop, Color* src, int opacity)
		{
			int t;
			src->r = blend_hard_light(backdrop->r, src->r, t);
			src->g = blend_hard_light(backdrop->g, src->g, t);
			src->b = blend_hard_light(backdrop->b, src->b, t);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Blend the two colors with soft light.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendSoftLight(Color* backdrop, Color* src, int opacity)
		{
			src->r = blend_soft_light(backdrop->r, src->r);
			src->g = blend_soft_light(backdrop->g, src->g);
			src->b = blend_soft_light(backdrop->b, src->b);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Blend the two colors with the difference.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendDifference(Color* backdrop, Color* src, int opacity)
		{
			src->r = blend_difference(backdrop->r, src->r);
			src->g = blend_difference(backdrop->g, src->g);
			src->b = blend_difference(backdrop->b, src->b);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Blend the two colors with exclustion.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendExclusion(Color* backdrop, Color* src, int opacity)
		{
			int t;
			src->r = blend_exclusion(backdrop->r, src->r, t);
			src->g = blend_exclusion(backdrop->g, src->g, t);
			src->b = blend_exclusion(backdrop->b, src->b, t);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Blend the two colors with hue.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendHue(Color* backdrop, Color* src, int opacity)
		{
			double r = backdrop->r / 255.0;
			double g = backdrop->g / 255.0;
			double b = backdrop->b / 255.0;
			double s = sat(r, g, b);
			double l = lum(r, g, b);

			r = src->r / 255.0;
			g = src->g / 255.0;
			b = src->b / 255.0;

			set_sat(r, g, b, s);
			set_lum(r, g, b, l);

			src->r = uint8_t(255.0 * r);
			src->g = uint8_t(255.0 * g);
			src->b = uint8_t(255.0 * b);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Blend the two colors with saturation.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendSaturation(Color* backdrop, Color* src, int opacity)
		{
			double r = src->r / 255.0;
			double g = src->g / 255.0;
			double b = src->b / 255.0;
			double s = sat(r, g, b);

			r = backdrop->r / 255.0;
			g = backdrop->g / 255.0;
			b = backdrop->b / 255.0;
			double l = lum(r, g, b);

			set_sat(r, g, b, s);
			set_lum(r, g, b, l);

			src->r = uint8_t(255.0 * r);
			src->g = uint8_t(255.0 * g);
			src->b = uint8_t(255.0 * b);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Blend the two colors with overlay.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendColor(Color* backdrop, Color* src, int opacity)
		{
			double r = backdrop->r / 255.0;
			double g = backdrop->g / 255.0;
			double b = backdrop->b / 255.0;
			double l = lum(r, g, b);

			r = src->r / 255.0;
			g = src->g / 255.0;
			b = src->b / 255.0;

			set_lum(r, g, b, l);

			src->r = uint8_t(255.0 * r);
			src->g = uint8_t(255.0 * g);
			src->b = uint8_t(255.0 * b);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Blend the two colors with luminosity.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendLuminosity(Color* backdrop, Color* src, int opacity)
		{
			double r = src->r / 255.0;
			double g = src->g / 255.0;
			double b = src->b / 255.0;
			double l = lum(r, g, b);

			r = backdrop->r / 255.0;
			g = backdrop->g / 255.0;
			b = backdrop->b / 255.0;
			
			set_lum(r, g, b, l);

			src->r = uint8_t(255.0 * r);
			src->g = uint8_t(255.0 * g);
			src->b = uint8_t(255.0 * b);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Blend the two colors with addition.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendAddition(Color* backdrop, Color* src, int opacity)
		{
			src->r = Math::min(backdrop->r + src->r, 255);
			src->g = Math::min(backdrop->g + src->g, 255);
			src->b = Math::min(backdrop->b + src->b, 255);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Blend the two colors by subtracting.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendSubtract(Color* backdrop, Color* src, int opacity)
		{
			src->r = Math::max(backdrop->r - src->r, 255);
			src->g = Math::max(backdrop->g - src->g, 255);
			src->b = Math::max(backdrop->b - src->b, 255);
			BlendNormal(backdrop, src, opacity);
		}

		/// @brief Blend the two colors by dividing.
		/// @param backdrop Backdrop color.
		/// @param src Source color.
		/// @param opacity Opacity of the source.
		static inline void BlendDivide(Color* backdrop, Color* src, int opacity)
		{
			src->r = blend_divide(backdrop->r, src->r);
			src->g = blend_divide(backdrop->g, src->g);
			src->b = blend_divide(backdrop->b, src->b);
			BlendNormal(backdrop, src, opacity);
		}

		static inline Color BlendNormal(Color backdrop, Color src, int opacity)
		{
			BlendNormal(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendMultiply(Color backdrop, Color src, int opacity)
		{
			BlendMultiply(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendScreen(Color backdrop, Color src, int opacity)
		{
			BlendScreen(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendOverlay(Color backdrop, Color src, int opacity)
		{
			BlendOverlay(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendDarken(Color backdrop, Color src, int opacity)
		{
			BlendDarken(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendLighten(Color backdrop, Color src, int opacity)
		{
			BlendLighten(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendColorDodge(Color backdrop, Color src, int opacity)
		{
			BlendColorDodge(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendColorBurn(Color backdrop, Color src, int opacity)
		{
			BlendColorBurn(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendHardLight(Color backdrop, Color src, int opacity)
		{
			BlendHardLight(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendSoftLight(Color backdrop, Color src, int opacity)
		{
			BlendSoftLight(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendDifference(Color backdrop, Color src, int opacity)
		{
			BlendDifference(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendExclusion(Color backdrop, Color src, int opacity)
		{
			BlendExclusion(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendHue(Color backdrop, Color src, int opacity)
		{
			BlendHue(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendSaturation(Color backdrop, Color src, int opacity)
		{
			BlendSaturation(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendColor(Color backdrop, Color src, int opacity)
		{
			BlendColor(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendLuminosity(Color backdrop, Color src, int opacity)
		{
			BlendLuminosity(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendAddition(Color backdrop, Color src, int opacity)
		{
			BlendAddition(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendSubtract(Color backdrop, Color src, int opacity)
		{
			BlendSubtract(&backdrop, &src, opacity);
			return backdrop;
		}

		static inline Color BlendDivide(Color backdrop, Color src, int opacity)
		{
			BlendDivide(&backdrop, &src, opacity);
			return backdrop;
		}




	};


}

