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
#include "scintilla/include/Scintilla.h"
#include "resource.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// View

class Document;

class View
{
protected:
  Document *m_document;

public:
  virtual ~View() { }
  virtual void onNotifyDocument() = 0;

  virtual Document *getDocument() { return m_document; }
};

//////////////////////////////////////////////////////////////////////
// Document

class Document
{
  String m_fileName;		// current file name for the document
  bool m_hasFileName;		// this document has a file name?
  std::vector<View *> m_views;	// Views attached to this Document

  typedef std::vector<View *>::iterator iterator;

public:
  
  Document(String fileName, bool hasFileName)
    : m_fileName(fileName)
    , m_hasFileName(hasFileName)
  {
  }

  void addView(View *view)
  {
    m_views.push_back(view);
    notify();
  }

  void removeView(View *view)
  {
    remove_element_from_container(m_views, view);
    notify();
  }

  int getViewNumber(View *view)
  {
    int index = 1;
    for (iterator it=m_views.begin(); it!=m_views.end(); ++it, ++index) {
      if (*it == view)
	return index;
    }
    return 0;
  }

  int getViewCount() { return m_views.size(); }
  bool hasFileName() { return m_hasFileName; }
  String getFileName() { return m_fileName; }

  void setHasFileName(bool state)
  {
    m_hasFileName = state;
  }

  void setFileName(const String &fileName)
  {
    m_fileName = fileName;
    notify();
  }

  // notifies all views that must be updated (the title-bar mainly,
  // because Scintilla handles views internally)
  void notify()
  {
    for (iterator it=m_views.begin(); it!=m_views.end(); ++it)
      (*it)->onNotifyDocument();
  }

};

//////////////////////////////////////////////////////////////////////
// A MdiChild with a SciEditor in its client area

class TextEditor : public MdiChild, public View
{
  SciEditor m_editor; // text editor inside this MdiChild frame

public:

  // creates a new TextEditor
  TextEditor(const String &fileName, bool hasFileName, MdiFrame *parent)
    : MdiChild(fileName.getFileName(), parent,
	       MdiChildStyle + ClientEdgeStyle)
    , m_editor(this)
  {
    // creates a new document
    m_document = new Document(fileName, hasFileName);

    // common initialization
    initialize();
  }

  // creates a new view for the specified textEditor
  TextEditor(TextEditor &textEditor)
    : MdiChild(textEditor.getText(),
	       dynamic_cast<MdiClient *>(textEditor.getParent()),
	       MdiChildStyle + ClientEdgeStyle)
    , View()
    , m_editor(this)
  {
    // same document
    m_document = textEditor.m_document;

    // reference the same document pointer at Scintilla level (only
    // necessary for Scintilla)
    m_editor.setDocPointer(textEditor.m_editor.getDocPointer());

    // common initialization
    initialize();
  }

  virtual ~TextEditor()
  {
    // remove this view from the document
    m_document->removeView(this);

    // last view closed?
    if (m_document->getViewCount() == 0)
      delete m_document;
  }

private:

  void initialize()
  {
    // Set icon
    setIcon(IDI_VACADOC);

    // the editor'll be arranged to client area bounds
    setLayout(new ClientLayout);

    // on GotFocus or Activate signals, put the focus to the m_editor
    // (so the user can start writting)
    GotFocus.connect(Bind(&SciEditor::requestFocus, &m_editor));
    // Activate.connect(Bind(&SciEditor::requestFocus, &m_editor));

    // add this view to the document
    m_document->addView(this);

    // when the text or the selection of the editor is updated, we
    // call m_document->notify() to put the "*" in the title-bar of the
    // TextEditor
    m_editor.UpdateUI.connect(Bind(&Document::notify, m_document));
  }

  // method from View class
  virtual void onNotifyDocument()
  {
    String newTitle =
      // the "*"
      (m_editor.isModified() ? "* ": "") +
      // the name of the file
      getFileName().getFileName() +
      // the view number (only if it's necessary)
      (isLastView() ? "": " : "+String::fromInt(m_document->getViewNumber(this)));

    // is the text different? (this avoid flicker of the title-bar
    // when we write text)
    if (getText() != newTitle)
      setText(newTitle);

    // here we don't need to invalidate the m_editor of each view,
    // because it's done by Scintilla automatically
  }

public:

  // opens a file and put its text into the editor
  bool openFile()
  {
    FILE *file = _tfopen(getFileName().c_str(), _T("rb"));
    if (file != NULL) {
      char buf[4096];
      int bytesReaded;
      while (!feof(file)) {
	bytesReaded = fread(buf, 1, sizeof(buf), file);
	m_editor.appendText(buf, bytesReaded);
      }
      fclose(file);

      // because the m_editor.appendText, we must reset the save-point
      // and undo information
      m_editor.emptyUndoBuffer();
      m_editor.setSavePoint();
      return true;
    }
    else
      return false;
  }

