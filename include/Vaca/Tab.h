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

#ifndef VACA_TAB_H
#define VACA_TAB_H

#include "Vaca/Widget.h"
#include "Vaca/Font.h"
#include "Vaca/WidgetClass.h"
#include "Vaca/Register.h"
#include "Vaca/Panel.h"

namespace Vaca {

#define TabBaseStyle		ChildStyle
#define TabStyle		TabBaseStyle
#define TabPageStyle		PanelStyle

//////////////////////////////////////////////////////////////////////
// TabBase

/**
 * Widget that contains tabs to switch between them. This class
 * doesn't controls pages automatically, it's useful when you want a
 * special behaviour for the tabs (see Tabular example). If you want
 * to put widgets in the pages, and swap them when the user change the
 * pages, you should use the Tab widget.
 *
 * @see Tab
 */
class VACA_DLL TabBase : public Widget
{
  Font *mUserFont;
  Font mTabFont;

public:

  TabBase(Widget *parent, Style style = TabBaseStyle);

  virtual Font &getFont();
  virtual void setFont(Font &font);

  virtual Rect getClientBounds();

  Side getSide();
  void setSide(Side side);

  bool isMultiline();
  void setMultiline(bool state);

  int addPage(const String &text, int pageIndex = -1);
  void removePage(int pageIndex);

  int getPageCount();
  int getRowCount();

  int getActivePage();
  void setActivePage(int pageIndex);

  String getPageText(int pageIndex);

//   void setPadding(Size padding);

  Size getNonClientSize();

  virtual Size preferredSize();
  virtual Size preferredSize(const Size &fitIn);

  // signals
//   boost::signal<void (WidgetEvent &ev)> PageChanging;
  boost::signal<void (WidgetEvent &ev)> PageChange; ///< @see onPageChange

protected:
  // reflection
  virtual bool onNotify(LPNMHDR lpnmhdr, LRESULT &lResult);

  // new events
//   void onPageChanging(WidgetEvent &ev);
  void onPageChange(WidgetEvent &ev);

private:

  void updateFont();

};

//////////////////////////////////////////////////////////////////////
// Tab

/**
 * Automatic controls for page in a tab. This class automatically
 * controls the visibility of each page (AutoPage). 
 *
 * TODO This is class isn't ready yet.
 */
class VACA_DLL Tab : public TabBase
{
public:

  Tab(Widget *parent, Style style = TabStyle);

};

//////////////////////////////////////////////////////////////////////
// TabPage

class TabPageClass : public WidgetClass
{
public:
  static LPCTSTR getClassName() { return _T("Vaca.TabPage"); }
};

/**
 * A page for an automatic Tab. It's like a Panel.
 */
class VACA_DLL TabPage : public Register<TabPageClass>, public Panel
{
public:
  TabPage(const String &text, Tab *parent, Style style = TabPageStyle);
};

} // namespace Vaca

#endif