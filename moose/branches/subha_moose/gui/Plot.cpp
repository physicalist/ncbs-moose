/*******************************************************************
 * File:            Plot.cpp
 * Description:      
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-07-20 10:54:40
 ********************************************************************/

#ifndef PLOT_CPP_
#define PLOT_CPP_
#include "Plot.h"
#include "moose.h"
#include "header.h"
#include <map>
#include <algorithm>
#include <qwt_symbol.h>
#include <QString>
const Cinfo * initPlotCinfo()
{
    static Finfo * dataFinfos[] = 
        {
            // TODO: Consider how much data duplication we want
            // domain X might be common for many curves Y
            // Also , should we use vector of pairs of doubles rather than two
            // vectors of doubles? The formers ensures the number of
            // X's and Y's remain identical
            new DestFinfo( "X", Ftype1 <double> ::global(), RFCAST( &Plot::_appendX )),
            new DestFinfo( "Y", Ftype1 <double> :: global(), RFCAST ( &Plot::_appendY )),
            new DestFinfo( "XY", Ftype2 < double, double > :: global(), RFCAST( &Plot::_appendXY)),
        };
    
    static Finfo * plotFinfos[] = 
        {
            // Set curveType to Yfx or the other way round
            new ValueFinfo( "Yfx",
                            Ftype1 <bool> :: global(),
                            reinterpret_cast<GetFunc> ( &Plot::_getYfx),
                            reinterpret_cast<RecvFunc> ( &Plot::_setYfx)),
            // Set the line colour
            new ValueFinfo ("color",
                            Ftype1 <std::string>::global(),
                            reinterpret_cast<GetFunc> ( &Plot::_getColor),
                            reinterpret_cast<RecvFunc> ( &Plot::_setColor)),
            // Set line style (as defined in Qwt)
            new ValueFinfo("style",
                           Ftype1 <std::string>::global(),
                           reinterpret_cast<GetFunc> (&Plot::_getPlotStyle),
                           reinterpret_cast<RecvFunc> (&Plot::_setPlotStyle)),
            // Line width 
            new ValueFinfo("width",
                           Ftype1< int >::global(),
                           reinterpret_cast< GetFunc> (&Plot::_getPlotWidth ),
                           reinterpret_cast <RecvFunc> ( &Plot::_setPlotWidth)),
            // Symbol used for data points
            new ValueFinfo("symbol",
                           Ftype1< std::string >::global(),
                           reinterpret_cast< GetFunc > (&Plot::_getPlotSymbol),
                           reinterpret_cast < RecvFunc > ( &Plot::_setPlotSymbol )),
        };

    static Cinfo plotCinfo(
        "Plot",
        "Subhasis Ray",
        "Plot object, for creating data plots.",
        initNeutralCinfo(),
        plotFinfos,
        sizeof( plotFinfos ) / sizeof( Finfo* ),
        ValueFtype1< Plot >::global()
	);

	return &plotCinfo;

    
}

const Cinfo * Plot::plotCinfo_ = initPlotCinfo();


Plot::Plot():QwtPlotCurve()
{
    x_ = y_ = NULL;    
    new_x_ = 0;
    new_x_ = 0;
    myAllocated_ = false;
    
    size_ = 0;
    capacity_ = 0;
    capacity_incr_ = 10;    
}

Plot::Plot(const QwtText & title ): QwtPlotCurve(title)
{
    x_ = y_ = NULL;    
    new_x_ = 0;
    new_x_ = 0;
    myAllocated_ = false;
    
    size_ = 0;
    capacity_ = 0;
    capacity_incr_ = 10;    
}

Plot::Plot(const QString& title ): QwtPlotCurve(title)
{
    x_ = y_ = NULL;    
    new_x_ = 0;
    new_x_ = 0;
    myAllocated_ = false;
    
    size_ = 0;
    capacity_ = 0;
    capacity_incr_ = 10;    
}

// Plot::~Plot()
// {
//      std::free(x_);
//      std::free(y_);
// //    ~((QwtPlotCurve)this); // TODO check if correct    
// }