  // saves the text of the editor in a file
  bool saveFile()
  {
    FILE *file = _tfopen(getFileName().c_str(), _T("wb"));
    if (file != NULL) {
      int c, lines = m_editor.getLineCount();
      for (c=0; c<lines; c++) {
	String line = m_editor.getLine(c);
	fwrite(line.c_str(), 1, line.size(), file);
      }
      fclose(file);

      // set save-point (here we are in sync with the file in disk)
      m_editor.setSavePoint();
      // now the document has a file name
      m_document->setHasFileName(true);
      // and finally notify all views to update the "*" mark in them title-bar
      m_document->notify();
      return true;
    }
    else
      return false;
  }
  
  bool hasFileName() { return m_document->hasFileName(); }
  String getFileName() { return m_document->getFileName(); }
  SciEditor &getEditor() { return m_editor; }
  bool isLastView() { return m_document->getViewCount() == 1; }

  void setFileName(const String &fileName)
  {
    m_document->setFileName(fileName);
  }

};

//////////////////////////////////////////////////////////////////////
// A MdiFrame with a collection of TextEditor as MdiChild

class MainFrame : public MdiFrame
{
  int m_docCounter; // counter for documents (only to make "Untitled1", "Untitled2", etc.)
  Font m_font;
  bool m_viewEol;
  FindTextDialog *m_findDlg;

public:

  MainFrame()
    : MdiFrame("TextEditor")
    , m_docCounter(0)
    , m_font("Courier New", 10)
    , m_viewEol(false)
    , m_findDlg(NULL)
  {
    setMenuBar(createMenuBar());
    setIcon(IDI_VACA);
  }

  virtual ~MainFrame()
  {
    // delete all TextEditors
    Widget::Container editors = getMdiClient()->getChildren();

    for (Widget::Container::iterator it=editors.begin(); it!=editors.end(); ++it) {
      TextEditor *textEditor = dynamic_cast<TextEditor *>(*it);
      delete_widget(textEditor);
    }
  }

private:

