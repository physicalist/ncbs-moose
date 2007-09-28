/*******************************************************************
 * File:            Plot.h
 * Description:      This defines the data structure to hold a plot.
 * Author:          Subhasis Ray
 * E-mail:          subhasis@ncbs.res.in
 * Created:         2007-04-19 13:00:22
 ********************************************************************/
#ifndef _PLOT_H
#define _PLOT_H
#include <iostream>
#include <cstdlib>
#include <string>
#include "MooseQt.h"
using namespace Qt;
class Plot: public QwtPlotCurve
{
  public:
    Plot();
    Plot(const QwtText & title);
    Plot(const QString & title);
    Plot(const Plot& p)
    {
    }
    Plot& operator=(const Plot& p)
    {
        Plot *p1 = new Plot();
        return *p1;
        
    }
    
//    ~Plot();
    
    int getCapacity(void);
    static int _getCapacity(const Conn& conn);
    void setCapacity(int capacity);
    static void _setCapacity(const Conn& conn, int capacity);
    int getCapacityIncr(void);
    static int _getCapacityIncr(const Conn & conn);
    void setCapacityIncr(int capIncr);
    static void _setCapacityIncr(const Conn & conn, int capIncr);
    int getSize(void);
    static int _getSize(const Conn & conn);
    double * getX(void);
    static double * _getX(const Conn & conn);
    double * getY(void);
    static double * _getY(const Conn & conn);
    
    void appendX(double x);
    static void _appendX(const Conn& conn, double x);
    void appendY(double y);
    static void _appendY(const Conn& conn, double y);
    void appendXY(double x, double y);
    static void _appendXY(const Conn& conn, double x, double y);
    bool getYfx(void);
    static bool _getYfx(const Conn& conn);
    void setYfx(bool set = true);
    static void _setYfx(const Conn& conn, bool set = true);
    const std::string getColor(void);
    static const std::string _getColor(const Conn& conn);
    void setColor(std::string color);
    static void _setColor(const Conn& conn, std::string color);
    const std::string getPlotStyle(void);
    static const std::string _getPlotStyle(const Conn& conn);
    void setPlotStyle(std::string style);
    static void _setPlotStyle(const Conn& conn, std::string style);
    const std::string getPlotSymbol( void );
    static const std::string _getPlotSymbol( const Conn& conn );
    void setPlotSymbol( std::string symbol =".");
    static void _setPlotSymbol( const Conn & conn, std::string symbol =".");
    void setSymbolFillColor( std::string fillcolor = "red" );
    static void _setSymbolFillColor( const Conn & conn, std::string fillcolor = "red" );
    void setSymbolSize(int w=3, int h = -1);
    static void _setSymbolSize(const Conn & conn, int w=3, int h = -1);
    void setSymbolBorderColor(std::string bordercolor = "red");
    static void _setSymbolBorderColor(const Conn& conn, std::string bordercolor = "red");
    void setSymbolBorderWidth(int border_width = 0);
    static void _setSymbolBorderWidth(const Conn & conn, int border_width = 0);
    int getPlotWidth( void );
    static int _getPlotWidth( const Conn& conn );
    void setPlotWidth( int width);
    static void _setPlotWidth( const Conn& conn, int width);
    void setData(double *x, double * y, int size);
    void setData( QwtArray<double> & x, QwtArray <double> & y );
    void setRawData(double *x, double *y, int size);
    
  private:
    double new_x_;
    double new_y_;
    int size_;
    int capacity_;
    int capacity_incr_;    
    int x_y_;
    double * x_;
    double * y_;
    bool myAllocated_;
    
    static const Cinfo * plotCinfo_;
    
};


#endif
