// LeakyIaF.h --- 
// 
// Filename: LeakyIaF.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Created: Thu Jul  7 12:16:08 2011 (+0530)
// Version: 
// Last-Updated: Thu Jul  7 14:07:02 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 16
// URL: 
// Keywords: 
// Compatibility: 
// 
// 

// Commentary: 
// 
// 
// 
// 

// Change log:
// 
// 
// 

// Code:

#ifndef _LEAKYIAF_H
#define _LEAKYIAF_H

class LeakyIaF
{
  public:
    LeakyIaF();
    ~LeakyIaF();
    
    void setRm(double Rm);
    double getRm() const;

    void setCm(double Cm);
    double getCm() const;    

    void setEm(double Em);
    double getEm() const;

    void setInitVm(double value);
    double getInitVm() const;
    
    void setVm(double value);
    double getVm() const;

    void setVreset(double value);
    double getVreset() const;
    
    void setVthreshold(double value);
    double getVthreshold() const;

    void setRefractoryPeriod(double value);
    double getRefractoryPeriod() const;

    void setTspike(double value);
    double getTspike() const;

    void handleInject(double value);
    
    void process(const Eref& e, ProcPtr p);
    void reinit(const Eref& e, ProcPtr p);

    static const Cinfo * initCinfo();
  protected:
    double Rm_;
    double Cm_;
    double Em_;
    double initVm_;
    double Vm_;
    double Vreset_;
    double Vthreshold_;
    double refractoryPeriod_;
    double tSpike_;
    double sumInject_;
}; //! class LeakyIaF

#endif



// 
// LeakyIaF.h ends here
