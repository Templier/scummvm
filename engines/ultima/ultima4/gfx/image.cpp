/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "ultima/ultima4/gfx/image.h"
#include "ultima/ultima4/gfx/screen.h"
#include "ultima/ultima4/core/settings.h"
#include "ultima/ultima4/core/utils.h"
#include "ultima/shared/std/containers.h"
#include "common/system.h"
#include "common/list.h"
#include "common/textconsole.h"
#include "graphics/screen.h"

namespace Ultima {
namespace Ultima4 {

Image::Image() : _surface(nullptr), _disposeAfterUse(DisposeAfterUse::NO),
		_paletted(false) {
}

Image *Image::create(int w, int h) {
	Image *im = new Image();
	im->createInternal(w, h, Graphics::PixelFormat::createFormatCLUT8());

	return im;
}

Image *Image::create(int w, int h, const Graphics::PixelFormat &format) {
	Image *im = new Image();
	im->createInternal(w, h, format);

	return im;
}

void Image::createInternal(int w, int h, const Graphics::PixelFormat &format) {
	_paletted = format.isCLUT8();
	_surface = new Graphics::ManagedSurface(w, h, format);
	_disposeAfterUse = DisposeAfterUse::YES;
}

void Image::blitFrom(const Graphics::Surface &src) {
	_surface->blitFrom(src);
}

Image *Image::createScreenImage() {
	Image *screen = new Image();
	screen->_surface = g_screen;
	screen->_disposeAfterUse = DisposeAfterUse::NO;
	screen->_paletted = false;

	return screen;
}

Image *Image::duplicate(Image *image, const Graphics::PixelFormat &format) {
	bool alphaOn = image->isAlphaOn();
	Image *im = create(image->width(), image->height(), format);

	if (im->isIndexed())
		im->setPaletteFromImage(image);

	/* Turn alpha off before blitting to non-screen surfaces */
	if (alphaOn)
		image->alphaOff();

	image->drawOn(im, 0, 0);

	if (alphaOn)
		image->alphaOn();

	im->_backgroundColor = image->_backgroundColor;

	return im;
}

Image::~Image() {
	if (_disposeAfterUse == DisposeAfterUse::YES)
		delete _surface;
}

void Image::setPalette(const byte *colors, unsigned n_colors) {
	assertMsg(_paletted, "imageSetPalette called on non-paletted image");
	_surface->setPalette(colors, 0, n_colors);
}

void Image::setPaletteFromImage(const Image *src) {
	assertMsg(_paletted && src->_paletted, "imageSetPaletteFromImage called on non-indexed image");

	uint8 srcPal[Graphics::PALETTE_COUNT * 3];
	src->_surface->grabPalette(srcPal, 0, Graphics::PALETTE_COUNT);
	_surface->setPalette(srcPal, 0, Graphics::PALETTE_COUNT);
}

RGBA Image::getPaletteColor(int index) {
	RGBA color = RGBA(0, 0, 0, 0);

	if (_paletted) {
		uint8 pal[1 * 3];
		_surface->grabPalette(pal, index, 1);
		color.r = pal[0];
		color.g = pal[1];
		color.b = pal[2];
		color.a = IM_OPAQUE;
	}

	return color;
}

RGBA Image::setColor(uint8 r, uint8 g, uint8 b, uint8 a) {
	RGBA color = RGBA(r, g, b, a);
	return color;
}

bool Image::setFontColor(ColorFG fg, ColorBG bg) {
	if (!setFontColorFG(fg)) return false;
	if (!setFontColorBG(bg)) return false;
	return true;
}

bool Image::setFontColorFG(ColorFG fg) {
	switch (fg) {
	case FG_GREY:
		if (!setPaletteIndex(TEXT_FG_PRIMARY_INDEX,   153, 153, 153)) return false;
		if (!setPaletteIndex(TEXT_FG_SECONDARY_INDEX, 102, 102, 102)) return false;
		if (!setPaletteIndex(TEXT_FG_SHADOW_INDEX,    51, 51, 51)) return false;
		break;
	case FG_BLUE:
		if (!setPaletteIndex(TEXT_FG_PRIMARY_INDEX,   102, 102, 255)) return false;
		if (!setPaletteIndex(TEXT_FG_SECONDARY_INDEX, 51, 51, 204)) return false;
		if (!setPaletteIndex(TEXT_FG_SHADOW_INDEX,    51, 51, 51)) return false;
		break;
	case FG_PURPLE:
		if (!setPaletteIndex(TEXT_FG_PRIMARY_INDEX,   255, 102, 255)) return false;
		if (!setPaletteIndex(TEXT_FG_SECONDARY_INDEX, 204, 51, 204)) return false;
		if (!setPaletteIndex(TEXT_FG_SHADOW_INDEX,    51, 51, 51)) return false;
		break;
	case FG_GREEN:
		if (!setPaletteIndex(TEXT_FG_PRIMARY_INDEX,   102, 255, 102)) return false;
		if (!setPaletteIndex(TEXT_FG_SECONDARY_INDEX, 0, 153, 0)) return false;
		if (!setPaletteIndex(TEXT_FG_SHADOW_INDEX,    51, 51, 51)) return false;
		break;
	case FG_RED:
		if (!setPaletteIndex(TEXT_FG_PRIMARY_INDEX,   255, 102, 102)) return false;
		if (!setPaletteIndex(TEXT_FG_SECONDARY_INDEX, 204, 51, 51)) return false;
		if (!setPaletteIndex(TEXT_FG_SHADOW_INDEX,    51, 51, 51)) return false;
		break;
	case FG_YELLOW:
		if (!setPaletteIndex(TEXT_FG_PRIMARY_INDEX,   255, 255, 51)) return false;
		if (!setPaletteIndex(TEXT_FG_SECONDARY_INDEX, 204, 153, 51)) return false;
		if (!setPaletteIndex(TEXT_FG_SHADOW_INDEX,    51, 51, 51)) return false;
		break;
	default:
		if (!setPaletteIndex(TEXT_FG_PRIMARY_INDEX,   255, 255, 255)) return false;
		if (!setPaletteIndex(TEXT_FG_SECONDARY_INDEX, 204, 204, 204)) return false;
		if (!setPaletteIndex(TEXT_FG_SHADOW_INDEX,    68, 68, 68)) return false;
	}
	return true;
}

bool Image::setFontColorBG(ColorBG bg) {
	switch (bg) {
	case BG_BRIGHT:
		if (!setPaletteIndex(TEXT_BG_INDEX, 0, 0, 102))
			return false;
		break;
	default:
		if (!setPaletteIndex(TEXT_BG_INDEX, 0, 0, 0))
			return false;
	}
	return true;
}

bool Image::setPaletteIndex(uint index, uint8 r, uint8 g, uint8 b) {
	if (!_paletted)
		return false;

	const uint8 palette[1 * 3] = {
		r, g, b
	};
	_surface->setPalette(palette, index, 1);

	// success
	return true;
}

bool Image::getTransparentIndex(uint &index) const {
	if (!_paletted || !_surface->hasTransparentColor())
		return false;

	index = _surface->getTransparentColor();
	return true;
}

void Image::initializeToBackgroundColor(RGBA backgroundColor) {
	if (_paletted)
		error("initializeToBackgroundColor: Not supported");

	_backgroundColor = backgroundColor;
	fillRect(0, 0, _surface->w, _surface->h, backgroundColor.r,
		backgroundColor.g, backgroundColor.b, backgroundColor.a);
}

bool Image::isAlphaOn() const {
	return !_paletted;
}

void Image::alphaOn() {
}

void Image::alphaOff() {
}

void Image::putPixel(int x, int y, int r, int g, int b, int a) {
	uint32 color = getColor(r, g, b, a);
	_surface->setPixel(x, y, color);
}

uint Image::getColor(byte r, byte g, byte b, byte a) {
	uint color;

	if (_surface->format.bytesPerPixel == 1) {
		uint8 pal[256 * 3];
		_surface->grabPalette(pal, 0, 256);
		for (color = 0; color <= 0xfe; ++color) {
			byte rv = pal[(color * 3) + 0];
			byte gv = pal[(color * 3) + 1] & 0xff;
			byte bv = pal[(color * 3) + 2] & 0xff;
			if (r == rv && g == gv && b == bv)
				break;
		}

		return color;
	} else {
		return _surface->format.ARGBToColor(a, r, g, b);
	}
}

void Image::makeBackgroundColorTransparent(int haloSize, int shadowOpacity) {
	uint32 bgColor = _surface->format.ARGBToColor(
	                     static_cast<byte>(_backgroundColor.a),
	                     static_cast<byte>(_backgroundColor.r),
	                     static_cast<byte>(_backgroundColor.g),
	                     static_cast<byte>(_backgroundColor.b)
	                 );

	performTransparencyHack(bgColor, 1, 0, haloSize, shadowOpacity);
}

void Image::performTransparencyHack(uint colorValue, uint numFrames,
									uint currentFrameIndex, uint haloWidth,
									uint haloOpacityIncrementByPixelDistance) {
	Common::List<Common::Pair<uint, uint> > opaqueXYs;
	int x, y;
	byte t_r, t_g, t_b;

	_surface->format.colorToRGB(colorValue, t_r, t_g, t_b);

	int frameHeight = _surface->h / numFrames;
	//Min'd so that they never go out of range (>=h)
	int top = MIN(_surface->h, (int16)(currentFrameIndex * frameHeight));
	int bottom = MIN(_surface->h, (int16)(top + frameHeight));

	for (y = top; y < bottom; y++) {

		for (x = 0; x < _surface->w; x++) {
			uint r, g, b, a;
			getPixel(x, y, r, g, b, a);
			if (r == t_r &&
			        g == t_g &&
			        b == t_b) {
				putPixel(x, y, r, g, b, IM_TRANSPARENT);
			} else {
				putPixel(x, y, r, g, b, a);
				if (haloWidth)
					opaqueXYs.push_back(Common::Pair<uint, uint>(x, y));
			}
		}
	}
	int ox, oy;
	for (const auto &xy : opaqueXYs) {
		ox = xy.first;
		oy = xy.second;
		int span = int(haloWidth);
		int x_start = MAX(0, ox - span);
		int x_finish = MIN(int(_surface->w), ox + span + 1);
		for (x = x_start; x < x_finish; ++x) {
			int y_start = MAX(int(top), oy - span);
			int y_finish = MIN(int(bottom), oy + span + 1);
			for (y = y_start; y < y_finish; ++y) {

				int divisor = 1 + span * 2 - abs(int(ox - x)) - abs(int(oy - y));

				uint r, g, b, a;
				getPixel(x, y, r, g, b, a);
				if (a != IM_OPAQUE) {
					putPixel(x, y, r, g, b, MIN(IM_OPAQUE, a + haloOpacityIncrementByPixelDistance / divisor));
				}
			}
		}
	}


}

void Image::setTransparentIndex(uint index) {
	if (_paletted)
		_surface->setTransparentColor(index);
}

void Image::putPixelIndex(int x, int y, uint index) {
	_surface->setPixel(x, y, index);
}

void Image::fillRect(int x, int y, int w, int h, int r, int g, int b, int a) {
	uint32 color = getColor(r, g, b, a);
	_surface->fillRect(Common::Rect(x, y, x + w, y + h), color);
}

void Image::getPixel(int x, int y, uint &r, uint &g, uint &b, uint &a) const {
	uint index;
	byte r1, g1, b1, a1;

	index = _surface->getPixel(x, y);

	if (_surface->format.bytesPerPixel == 1) {
		uint8 pal[1 * 3];
		_surface->grabPalette(pal, index, 1);
		r = pal[0];
		g = pal[1];
		b = pal[2];
		a = IM_OPAQUE;
	} else {
		_surface->format.colorToARGB(index, a1, r1, g1, b1);
		r = r1;
		g = g1;
		b = b1;
		a = a1;
	}
}

void Image::getPixelIndex(int x, int y, uint &index) const {
	index = _surface->getPixel(x, y);
}

Graphics::ManagedSurface *Image::getSurface(Image *d) const {
	if (d)
		return d->_surface;

	return g_screen;
}

void Image::drawOn(Image *d, int x, int y) const {
	Graphics::ManagedSurface *destSurface = getSurface(d);
	destSurface->blitFrom(*_surface, Common::Point(x, y));
}

void Image::drawSubRectOn(Image *d, int x, int y, int rx, int ry, int rw, int rh) const {
	Graphics::ManagedSurface *destSurface = getSurface(d);

	Common::Rect srcRect(rx, ry, MIN(rx + rw, (int)_surface->w), MIN(ry + rh, (int)_surface->h));
	Common::Point destPos(x, y);

	// Handle when the source rect is off the surface
	if (srcRect.left < 0) {
		destPos.x += -srcRect.left;
		srcRect.left = 0;
	}

	if (srcRect.top < 0) {
		destPos.y += -srcRect.top;
		srcRect.top = 0;
	}

	if (srcRect.isValidRect())
		// Blit the surface
		destSurface->blitFrom(*_surface, srcRect, destPos);
}

void Image::drawSubRectInvertedOn(Image *d, int x, int y, int rx, int ry, int rw, int rh) const {
	Graphics::ManagedSurface *destSurface = getSurface(d);
	int i;
	Common::Rect src;
	Common::Point destPos;

	for (i = 0; i < rh; i++) {
		src.left = rx;
		src.top = ry + i;
		src.right = rx + rw;
		src.bottom = ry + i + 1;

		destPos.x = x;
		destPos.y = y + rh - i - 1;

		destSurface->blitFrom(*_surface, src, destPos);
	}
}

void Image::dump() {
	g_screen->blitFrom(*_surface, Common::Point(0, 0));
	g_screen->update();
}

void Image::drawHighlighted() {
	RGBA c;
	for (int i = 0; i < _surface->h; i++) {
		for (int j = 0; j < _surface->w; j++) {
			getPixel(j, i, c.r, c.g, c.b, c.a);
			putPixel(j, i, 0xff - c.r, 0xff - c.g, 0xff - c.b, c.a);
		}
	}
}

} // End of namespace Ultima4
} // End of namespace Ultima
