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
#include "Vaca/KeyEvent.h"
#include "Vaca/Widget.h"

using namespace Vaca;

KeyEvent::KeyEvent(Widget *source, Keys::Type keys)
  : Event(source)
  , mConsumed(false)
  , mKeys(keys)
{
}

KeyEvent::~KeyEvent()
{
}

void KeyEvent::consume()
{
  mConsumed = true;
}

bool KeyEvent::isConsumed() const
{
  return mConsumed;
}

Keys::Type KeyEvent::getKeyCode() const
{
  return mKeys & Keys::KeyCode;
}

Keys::Type KeyEvent::getModifiers() const
{
  return mKeys & Keys::Modifiers;
}

bool KeyEvent::isShift() const
{
  return (mKeys & Keys::Shift  ) != 0;
}

bool KeyEvent::isControl() const
{
  return (mKeys & Keys::Control) != 0;
}

bool KeyEvent::isAlt() const
{
  return (mKeys & Keys::Alt    ) != 0;
}