void Plot::setCapacity( int capacity)
{
    if ( capacity < size_ )
    {
        cerr << "Error: Plot::setCapacity(int capacity) - capacity cannot be less than current number of data points." << endl;
        return;
    }
    
    capacity_ = capacity;    
}

void Plot::_setCapacity(const Conn & conn, int capacity)
{
    Plot * p =(static_cast< Plot* > ( conn.targetElement()->data() ));
    
    if ( capacity < p->size_ )
    {
        cerr << "Error: Plot::setCapacity(int capacity) - capacity cannot be less than current number of data points." << endl;
        return;
    }
   
    p->capacity_ = capacity;    
}


int Plot::getCapacity(void)
{
    return capacity_;
}
int Plot::_getCapacity(const Conn & conn)
{
    return (static_cast< Plot* > ( conn.targetElement()->data() ))->capacity_;
}


int Plot::getSize(void)
{
    return size_;
}

int Plot::_getSize(const Conn & conn)
{
    return (static_cast< Plot* > ( conn.targetElement()->data() ))->size_;
}

double * Plot::getX()
{
    return x_;
}
double * Plot::_getX(const Conn & conn )
{
    return (static_cast< Plot* > ( conn.targetElement()->data() ))->x_;
}



double * Plot::getY(void)
{
    return y_;
}
double * Plot::_getY(const Conn & conn)
{
    return (static_cast< Plot* > ( conn.targetElement()->data() ))->y_;
}

/**
   Get the curveType returns true if curve is plotted with X as the
   free variable, false otherwise
*/

bool Plot::getYfx(void)
{
    return ( curveType() == QwtPlotCurve::Yfx );
}
bool Plot::_getYfx(const Conn & conn)
{
    return ( (static_cast< Plot* > ( conn.targetElement()->data() ))->curveType() == QwtPlotCurve::Yfx );
}

void Plot::setYfx(bool yfx)
{
    if ( yfx )
    {
        setCurveType(QwtPlotCurve::Yfx);
    }
    else
    {
        setCurveType(QwtPlotCurve::Xfy);
    }
}


void Plot::_setYfx(const Conn & conn, bool yfx)
{
    if ( yfx )
    {
        (static_cast< Plot* > ( conn.targetElement()->data() ))->setCurveType(QwtPlotCurve::Yfx);
    }
    else
    {
        (static_cast< Plot* > ( conn.targetElement()->data() ))->setCurveType(QwtPlotCurve::Xfy);
    }
}

/**
   Append the value x to the data list if there is already a
   corresponding y value available. Other wise keep the x value separately.
 */
void Plot::appendX(double x)
{
    x_y_ += 1;
    new_x_ = x;
    
    if (x_y_ == 0)
    {
        appendXY(new_x_, new_y_);        
    }
}
void Plot::_appendX(const Conn& conn, double x)
{
    (static_cast< Plot* > ( conn.targetElement()->data() ))->appendX(x);
}
/**
   Append the value y to the data if there is a corresponding x value
    available, otherwise keep this value for future use.
*/
void Plot::appendY(double y)
{
    x_y_ -= 1;
    new_y_ = y;

    if ( x_y_ == 0 )
    {
        appendXY(new_x_, new_y_);
    }    
}
void Plot::_appendY(const Conn& conn,double y)
{
    (static_cast< Plot* > ( conn.targetElement()->data() ))->appendY(y);    
}

/**
   Append the pair (x, y) to the data list
 */

void Plot::appendXY(double x, double y)
{
    if ( size_ == capacity_ )
    {
        capacity_ += capacity_incr_;        
        double* newX = (double*) realloc( x_, size_t(capacity_ ));
        double* newY = (double*) realloc(y_, size_t(capacity_ ) );
        
        if ( ( newX == NULL ) || ( newY == NULL ) )
        {
            cerr << "Plot::appendX() - ERROR: realloc failed." << endl;
            return;            
        }
        else
        {
            x_ = newX;
            y_ = newY;            
        }
    }
    x_[size_] = x;        
    y_[size_] = y;
    ++size_;
    QwtPlotCurve::setRawData(x_, y_, size_);
}

