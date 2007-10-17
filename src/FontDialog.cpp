// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, David A. Capello
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
#include "Vaca/FontDialog.h"
#include "Vaca/Font.h"
#include "Vaca/Widget.h"
#include "Vaca/Debug.h"
#include "Vaca/Application.h"
// #include "Vaca/Bind.h"

using namespace Vaca;

FontDialog::FontDialog(Font &font, Widget *parent)
  : CommonDialog(parent)
  , mFont(font)
{
}

FontDialog::~FontDialog()
{
}

bool FontDialog::doModal()
{
  CHOOSEFONT cf;

  cf.lStructSize = sizeof(CHOOSEFONT);
  cf.hwndOwner = getParentHWND();
  cf.hDC = NULL;
  cf.lpLogFont = &mLogFont;
  cf.iPointSize = 0; 
  cf.Flags = CF_SCREENFONTS | CF_LIMITSIZE;
  cf.rgbColors = RGB(0,0,0); 
  cf.lCustData = 0L; 
  cf.lpfnHook = (LPCFHOOKPROC)NULL; 
  cf.lpTemplateName = (LPTSTR)NULL; 
  cf.hInstance = (HINSTANCE)Application::getHINSTANCE();
  cf.lpszStyle = (LPTSTR)NULL; 
  cf.nFontType = SCREEN_FONTTYPE; 
  cf.nSizeMin = 4; 
  cf.nSizeMax = 72; 

  if (mFont.getLogFont(&mLogFont))
    cf.Flags |= CF_INITTOLOGFONTSTRUCT;

  if (ChooseFont(&cf)) {
    mFont.assign(&mLogFont);
    return true;
  }
  else
    return false;
}

bool FontDialog::getLogFont(LPLOGFONT lplf) const
{
  CopyMemory(lplf, &mLogFont, sizeof(LOGFONT));
  return true;
}
