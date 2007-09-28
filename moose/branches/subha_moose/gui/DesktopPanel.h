/*
** DesktopPanel.h
** 
** Made by Subhasis Ray
** Login   <subha@pinni>
** 
** Started on  2007-07-21 11:44:28
// Last update Thu Jul 26 11:14:56 2007 
*/

#ifndef   	DESKTOPPANEL_H_
#define   	DESKTOPPANEL_H_
#include "MooseQt.h"
using namespace Qt;


class DesktopPanel:public QMainWindow
{
    Q_OBJECT
    
  public:
    DesktopPanel(QWidget* parent=0);
    DesktopPanel(const DesktopPanel & source):QMainWindow()
    {
        // Is this used ever? But compilation problem otherwise saying
/*
  DesktopPanel.h: In copy constructor ‘DesktopPanel::DesktopPanel(const DesktopPanel&)’:
  DesktopPanel.h:17:   instantiated from ‘bool Ftype1<T>::strGet(const Element*, const Finfo*, std::string&) const [with T = DesktopPanel]’
  DesktopPanel.cpp:262:   instantiated from here
  /usr/include/qt4/QtGui/qmainwindow.h:117: error: ‘QMainWindow::QMainWindow(const QMainWindow&)’ is private
  DesktopPanel.h:17: error: within this context
  ../../basecode/DerivedFtype.h: In member function ‘bool Ftype1<T>::strGet(const Element*, const Finfo*, std::string&) const [with T = DesktopPanel]’:
  
*/
}


    DesktopPanel& operator=(const DesktopPanel& p)
    {
        // Same purpose as copy constructor
        DesktopPanel *p1 = new DesktopPanel();
        return *p1;
    }

    void init();
    static void _setX( const Conn&, int );
    static void _setY( const Conn&,int );
    static void _setWidth( const Conn&,int );
    static void _setHeight( const Conn&,int );
    static int _x(const Conn&);
    static int _y(const Conn&);
    static int _width(const Conn&);
    static int _height(const Conn&);
    static void _resize(const Conn&,int, int);
    static void _move(const Conn&, int, int);
    static std::string _size(const Conn & conn );
    static std::string _position(const Conn & conn );
    static bool _is_show(const Conn & conn);
    static void _do_show(const Conn & conn, bool show = true);
    void setX( int );
    void setY(int );
    void setWidth( int );
    void setHeight( int );
//     void resize(int, int);
//     void move(int, int);
    
  protected:
    void createMenus();
    void createLeftPanel();
    
    
  private:
    
    
//    DesktopPanel(const DesktopPanel & copy);    
    QDockWidget *rightDockWidget_;
    QDockWidget *leftDockWidget_;
    QWorkspace * workspace_;
    QMenu * fileMenu_;
    QMenu * viewMenu_;
    QAction *toggleLeftPanelAct_;
    QAction *toggleRightPanelAct_;
    
    QAction * newAct, *openAct, *saveAct;
    
 private slots:
    void createChildWindow();
    void toggleLeftPanel();
    void toggleRightPanel();    
};

#endif 	    /* !DESKTOPPANEL_H_ */
