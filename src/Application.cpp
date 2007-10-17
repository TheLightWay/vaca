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
#include "Vaca/Debug.h"
#include "Vaca/Application.h"
#include "Vaca/Frame.h"

using namespace Vaca;

HINSTANCE Application::mHINSTANCE = NULL;
Application *Application::mInstance = NULL;

/**
 * Initializes the variables of the Application class. Also calls the
 * InitCommonControls.
 */
Application::Application()
  : Thread(true)
{
  assert(Application::mHINSTANCE == NULL);
  assert(Application::mInstance == NULL);

  CoInitialize(NULL);

  Application::mHINSTANCE = ::GetModuleHandle(NULL);
  Application::mInstance = this;

  INITCOMMONCONTROLSEX icce;
  icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
  icce.dwICC = 0
    | ICC_WIN95_CLASSES
    | ICC_BAR_CLASSES
    | ICC_COOL_CLASSES
    ;

  InitCommonControlsEx(&icce);
}

Application::~Application()
{
  assert(Application::mHINSTANCE != NULL);
  assert(Application::mInstance == this);

  Application::mHINSTANCE = NULL;
  Application::mInstance = NULL;

  // delete brushes and pens
  Graphics::deleteHandles();

  // close the log file
  __vaca_trace(NULL, 0, NULL);

  CoUninitialize();
}

Application *Application::getInstance()
{
  return mInstance;
}

HINSTANCE Application::getHINSTANCE()
{
  return Application::mHINSTANCE;
}

/**
 * The common way to start the application. You should call this
 * method from <em>main</em> or <em>WinMain</em>, using an instance of
 * a class derived of Application.
 */
void Application::run()
{
  //////////////////////////////////////////////////////////////////////
  // Convert the command-line to a vector of arguments...
  std::vector<String> args;

  LPTSTR cmdline = _tcsdup(GetCommandLine());
  Character quote;

  for (int i = 0; cmdline[i] != 0; ) {
    // eat spaces
    while (cmdline[i] != 0 && _istspace(cmdline[i]))
      ++i;

    // string with quotes?
    if (cmdline[i] == '\"' || cmdline[i] == '\'')
      quote = cmdline[i++];
    else if (cmdline[i] == 0)
      break;
    else
      quote = 0;

    // read the string
    String arg;
    
    for (; cmdline[i] != 0; ++i) {
      // with quotes
      if (quote != 0) {
	if (cmdline[i] == quote) {
	  ++i;
	  break;
	}
	else if (cmdline[i] == '\\' && cmdline[i+1] == quote)
	  ++i;
      }
      // without quotes
      else if (_istspace(cmdline[i]))
	break;

      arg.push_back(cmdline[i]);
    }

    args.push_back(arg);
  }

  free(cmdline);
  //////////////////////////////////////////////////////////////////////

  main(args);
  doMessageLoop();
}
