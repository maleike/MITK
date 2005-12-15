#ifndef QMITK_DATATREELISTVIEW_H_INCLUDED_SNewS
#define QMITK_DATATREELISTVIEW_H_INCLUDED_SNewS

/*=========================================================================
 
Program:   Medical Imaging & Interaction Toolkit
Module:    $RCSfile$
Language:  C++
Date:      $Date$
Version:   $Revision$
 
Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.
 
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.
 
=========================================================================*/

#include <qlistview.h>

#include <QmitkDataTreeListViewExpander.h>

/// @brief Displays items of a mitk::DataTreeBase
class QmitkDataTreeListView : public QWidget, public QmitkListViewItemIndex
{
  Q_OBJECT;

  public:

    typedef mitk::DataTreeFilter::PropertyList PropertyList;

    QmitkDataTreeListView(QWidget* parent = 0, const char* name = 0);
    QmitkDataTreeListView(mitk::DataTreeFilter* filter,QWidget* parent = 0, const char* name = 0);
    QmitkDataTreeListView(mitk::DataTreeBase* filter,QWidget* parent = 0, const char* name = 0);
    QmitkDataTreeListView(mitk::DataTreeIteratorBase* filter,QWidget* parent = 0, const char* name = 0);
    ~QmitkDataTreeListView();

    void SetDataTree(mitk::DataTreeBase*);
    void SetDataTree(mitk::DataTreeIteratorBase*);

    void SetFilter(mitk::DataTreeFilter*);

    int stretchedColumn();
    void setStretchedColumn(int);
   
    virtual QSize sizeHint() const;

    // handler for event notification
    void removeItemHandler( const itk::EventObject& e );
    void removeChildrenHandler( const itk::EventObject& e );
    void removeAllHandler( const itk::EventObject& e );
    void selectionChangedHandler( const itk::EventObject& e );
    void itemAddedHandler( const itk::EventObject& e );
    void updateAllHandler( const itk::EventObject& e );
    
  protected:

    void initialize();
    void generateItems();

    void paintListBackground(QPainter& painter, QmitkListViewItemIndex* index);
    virtual void paintEvent(QPaintEvent*);

    virtual void mouseReleaseEvent (QMouseEvent*);

    void clearItems();
    
  private:

    void QmitkDataTreeListView::AddItemsToList(QWidget* parent, QmitkListViewItemIndex* index,
                                               const mitk::DataTreeFilter::ItemList* items,
                                               const mitk::DataTreeFilter::PropertyList& visibleProps,
                                               const mitk::DataTreeFilter::PropertyList editableProps);

    void connectNotifications();
    void disconnectNotifications();
    
    mitk::DataTreeFilter* m_DataTreeFilter;
    int m_StretchedColumn;

    QSize m_SizeHint;
    
    mitk::DataTreeFilter::Pointer m_PrivateFilter;
    const mitk::DataTreeFilter::Item* m_SkipItem;
    const mitk::DataTreeFilter::Item* m_SkipItemParent;

    unsigned long  m_RemoveItemConnection;
    unsigned long  m_RemoveChildrenConnection;
    unsigned long  m_RemoveAllConnection;
    unsigned long  m_SelectionChangedConnection;
    unsigned long  m_ItemAddedConnection;
    unsigned long  m_UpdateAllConnection;

    bool m_SelfCall;
};

#endif