void Plot::_appendXY(const Conn& conn,double x, double y)
{
    (static_cast< Plot* > ( conn.targetElement()->data() ))->appendXY(x,y);
    
}

// Overrides the same in QwtPlotCurve
void Plot::setRawData(double *x, double * y, int size)
{
    if (myAllocated_)
    {
        std::free(x_);        
        std::free(y_);
    }    
    
    x_ = x;
    y_ = y;
    size_ = size;
    capacity_ = size;
    ((QwtPlotCurve*)this)->setRawData(x_,y_,size_);    
}
// Overrides the same in QwtPlotCurve
void Plot::setData(double * x, double * y, int size)
{
    double *tmp_x = (double*)realloc(x_, (size_t)size);
    double *tmp_y = (double*)realloc(y_, (size_t)size);
    if (( tmp_x != NULL ) && (tmp_y != NULL ))
    {
        x_ = tmp_x;
        y_ = tmp_y;
        myAllocated_ = true;
        size_ = size;
        QwtPlotCurve::setData(x_, y_, size_);        
    }
    else
    {
        std::free(tmp_x);
        std::free(tmp_y);
        cerr << "Error: Plot::setData(double * x, double * y, int size) - failed to allocate memory!" << endl;        
    }
}
// Overrides the same in QwtPlotCurve
void Plot::setData( QwtArray <double > & x, QwtArray < double > & y )
{
    QwtArray<double> :: iterator xIter;
    QwtArray<double> :: iterator yIter;
    size_ = capacity_ = x.size();
    double * tmp_x = (double*)realloc(x_, x.size());
    double * tmp_y = (double*)realloc(y_, y.size());
    
    
    if ( (tmp_x != NULL) && (tmp_y != NULL))
    {
        int i;
        myAllocated_ = true;
        x_ = tmp_x;
        y_ = tmp_y;

        for ( i = 0, xIter = x.begin(); xIter != x.end(); ++ xIter, ++i )
        {
            x_[i] = x[i];
        }

        for ( i = 0, yIter = y.begin(); yIter != y.end(); ++yIter, ++i)
        {
            y_[i] = y[i];            
        }
    }         
}

/**
   Sets the line style for the plot
   TODO: see if this style is same as style in QPen object
*/
void Plot::setPlotStyle(std::string style)
{
    static map <std::string, QwtPlotCurve::CurveStyle> styleMap;
    if (styleMap.empty()) {

        styleMap["none"] = QwtPlotCurve::NoCurve;
        styleMap[" "] = QwtPlotCurve::NoCurve;
        styleMap["lines"] = QwtPlotCurve::Lines;
        styleMap["-"] = QwtPlotCurve::Lines;
        styleMap["sticks"] = QwtPlotCurve::Sticks;
        styleMap["|"] = QwtPlotCurve::Sticks;
        styleMap["steps"] = QwtPlotCurve::Steps;
        styleMap["_"] = QwtPlotCurve::Steps;
        styleMap["dots"] = QwtPlotCurve::Dots;
        styleMap["."] = QwtPlotCurve::Dots;
    }
    std::transform(style.begin(), 
                   style.end(),
                   style.begin(),
                   (int(*)(int))std::tolower);
 
    std::map<std::string,QwtPlotCurve::CurveStyle>::iterator st = styleMap.find(style);
    if ( st != styleMap.end() )
    {
        setStyle(st->second);
    }
    else
    {
        cerr << "Plot::setPlotStyle(std::string style) - " << style << " is not a valid style." << endl;
    }    
}
void Plot::_setPlotStyle(const Conn & conn, std::string style)
{
    (static_cast< Plot* > ( conn.targetElement()->data() ))->setPlotStyle(style);    
}

