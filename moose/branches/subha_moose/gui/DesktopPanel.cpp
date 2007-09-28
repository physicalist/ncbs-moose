/*******************************************************************
 * File:            Main.cpp
 * Description:      
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-07-21 11:44:28
 ********************************************************************/
#ifndef DESKTOPPANEL_CPP_
#define DESKTOPPANEL_CPP_
#include "DesktopPanel.h"
#include "Plot.h"
#include <iostream>
#include <cmath>

using std::cerr;
using std::endl;
using std::flush;

static const Cinfo* initDesktopPanelCinfo()
{
    // TODO: figure out a general framework for event passing
//     static Finfo * commandFinfos[] = 
//         {
//             new DestFinfo ( ""),
//         };
    static Finfo * desktopPanelFinfos[] = 
        {
            new ValueFinfo( "x", Ftype1 < int > :: global(),
                            reinterpret_cast < GetFunc > ( &DesktopPanel::_x),
                            reinterpret_cast < RecvFunc > ( &DesktopPanel::_setX)),
            new ValueFinfo( "y", Ftype1 < int > :: global(),
                            reinterpret_cast < GetFunc > ( &DesktopPanel::_y),
                            reinterpret_cast < RecvFunc > ( &DesktopPanel::_setY)),
            new ValueFinfo( "w", Ftype1 < int > :: global(),
                            reinterpret_cast < GetFunc > ( &DesktopPanel::_width),
                            reinterpret_cast < RecvFunc > ( &DesktopPanel::_setWidth)),
            new ValueFinfo( "h", Ftype1 < int > :: global(),
                            reinterpret_cast < GetFunc > ( &DesktopPanel::_height),
                            reinterpret_cast < RecvFunc > ( &DesktopPanel::_setHeight)),
            new ValueFinfo( "show", Ftype1 < bool > :: global(),
                            reinterpret_cast < GetFunc > ( &DesktopPanel::_is_show),
                            reinterpret_cast < RecvFunc > ( &DesktopPanel::_do_show)),
//             new ValueFinfo("size", Ftype2 <int, int> :: global(),
//                            reinterpret_cast < GetFunc > :: ( &DesktopPanel::_size ),
//                            reinterpret_cast < RecvFunc > :: ( &DesktopPanel::_resize )),
//             new ValueFinfo("position", Ftype2 <int, int>::global(),
//                            reinterpret_cast < GetFunc > :: ( &DesktopPanel:: _position ),
//                            reinterpret_cast < RecvFunc > :: ( &DesktopPanel:: _move )),
        };
    
    
    static Cinfo desktopPanelCinfo(
        "DesktopPanel",
        "Subhasis Ray",
        "Main window for MOOSE GUI.",
        initNeutralCinfo(),
        desktopPanelFinfos,
        sizeof( desktopPanelFinfos ) / sizeof( Finfo * ),
        ValueFtype1 < DesktopPanel >::global()
        );
    return &desktopPanelCinfo;    
    
}

static const Cinfo* desktopPanelCinfo = initDesktopPanelCinfo();

int DesktopPanel::_x(const Conn & conn)
{
    DesktopPanel * panel = static_cast< DesktopPanel * > (conn.targetElement()->data());
    return panel->x();
}

int DesktopPanel::_y(const Conn & conn)
{
    DesktopPanel * panel = static_cast < DesktopPanel * >( conn.targetElement()->data());
    return panel->y();    
}


int DesktopPanel::_width(const Conn & conn)
{
    DesktopPanel * panel = static_cast<DesktopPanel * > (conn.targetElement()->data());
    return panel->width();    
}

int DesktopPanel::_height(const Conn & conn)
{
    DesktopPanel * panel = static_cast<DesktopPanel * > (conn.targetElement()->data());
    return panel->height();    
}

std::string DesktopPanel::_size(const Conn& conn)
{
    return "DUMMY_SIZE";    
}
std::string DesktopPanel::_position(const Conn& conn)
{
    return "DUMMY_POS";    
}


void DesktopPanel::setX(int _x)
{
    move(_x,y());    
}
void DesktopPanel::setY(int _y)
{
    move(x(),_y);    
}

void DesktopPanel::setHeight(int _height)
{
     resize(width(),_height);    
}

void DesktopPanel::setWidth(int _width)
{
     resize(_width,height());    
}
void DesktopPanel::_setX(const Conn & conn, int _x)
{
    DesktopPanel * panel = static_cast<DesktopPanel * > (conn.targetElement()->data());
    panel->setX(_x);
}

void DesktopPanel::_setY(const Conn & conn, int _y)
{
    DesktopPanel * panel = static_cast<DesktopPanel * > (conn.targetElement()->data());
    panel->setY(_y);    
}


void DesktopPanel::_setWidth(const Conn & conn, int w ) 
{
        DesktopPanel * panel = static_cast<DesktopPanel * > (conn.targetElement()->data());
    panel->setWidth(w);    
}

void DesktopPanel::_setHeight(const Conn & conn,  int h) 
{
        DesktopPanel * panel = static_cast<DesktopPanel * > (conn.targetElement()->data());
    panel->setHeight(h);    
}

void DesktopPanel::_resize(const Conn & conn, int w, int h)
{
    DesktopPanel * panel = static_cast<DesktopPanel * > (conn.targetElement()->data());
    panel->resize(w,h);    
}

void DesktopPanel::_move(const Conn & conn, int _x, int _y)
{
    DesktopPanel * panel = static_cast<DesktopPanel * > (conn.targetElement()->data());
    panel->move(_x,_y);    
}

