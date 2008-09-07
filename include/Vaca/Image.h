// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the Vaca nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef VACA_IMAGE_H
#define VACA_IMAGE_H

#include "Vaca/base.h"
#include "Vaca/ResourceId.h"
#include "Vaca/Size.h"

namespace Vaca {

class Graphics;

/**
 * An image (HBITMAP wrapper)
 */
class VACA_DLL Image
{
  HDC m_fromHDC;
  Size m_size;
  HBITMAP m_HBITMAP;
  Graphics* m_graphics;

public:

  Image();
  Image(ResourceId imageId);
  Image(const String& fileName);
  Image(const Size& sz);
  Image(const Size& sz, int bpp);
  Image(const Size& sz, Graphics& g);
  Image(const Image& image);
  virtual ~Image();

  bool isValid();

  int getWidth() const;
  int getHeight() const;
  Size getSize() const;

  Graphics* getGraphics();

  HBITMAP getHBITMAP();

  Image& operator=(const Image& image);

private:

  void destroy();
  void copyTo(Image& image) const;
  
};

} // namespace Vaca

#endif // VACA_IMAGE_H