/**
   returns the name of the plot style as a std::std::string
*/
const string Plot::getPlotStyle()
{
    switch (style())
    {
        case  ( QwtPlotCurve::NoCurve ) : return "NoStyle";
        case ( QwtPlotCurve::Lines ) :   return "Lines";
        case ( QwtPlotCurve::Sticks ) : return "Sticks";
        case ( QwtPlotCurve::Steps ) :  return "Steps";
        case ( QwtPlotCurve::Dots ) :   return "Dots";
        default:                        return "UserDefined";            
    }    
}

const std::string Plot::_getPlotStyle(const Conn & conn)
{
    return (static_cast< Plot* > ( conn.targetElement()->data() ))->getPlotStyle();    
}


void Plot::setPlotSymbol(std::string symbol)
{
    static map < std::string , QwtSymbol::Style > symbolMap;
    if ( symbolMap.empty() ) {
        symbolMap["none"] = QwtSymbol::NoSymbol;
        symbolMap["nosymbol"] =QwtSymbol::NoSymbol;
        symbolMap[" "] = QwtSymbol::NoSymbol;
        symbolMap["ellipse"] = QwtSymbol::Ellipse;
        symbolMap["0"] = QwtSymbol::Ellipse;
        symbolMap["rect"] = QwtSymbol::Rect;
        symbolMap["[]"] = QwtSymbol::Rect;
        symbolMap["diamond"] = QwtSymbol::Diamond;
        symbolMap["<>"] = QwtSymbol::Ellipse;
        symbolMap["triangle"] = QwtSymbol::Triangle;
        symbolMap["dtriangle"] = QwtSymbol::DTriangle;
        symbolMap["v"] = QwtSymbol::Ellipse;
        symbolMap["utriangle"] = QwtSymbol::UTriangle;
        symbolMap["^"] = QwtSymbol::Ellipse;
        symbolMap["ltriangle"] = QwtSymbol::LTriangle;
        symbolMap["<"] = QwtSymbol::Ellipse;
        symbolMap["rtriangle"] = QwtSymbol::RTriangle;
        symbolMap[">"] = QwtSymbol::Ellipse;
        symbolMap["cross"] = QwtSymbol::Cross;
        symbolMap["+"] = QwtSymbol::Cross;
        symbolMap["x"] = QwtSymbol::XCross;
        symbolMap["xcross"] = QwtSymbol::XCross;
        symbolMap["hline"] = QwtSymbol::HLine;
        symbolMap["-"] = QwtSymbol::HLine;
        symbolMap["vline"] = QwtSymbol::VLine;
        symbolMap["|"] = QwtSymbol::VLine;
        symbolMap["star1"] = QwtSymbol::Star1;
        symbolMap["*"] = QwtSymbol::Star1;
        symbolMap["star2"] = QwtSymbol::Star2;
        symbolMap["hexagon"] = QwtSymbol::Hexagon;
    }

    std::transform(symbol.begin(), 
                   symbol.end(),
                   symbol.begin(),
                   (int(*)(int))std::tolower);
    
    map<std::string, QwtSymbol::Style>::iterator st = symbolMap.find(symbol);
    QwtSymbol symbol_;
    
    if ( st != symbolMap.end() )
    {
        symbol_.setStyle(st->second);
    }
    else
    {
        cerr << "Plot::setPlotSymbol(std::string symbol) - " << symbol << " is not a valid symbol." << endl;
    }
    setSymbol(symbol_);
    
}

void Plot::_setPlotSymbol(const Conn & conn, std::string symbol)
{
    (static_cast< Plot* > ( conn.targetElement()->data() ))->setPlotSymbol(symbol);
    
}

