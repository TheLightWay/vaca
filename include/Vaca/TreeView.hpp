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

#ifndef VACA_TREEVIEW_HPP
#define VACA_TREEVIEW_HPP

#include <commctrl.h>
#include <boost/iterator/iterator_facade.hpp>

#include "Vaca/base.hpp"
#include "Vaca/Widget.hpp"
#include "Vaca/TreeNode.hpp"

namespace Vaca {

#define ShowSelectionAlwaysTreeViewStyle	\
  (Style(TVS_SHOWSELALWAYS, 0))

#define TreeViewStyle				\
  (ChildStyle +					\
   FocusableStyle +				\
   ClientEdgeStyle +				\
   Style(TVS_LINESATROOT | TVS_HASBUTTONS |	\
	 TVS_HASLINES | TVS_SHOWSELALWAYS |	\
	 TVS_EDITLABELS, 0))

#define EditLabelTreeViewStyle	(Style(TVS_EDITLABELS, 0))

class TreeViewEvent;
class ImageList;

/**
 * @internal You should use #TreeView::iterator.
 */
class VACA_DLL TreeViewIterator
  : public boost::iterator_facade<TreeViewIterator,
				  TreeNode*,
				  boost::bidirectional_traversal_tag,
				  TreeNode*>
{
  TreeNode* m_currentNode;

public:
  TreeViewIterator();
  TreeViewIterator(const TreeViewIterator& other);
  explicit TreeViewIterator(TreeNode* node);

  TreeViewIterator& operator=(const TreeViewIterator& other);

private:
  friend class boost::iterator_core_access;

  void increment();
  void decrement();
  bool equal(TreeViewIterator const& other) const;
  TreeNode* dereference() const;
    
};

/**
 * Handles a TreeView control.  A TreeView (internally) has a root
 * TreeNode that is the parent of the first-level's nodes.
 */
class VACA_DLL TreeView : public Widget
{
  friend class TreeNode;

  TreeNode m_root;
  bool     m_deleted;
  String   m_tmpBuffer; // to use LPSTR_TEXTCALLBACK we need some space
                        // to allocate text temporally

public:
  /**
   * Iterator to go through all the nodes of the tree (it does a deep
   * scan).
   */
  typedef TreeViewIterator iterator;

  TreeView(Widget* parent, Style style = TreeViewStyle);
  virtual ~TreeView();

  iterator begin();
  iterator end();

  void setImageList(ImageList& imageList, int type);
  void setNormalImageList(ImageList& imageList);
  void setStateImageList(ImageList& imageList);

  TreeNode* getRootNode();

  void addNode(TreeNode* node);
  void removeNode(TreeNode* node);

  TreeNode* getSelectedNode();
  void setSelectedNode(TreeNode* node);

  virtual void setBgColor(Color color);

  // signals
  boost::signal<void (TreeViewEvent&)> BeforeExpand;
  boost::signal<void (TreeViewEvent&)> BeforeCollapse;
  boost::signal<void (TreeViewEvent&)> BeforeSelect;
  boost::signal<void (TreeViewEvent&)> BeforeLabelEdit;
  boost::signal<void (TreeViewEvent&)> AfterExpand;
  boost::signal<void (TreeViewEvent&)> AfterCollapse;
  boost::signal<void (TreeViewEvent&)> AfterSelect;
  boost::signal<void (TreeViewEvent&)> AfterLabelEdit;
//   boost::signal<void (TreeViewEvent&)> BeginDrag;
//   boost::signal<void (TreeViewEvent&)> EndDrag;

protected:
  // new events
  virtual void onBeforeExpand(TreeViewEvent& ev);
  virtual void onBeforeCollapse(TreeViewEvent& ev);
  virtual void onBeforeSelect(TreeViewEvent& ev);
  virtual void onBeforeLabelEdit(TreeViewEvent& ev);
  virtual void onAfterExpand(TreeViewEvent& ev);
  virtual void onAfterCollapse(TreeViewEvent& ev);
  virtual void onAfterSelect(TreeViewEvent& ev);
  virtual void onAfterLabelEdit(TreeViewEvent& ev);
//   virtual void onBeginDrag(TreeViewEvent& ev);
//   virtual void onEndDrag(TreeViewEvent& ev);

  // reflection
  virtual bool onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult);

};

} // namespace Vaca

#endif