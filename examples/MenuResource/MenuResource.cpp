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

#include <Vaca/Vaca.h>
#include "resource.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////

class Console : public MultilineEdit
{
  Font m_font;
  
public:

  Console(Widget *parent)
    : MultilineEdit("", parent, MultilineEditStyle + ScrollStyle)
    , m_font("Courier New", 10)
  {
    setFont(&m_font);
    setBgColor(Color::Black);
    setFgColor(Color(0, 220, 0));
  }

  void println(const String &str)
  {
    setText(getText() + str + "\r\n");
    scrollLines(getLineCount());
  }
  
};

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  Console m_console;
  bool m_item2Enabled;

public:

  MainFrame()
    : Frame("MenuResource")
    , m_console(this)
    , m_item2Enabled(true)
  {
    setMenuBar(new MenuBar(IDM_MENUBAR));
    setLayout(new ClientLayout);

    // disable Item4
    getMenuBar()->getMenuItemById(IDM_ITEM4)->setEnabled(false);
  }

private:

  virtual bool onActionById(int actionId)
  {
    switch (actionId) {

      case IDM_ITEM1:
	m_console.println("Item1 selected");
	break;

      case IDM_ITEM2:
	if (m_item2Enabled) {
	  m_console.println("Item2 selected");
	}
	break;

      case IDM_ITEM3:
	m_console.println("Item3 selected (change Item2 state)");
	//m_item2Enabled ) {
	break;

      default:
	m_console.println("Unknown action");
	break;
    }

    return true;
  }

//   void initializeCommands()
//   {
//     Command *command;

//     command = new Command(IDM_ITEM1);
//     command->Action.connect(Bind(&Console::println, &m_console, "Item1 selected"));
//     command->Update.connect(Bind(&MainFrame::onUpdateItem1, this));
//     addCommand(command);

//     command = new Command(IDM_ITEM2);
//     command->Action.connect(Bind(&Console::println, &m_console, "Item2 selected"));
//     command->Update.connect(Bind(&MainFrame::onUpdateItem2, this));
//     addCommand(command);

//     command = new Command(IDM_ITEM3);
//     command->Action.connect(Bind(&Console::println, &m_console, "Item3 selected"));
//     command->Action.connect(Bind(&MainFrame::onActionItem3, this));
//     command->Update.connect(Bind(&MainFrame::onUpdateItem3, this));
//     addCommand(command);
//   }

//   void onUpdateItem1(CommandState &cmdState)
//   {
//     m_console.println("Item1 updated");
//   }

//   void onUpdateItem2(CommandState &cmdState)
//   {
//     m_console.println("Item2 updated");
//     cmdState.setEnabled(m_item2Enabled);
//   }

//   void onUpdateItem3(CommandState &cmdState)
//   {
//     m_console.println("Item3 updated");
//     cmdState.setChecked(!m_item2Enabled);
//   }

//   void onActionItem3()
//   {
//     m_item2Enabled = !m_item2Enabled;
//     m_console.println(m_item2Enabled ? "Item2 enabled": "Item2 disabled");
//   }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame m_mainFrame;

public:
  
  virtual void main(std::vector<String> args) {
    m_mainFrame.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example *app(new Example);
  app->run();
  delete app;
  return 0;
}