const std::string Plot::getPlotSymbol()
{
    switch ( symbol().style())
    {
        case ( QwtSymbol::NoSymbol ): return "NoSymbol";
        case ( QwtSymbol::Ellipse ): return "Ellipse";
        case ( QwtSymbol::Rect): return "Rect";            
        case ( QwtSymbol::Diamond): return  "Diamond";            
        case ( QwtSymbol::Triangle): return  "Triangle";            
        case ( QwtSymbol::DTriangle): return "DTriangle";            
        case ( QwtSymbol::UTriangle): return  "UTriangle";            
        case ( QwtSymbol::LTriangle): return  "LTriangle";            
        case ( QwtSymbol::RTriangle): return  "RTriangle";            
        case ( QwtSymbol::Cross): return  "Cross";            
        case ( QwtSymbol::XCross): return  "XCross";            
        case ( QwtSymbol::HLine): return  "HLine";            
        case ( QwtSymbol::VLine): return  "VLine";            
        case ( QwtSymbol::Star1): return  "Star1";            
        case ( QwtSymbol::Star2): return  "Star2";            
        case ( QwtSymbol::Hexagon): return  "Hexagon";
        default: cerr << "Error:  Plot::getPlotSymbol() - unknown symbol " << endl;            
    }    
}

const std::string Plot::_getPlotSymbol(const Conn & conn)
{
    return (static_cast< Plot* > ( conn.targetElement()->data() ))->getPlotSymbol();
    
}


const std::string Plot::getColor(void)
{
    
    return pen().color().name().toStdString();
}

const std::string Plot::_getColor(const Conn& conn)
{
    return std::string((static_cast< Plot* > ( conn.targetElement()->data() ))->pen().color().name().toStdString());
}

void Plot::setColor(std::string color)
{
    setPen(QPen(QColor(color.c_str()), getPlotWidth()));    
}

void Plot::_setColor(const Conn& conn, std::string color)
{
    Plot *p = (static_cast< Plot* > ( conn.targetElement()->data() ));
    p->setColor(color);    
}

void Plot::setSymbolFillColor( std::string fillcolor ) 
{
    QwtSymbol sym = symbol();
    sym.setBrush(QBrush(QColor(fillcolor.c_str())));
    setSymbol(sym);    
}

void Plot::_setSymbolFillColor( const Conn & conn, std::string fillcolor )
{
    Plot *p = (static_cast< Plot* > ( conn.targetElement()->data() ));
    p->setSymbolFillColor(fillcolor);    
}

void Plot::setSymbolSize(int w, int h )
{
    QwtSymbol sym = symbol();
    sym.setSize(w,h);
    setSymbol(sym);    
}

void Plot::_setSymbolSize(const Conn & conn, int w, int h)
{
    Plot *p = (static_cast< Plot* > ( conn.targetElement()->data() ));
    p->setSymbolSize(w,h);    
}

void Plot::setSymbolBorderColor(std::string bordercolor )
{
    QwtSymbol sym = symbol();
    sym.setPen( QPen(QColor(bordercolor.c_str())) );
    setSymbol(sym);    
}

void Plot::_setSymbolBorderColor(const Conn& conn, std::string bordercolor )
{
    Plot *p = (static_cast< Plot* > ( conn.targetElement()->data() ));
    p->setSymbolBorderColor(bordercolor);    
}

void Plot::setSymbolBorderWidth(int border_width )
{
    QwtSymbol sym = symbol();
    QPen pen = sym.pen();
    pen.setWidth(border_width);
    sym.setPen(pen);    
}

void Plot::_setSymbolBorderWidth(const Conn & conn, int border_width)
{
    Plot *p = (static_cast< Plot* > ( conn.targetElement()->data() ));
    p->setSymbolBorderWidth(border_width);    
}

int Plot::getPlotWidth( void )
{
    return pen().width();    
}

int Plot::_getPlotWidth( const Conn& conn )
{
    Plot *p = (static_cast< Plot* > ( conn.targetElement()->data() ));
    return p->getPlotWidth();    
}

void Plot::setPlotWidth( int width)
{
    QPen pen_ = pen();
    pen_.setWidth(width);
    setPen(pen_);    
}

void Plot::_setPlotWidth( const Conn& conn, int width)
{
    Plot *p = (static_cast< Plot* > ( conn.targetElement()->data() ));
    p->setPlotWidth(width);
}


#endif