  MenuBar *createMenuBar()
  {
    MenuBar *menuBar = new MenuBar;
    Menu *fileMenu = new Menu("&File");
    Menu *editMenu = new Menu("&Edit");
    Menu *optionsMenu = new Menu("&Options");
    MdiListMenu *windowsMenu = new MdiListMenu("&Windows");
    MenuItem *menuItem;

    // File/New
    menuItem = fileMenu->add("&New\tCtrl+N", Keys::Control | Keys::N);
    menuItem->Action.connect(Bind(&MainFrame::onNew, this));

    // File/Open
    menuItem = fileMenu->add("&Open\tCtrl+O", Keys::Control | Keys::O);
    menuItem->Action.connect(Bind(&MainFrame::onOpen, this));

    // ---
    fileMenu->addSeparator();

    // File/Save
    menuItem = fileMenu->add("&Save\tCtrl+S", Keys::Control | Keys::S);
    menuItem->Action.connect(Bind(&MainFrame::onSave, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_forSave, this));

    // File/Save As
    menuItem = fileMenu->add("Save &as...");
    menuItem->Action.connect(Bind(&MainFrame::onSaveAs, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // ---
    fileMenu->addSeparator();

    // File/Exit
    menuItem = fileMenu->add("E&xit");
    menuItem->Action.connect(Bind(&MainFrame::onExit, this));

    // Edit/Undo
    menuItem = editMenu->add("&Undo\tCtrl+Z", Keys::Control | Keys::Z);
    menuItem->Action.connect(Bind(&MainFrame::onUndo, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_canUndo, this));

    // Edit/Redo
    menuItem = editMenu->add("&Redo\tCtrl+Y", Keys::Control | Keys::Y);
    menuItem->Action.connect(Bind(&MainFrame::onRedo, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_canRedo, this));

    // ---
    editMenu->addSeparator();

    // Edit/Cut
    menuItem = editMenu->add("Cu&t\tCtrl+X", Keys::Control | Keys::X);
    menuItem->Action.connect(Bind(&MainFrame::onCut, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // Edit/Copy
    menuItem = editMenu->add("&Copy\tCtrl+C", Keys::Control | Keys::C);
    menuItem->Action.connect(Bind(&MainFrame::onCopy, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // Edit/Paste
    menuItem = editMenu->add("&Paste\tCtrl+V", Keys::Control | Keys::P);
    menuItem->Action.connect(Bind(&MainFrame::onPaste, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // Edit/Clear
    menuItem = editMenu->add("Clea&r");
    menuItem->Action.connect(Bind(&MainFrame::onClear, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // ---
    editMenu->addSeparator();

    // Edit/Find
    menuItem = editMenu->add("&Find\tCtrl+F", Keys::Control | Keys::F);
    menuItem->Action.connect(Bind(&MainFrame::onFindReplace, this, false));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // Edit/Replace
    menuItem = editMenu->add("R&eplace\tCtrl+H", Keys::Control | Keys::H);
    menuItem->Action.connect(Bind(&MainFrame::onFindReplace, this, true));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // Options/Change Font
    menuItem = optionsMenu->add("Change &Font");
    menuItem->Action.connect(Bind(&MainFrame::onChangeFont, this));

    // Options/View EOL
    menuItem = optionsMenu->add("View &EOL");
    menuItem->Action.connect(Bind(&MainFrame::onToggleViewEol, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_viewEol, this));

    // Windows/Close
    menuItem = windowsMenu->add("&Close\tCtrl+W", Keys::Control | Keys::W);
    menuItem->Action.connect(Bind(&MainFrame::onCloseWindow, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // Windows/Duplicate
    menuItem = windowsMenu->add("&Duplicate\tCtrl+D", Keys::Control | Keys::D);
    menuItem->Action.connect(Bind(&MainFrame::onDuplicateWindow, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // Windows/Cascade
    menuItem = windowsMenu->add("&Cascade");
    menuItem->Action.connect(Bind(&MdiClient::cascade, getMdiClient()));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // Menu bar
    menuBar->add(fileMenu);
    menuBar->add(editMenu);
    menuBar->add(optionsMenu);
    menuBar->add(windowsMenu);

    return menuBar;
  }

  //////////////////////////////////////////////////////////////////////
  // signal bindings

  void onNew()
  {
    String title = String("Untitled")+String::fromInt(++m_docCounter);
    addTextEditor(new TextEditor(title, false, this));
  }

  void onOpen()
  {
    OpenFileDialog dlg("Open file", this);
    addFilters(dlg);

    dlg.setMultiselect(true);

    if (dlg.doModal()) {
      // selected file names
      std::vector<String> fileNames = dlg.getFileNames();

      // for each selected file name
      for (std::vector<String>::iterator it=fileNames.begin(); it!=fileNames.end(); ++it) {
	String fileName = *it;

	// check if this file is already opened
	TextEditor *textEditor = getTextEditorByFileName(fileName);

	// active that editor (if exist)
	if (textEditor != NULL) {
	  getMdiClient()->activate(textEditor);
	}
	// create the new editor reading the file
	else {
	  textEditor = new TextEditor(fileName, true, this);

	  // open the file
	  if (textEditor->openFile())
	    addTextEditor(textEditor);
	  else {
	    delete_widget(textEditor);
	    msgBox("Error opening file '"+fileName+"'",
		   "Error",
		   MB_OK | MB_ICONERROR);
	    break;
	  }
	}
      }
    }
  }

  void onSave()
  {
    saveTextEditor(getTextEditor(), "Save file", false);
  }

  void onUpdate_forSave(MenuItemEvent &ev)
  {
    TextEditor *textEditor = getTextEditor();
    ev.getMenuItem()->setEnabled((textEditor != NULL)
				 && (!(textEditor->hasFileName())
				     || (textEditor->getEditor().isModified())));
  }

  void onSaveAs()
  {
    saveTextEditor(getTextEditor(), "Save file as...", true);
  }

  void onUpdate_getTextEditor_Available(MenuItemEvent &ev)
  {
    ev.getMenuItem()->setEnabled(getTextEditor() != NULL);
  }

  void onExit()
  {
    CloseEvent ev(this);
    onClose(ev);
    if (!ev.isCanceled())
      // DISPOSE();
      setVisible(false);
  }

  void onUndo()
  {
    getTextEditor()->getEditor().undo();
  }

  void onUpdate_canUndo(MenuItemEvent &ev)
  {
    TextEditor *textEditor = getTextEditor();
    ev.getMenuItem()->setEnabled((textEditor != NULL)
				 && (textEditor->getEditor().canUndo()));
  }

  void onRedo()
  {
    getTextEditor()->getEditor().redo();
  }

  void onUpdate_canRedo(MenuItemEvent &ev)
  {
    TextEditor *textEditor = getTextEditor();
    ev.getMenuItem()->setEnabled((textEditor != NULL)
				 && (textEditor->getEditor().canRedo()));
  }

  void onCut()
  {
    getTextEditor()->getEditor().cutTextToClipboard();
  }

  void onCopy()
  {
    getTextEditor()->getEditor().copyTextToClipboard();
  }

  void onPaste()
  {
    getTextEditor()->getEditor().pasteTextFromClipboard();
  }

  void onClear()
  {
    getTextEditor()->getEditor().clearText();
  }

  //////////////////////////////////////////////////////////////////////
  // Find stuff begin
  void onFindReplace(bool replace)
  {
    if (m_findDlg == NULL) {
      m_findDlg = new FindTextDialog(replace, this);
      m_findDlg->FindNext.connect(Bind(&MainFrame::onFindNext, this));
      m_findDlg->Replace.connect(Bind(&MainFrame::onReplace, this));
      m_findDlg->ReplaceAll.connect(Bind(&MainFrame::onReplaceAll, this));
      m_findDlg->Cancel.connect(Bind(&MainFrame::onCancelFind, this));
      m_findDlg->setVisible(true);
    }
    else
      m_findDlg->requestFocus();
  }

  void onFindNext()
  {
    SciEditor &sciEditor(getTextEditor()->getEditor());
    String findWhat = m_findDlg->getFindWhat();
    int flags =
      (m_findDlg->isMatchCase() ? SCFIND_MATCHCASE: 0) |
      (m_findDlg->isWholeWord() ? SCFIND_WHOLEWORD: 0);

    // forward search
    if (m_findDlg->isForward()) {
      sciEditor.goToPos(sciEditor.getSelectionEnd());
      sciEditor.searchAnchor();

      if (!sciEditor.searchNext(flags, findWhat))
	Beep(100, 10);

      int start = sciEditor.getSelectionStart();
      int end = sciEditor.getSelectionEnd();
      sciEditor.goToPos(end);
      sciEditor.setAnchor(start);
    }
    // backward search
    else {
      sciEditor.goToPos(sciEditor.getSelectionStart());
      sciEditor.searchAnchor();

      if (!sciEditor.searchPrev(flags, findWhat))
	Beep(100, 10);

      int start = sciEditor.getSelectionStart();
      int end = sciEditor.getSelectionEnd();
      sciEditor.goToPos(start);
      sciEditor.setAnchor(end);
    }
  }

  void onReplace()
  {
    SciEditor &sciEditor(getTextEditor()->getEditor());
    int start = sciEditor.getSelectionStart();
    int end = sciEditor.getSelectionEnd();

    sciEditor.goToPos(start);

    onFindNext();

    if (start != end &&
	start == sciEditor.getSelectionStart() &&
	end == sciEditor.getSelectionEnd()) {
      sciEditor.replaceSel(m_findDlg->getReplaceWith());
      onFindNext();
    }
  }

  void onReplaceAll()
  {
    SciEditor &sciEditor(getTextEditor()->getEditor());
    do {
      onReplace();
    } while (sciEditor.getSelectionStart() != sciEditor.getSelectionEnd());
  }

  void onCancelFind()
  {
    delete_widget(m_findDlg);
    m_findDlg = NULL;
  }
  // Find stuff end
  //////////////////////////////////////////////////////////////////////

  void onChangeFont()
  {
    FontDialog dialog(&m_font, this);
    if (dialog.doModal()) {
      Container editors = getMdiClient()->getChildren();

      // set the font of all editors
      for (Widget::Container::iterator it=editors.begin(); it!=editors.end(); ++it)
	dynamic_cast<TextEditor *>(*it)->getEditor().setFont(&m_font);
    }
  }

  void onToggleViewEol()
  {
    m_viewEol = !m_viewEol;

    // set the new state of ViewEol to all editors
    Widget::Container editors = getMdiClient()->getChildren();

    for (Widget::Container::iterator it=editors.begin(); it!=editors.end(); ++it)
      dynamic_cast<TextEditor *>(*it)->getEditor().setViewEol(m_viewEol);
  }

  void onUpdate_viewEol(MenuItemEvent &ev)
  {
    ev.getMenuItem()->setChecked(m_viewEol);
  }

  void onCloseWindow()
  {
    TextEditor *textEditor = getTextEditor();
    if (closeTextEditor(textEditor, false))
      delete_widget(textEditor);
  }

  void onDuplicateWindow()
  {
    addTextEditor(new TextEditor(*getTextEditor()));
  }

  // when the user press the close button in a MdiChild (TextEditor child)
  void onCloseTextEditor(CloseEvent &ev)
  {
    TextEditor *textEditor = dynamic_cast<TextEditor *>(ev.getSource());

    // show the warning dialog
    if (closeTextEditor(textEditor, false)) {
      // the user really want to close this...

      // the widget must be deleted but we can't delete it because
      // we're inside an event that was generated by the same widget
      // (see TN006)
      delete_widget(textEditor);
    }
    // the user cancels the close operation
    else {
      // cancel the close event (don't close the TextEditor)
      ev.cancel();
    }
  }

  //////////////////////////////////////////////////////////////////////
  // events

  // when the user press the close button of the MainFrame we must
  // close all the editors
  virtual void onClose(CloseEvent &ev)
  {
    MdiFrame::onClose(ev);

    std::vector<Document *> asked;
    Widget::Container editors = getMdiClient()->getChildren();

    // for each children
    for (Widget::Container::iterator it=editors.begin(); it!=editors.end(); ++it) {
      TextEditor *textEditor = dynamic_cast<TextEditor *>(*it);
      Document *document = textEditor->getDocument();

      // we already asked to close document?
      if (std::find(asked.begin(), asked.end(), document) != asked.end())
	continue;
      // if not, add it to the list
      else
	asked.push_back(document);

      // close the editor
      if (!closeTextEditor(textEditor, true)) {
	// the user cancels the close operation...

	// cancel the event (don't close the MainFrame)
	ev.cancel();

	// don't delete any TextEditor
	return;
      }
    }
  }

  //////////////////////////////////////////////////////////////////////
  // logic
private:

  // returns the current text editor
  TextEditor *getTextEditor()
  {
    return dynamic_cast<TextEditor *>(getMdiClient()->getActive());
  }

  // adds file filters to the FileDialog
  void addFilters(FileDialog &dlg)
  {
    dlg.addFilter("All (*.*)", "*.*", true);
    dlg.addFilter("Text files (*.txt)", "*.txt");
  }

  // adds a new text editor to the application
  void addTextEditor(TextEditor *textEditor)
  {
    // set the font of the editor to the current selected font
    textEditor->getEditor().setFont(&m_font);

    // set the ViewEol state
    textEditor->getEditor().setViewEol(m_viewEol);

    // bind the close event
    textEditor->Close.connect(Bind(&MainFrame::onCloseTextEditor, this));
  }

  // Saves the file that is associated with 'textEditor'.  Returns
  // true if the file was saved, or false in case of error or if the
  // user cancels the operation.
  bool saveTextEditor(TextEditor *textEditor, const String &saveDialogTitle,
		      bool forceSelectName)
  {
    // the file doesn't have a file name?
    if (forceSelectName || !textEditor->hasFileName()) {
      // create the dialog to select the file name
      SaveFileDialog dlg(saveDialogTitle, this);
      addFilters(dlg);

      // set initial file name for the dialog
      dlg.setFileName(textEditor->getFileName());

      // show the dialog to the user, and ask if it press OK
      if (dlg.doModal()) {
	// the user press OK...

	// change the name of the file in the text editor (to update the
	// title, and to known that this file has another name now)
	textEditor->setFileName(dlg.getFileName());
      }
      // the user cancels the operation
      else
	return false;
    }

    // save the file to the disk
    if (textEditor->saveFile()) {
      // all just fine
      return true;
    }
    // error...
    else {
      msgBox("Error saving file '"+textEditor->getFileName()+"'",
	     "Error",
	     MB_OK | MB_ICONERROR);
      return false;
    }
  }

  // returns false if the user don't want to close the text editor
  bool closeTextEditor(TextEditor *textEditor, bool forceAsk)
  {
    if (m_findDlg != NULL)
      onCancelFind();

    if ((textEditor->isLastView() || forceAsk) &&
	textEditor->getEditor().isModified()) {
      int ret = msgBox("Save changes to '"+textEditor->getFileName()+"'?",
		       "TextEditor",
		       MB_YESNOCANCEL | MB_ICONQUESTION);

      if (ret == IDCANCEL) {
	return false;		// "Cancel" in message-box
      }
      else if (ret == IDYES) {
	if (!saveTextEditor(textEditor, "Save file", false))
	  return false;		// "Cancel" in save-file dialog
      }
    }
    return true;
  }

  // returns the first found TextEditor that is editing the "fileName"
  TextEditor *getTextEditorByFileName(const String &fileName)
  {
    Widget::Container listOfTextEditors = getMdiClient()->getChildren();
    Widget::Container::iterator it;

    for (it=listOfTextEditors.begin(); it!=listOfTextEditors.end(); ++it) {
      TextEditor *textEditor = dynamic_cast<TextEditor *>(*it);

      if (textEditor->getFileName() == fileName)
	return textEditor;
    }

    return NULL;
  }

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