DesktopPanel::DesktopPanel(QWidget *parent):QMainWindow(parent)
{
    rightDockWidget_ = new QDockWidget(tr("Right"),this);
    leftDockWidget_ = new QDockWidget(tr("Left"),this);
    workspace_ = new QWorkspace();
    workspace_->resize(200,600);
    
    
//    leftDockWidget_->setWidget(new QWidget(this));    
//    this->addDockWidget(Qt::RightDockWidgetArea,rightDockWidget_);
//  rightDockWidget_->setWidget(new QWidget(this));
    this->setCentralWidget(workspace_);    
}

void DesktopPanel::createMenus()
{
    fileMenu_ = menuBar()->addMenu("&File");
    newAct = new QAction(tr("&New"),this);
//     openAct = open QAction(tr("&Open"),this);
//     saveAct = save QAction(tr("&Save"),this);
    QObject::connect(newAct, SIGNAL(triggered()), this, SLOT(createChildWindow()));  
    fileMenu_->addAction(newAct);
    
    viewMenu_ = menuBar()->addMenu("&View");
    toggleLeftPanelAct_ = new QAction(tr("&Left Panel"), this);
    viewMenu_->addAction(toggleLeftPanelAct_);
    
    QObject::connect(toggleLeftPanelAct_, SIGNAL(triggered()), this, SLOT(toggleLeftPanel()));

    toggleRightPanelAct_ = new QAction(tr("&Right Panel"), this);
    QObject::connect(toggleRightPanelAct_, SIGNAL(triggered()), this, SLOT(toggleRightPanel()));
    viewMenu_->addAction(toggleRightPanelAct_);
    
//     fileMenu_->addAction(openAct);
//     fileMenu_->addAction(saveAct);    
}

void DesktopPanel::createLeftPanel()
{    
    leftDockWidget_->resize(200,600);    
    QTabWidget *leftTabs = new QTabWidget(leftDockWidget_);
    leftTabs->resize(200,300);
    leftDockWidget_->setWidget(leftTabs);
    this->addDockWidget(Qt::LeftDockWidgetArea,leftDockWidget_);

    // Directory Browser - on left panel by default
    QSplitter *browser = new QSplitter();
    browser->setOrientation(Qt::Vertical);    
    browser->resize(leftDockWidget_->size());
    
    QDirModel *model = new QDirModel(browser);
    model->setFilter(QDir::AllEntries);
    
    // Default view is list of directories and files
    QListView * fileList = new QListView(browser);
    fileList->setModel(model);
    fileList->setRootIndex(model->index(QDir::currentPath()));

    // Make it scrollable
    QScrollArea * browserScrollArea = new QScrollArea();
    browserScrollArea->setBackgroundRole(QPalette::Dark);
    browserScrollArea->setWidget(browser);
    
    leftTabs->addTab(browserScrollArea, "Browse Files");        
}

void DesktopPanel::init()
{
    createMenus();
    createLeftPanel();    
}

void DesktopPanel::createChildWindow()
{
    static int childCount = 1;    
    QWidget * child = new QwtPlot(workspace_);
    double *x = new double[1000];
    double *y = new double[1000];
    for ( int i = 0; i < 1000; ++i )
    {
        x[i] = i;
        y[i] = std::sin(i*3.1416/180);
    }
    
    Plot *p1 = new Plot();
    Plot *p2 = new Plot();
    p1->setRawData(x, y, 1000);
    p2->setRawData(x, y, 1000);
    p1->attach((QwtPlot*)child);
    p2->attach((QwtPlot*)child);
    
    QString title = "Untitled "+QString::number(childCount++);
    child->setWindowTitle(title);
    child->resize(150,100);    
    workspace_->addWindow(child);
    child->setVisible(true);    
    std::cerr << "DesktopPanel::createChildWindow() - Created child window titled: " << title.toStdString() << std::endl;
}

void DesktopPanel::toggleLeftPanel()
{
    if(leftDockWidget_->isVisible())
    {
        leftDockWidget_->hide();        
    }else
    {
        leftDockWidget_->show();
    }
}

void DesktopPanel::toggleRightPanel()
{
    if(rightDockWidget_->isVisible())
    {
        rightDockWidget_->hide();        
    }else
    {
        rightDockWidget_->show();
    }    
}

bool DesktopPanel::_is_show(const Conn & conn)
{
    return true; // TODO: dummy
}

void DesktopPanel::_do_show(const Conn & conn, bool value)
{
    DesktopPanel * panel = static_cast<DesktopPanel * > (conn.targetElement()->data());
    panel->show();
}

    
    
#ifdef DO_UNIT_TESTS
#include "../element/Neutral.h"
void testDesktopPanel(int argc, char **argv)
{
    cout << "\nTesting DesktopPanel" << flush;

    Element * n = Neutral::create("Neutral", "n", Element::root() );
    QApplication *app = new QApplication(argc, argv);
    
    Element * w0 = Neutral::create("DesktopPanel", "w0", n);
    ASSERT( w0 != 0, "creating DesktopPanel");
    Conn c( w0, 0);
    DesktopPanel::_setX(c, 10);
    DesktopPanel::_setY(c, 10);
    DesktopPanel::_setHeight(c, 600);
    DesktopPanel::_setWidth(c, 800);
    DesktopPanel::_do_show(c, true);
    app.exec();
    
//    set( n, "destroy");    
}

#endif
#endif // !DESKTOPPANEL_CPP_
