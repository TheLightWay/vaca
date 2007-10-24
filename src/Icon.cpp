// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
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

#include "stdvaca.h"
#include "Vaca/Icon.hpp"
#include "Vaca/Application.hpp"
#include "Vaca/Debug.hpp"
#include "Vaca/ResourceException.hpp"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// Icon


Icon::Icon()
{
  m_HICON = NULL;
}

/**
 * Loads an Icon from a ICON resource.
 */
Icon::Icon(int iconId, const Size &sz)
{
  m_HICON = reinterpret_cast<HICON>(LoadImage(Application::getHINSTANCE(),
					      MAKEINTRESOURCE(iconId),
					      IMAGE_ICON,
					      sz.w, sz.h, 0));

  if (m_HICON == NULL)
    throw ResourceException("Can't load the icon resource " + String::fromInt(iconId));
}

/**
 * Loads an Icon from a file.
 */
Icon::Icon(const String &fileName, const Size &sz)
{
  m_HICON = reinterpret_cast<HICON>(LoadImage(Application::getHINSTANCE(),
					      fileName.c_str(),
					      IMAGE_ICON,
					      sz.w, sz.h, LR_LOADFROMFILE));

  if (m_HICON == NULL)
    throw ResourceException("Can't load icon from file " + fileName);
}

Icon::~Icon()
{
  if (m_HICON != NULL)
    DestroyIcon(m_HICON);
}

HICON Icon::getHICON()
{
  return m_HICON;
}

void Icon::setHICON(HICON hicon)
{
  m_HICON = hicon;
}

//////////////////////////////////////////////////////////////////////
// SharedIcon


SharedIcon::SharedIcon(int iconId, const Size &sz)
{
  HICON hicon = reinterpret_cast<HICON>(LoadImage(Application::getHINSTANCE(),
						  MAKEINTRESOURCE(iconId),
						  IMAGE_ICON,
						  sz.w, sz.h, LR_SHARED));

  if (hicon == NULL)
    throw ResourceException("Can't load the icon resource " + String::fromInt(iconId));

  setHICON(hicon);
}

SharedIcon::SharedIcon(const String &fileName, const Size &sz)
{
  HICON hicon = reinterpret_cast<HICON>(LoadImage(Application::getHINSTANCE(),
						  fileName.c_str(),
						  IMAGE_ICON,
						  sz.w, sz.h,
						  LR_LOADFROMFILE | LR_SHARED));

  if (hicon == NULL)
    throw ResourceException("Can't load icon from file " + fileName);

  setHICON(hicon);
}

SharedIcon::~SharedIcon()
{
  // avoid to call DestroyIcon, this icon is shared
  setHICON(NULL);
}
