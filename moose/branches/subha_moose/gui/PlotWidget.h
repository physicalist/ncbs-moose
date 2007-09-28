/*******************************************************************
 * File:            PlotWidget.h
 * Description:     This is the widget for actually drawing plots.
 * Author:          Subhasis Ray
 * E-mail:          subhasis@ncbs.res.in
 * Created:         2007-04-23 17:07:22
 ********************************************************************/
#ifndef _PLOTWIDGET_H
#define _PLOTWIDGET_H
//#include "Plot.h"
#include <QtGui>
//#include <QwtData>
#include <qwt_data.h>
#include <qwt_plot.h>
#include<qwt_plot_curve.h>
#include <vector>
#include <cmath>
using namespace Qt;
using std::vector;

class PlotWidget:public QwtPlot
{
  public:
    
    // TODO: sample widget - to be enhanced later
    PlotWidget(QWidget * parent = 0):QwtPlot(parent)
    {
        for ( int i = 0; i < 360; ++i)
        {
            double theta = ((double)i)*3.141592/180.0;
            x_.push_back(theta);
            y_.push_back(std::cos(theta));            
        }
        curve_.setData(x_, y_);
        curve_.attach(this);        
    }
    
    PlotWidget(const QwtText& title, QWidget * parent = 0):QwtPlot( title, parent )
    {
        for ( int i = 0; i < 360; ++i)
        {
            double theta = ((double)i)*3.141592/180.0;
            x_.push_back(theta);
            y_.push_back(std::sin(theta));            
        }
        curve_.setData(x_, y_);
        curve_.attach(this);
    }
  private:
    QwtArray <double> x_;
    QwtArray <double> y_;
    QwtPlotCurve curve_;
    
    
    
};



#endif // _PLOTWIDGET_H!
