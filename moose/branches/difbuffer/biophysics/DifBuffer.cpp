// DifBuffer.cpp --- 
// 
// Filename: DifBuffer.cpp
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Created: Mon Feb 16 12:02:11 2015 (-0500)
// Version: 
// Package-Requires: ()
// Last-Updated: Mon Feb 16 14:03:00 2015 (-0500)
//           By: Subhasis Ray
//     Update #: 85
// URL: 
// Doc URL: 
// Keywords: 
// Compatibility: 
// 
// 

// Commentary: 
// 
// 
// 
// 

// Change Log:
// 
// 
// 
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
// 
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
// 
// 

// Code:

static SrcFinfo4< double, double, double, double >* reactionOut()
{
  static SrcFinfo4< double, double, double, double > reactionOut(
      "reactionOut",
      "Sends out reaction rates (forward and backward), and concentrations"
      " (free-buffer and bound-buffer molecules).");
  return &reactionOut;
}
                                                                 

const Cinfo * DifBuffer::initCinfo()
{
  static DestFinfo process( "process",
                            "Handles process call",
                            new ProcOpFunc< DifBuffer >(  &DifBuffer::process_0 ) );
  static DestFinfo process( "process",
                            "Handles process call",
                            new ProcOpFunc< DifBuffer >(  &DifBuffer::process_0 ) );
  static DestFinfo reinit( "reinit",
                           "Reinit happens only in stage 0",
                           new ProcOpFunc< DifBuffer >( &DifBuffer::reinit_0 ));
    
  static Finfo* processShared_0[] = {
    &process,
    &reinit
  };

  static SharedFinfo process_0(
      "process_0", 
      "Here we create 2 shared finfos to attach with the Ticks. This is because we want to perform DifBuffer "
      "computations in 2 stages, much as in the Compartment object. "
      "In the first stage we send out the concentration value to other DifBuffers and Buffer elements. We also "
      "receive fluxes and currents and sum them up to compute ( dC / dt ). "
      "In the second stage we find the new C value using an explicit integration method. "
      "This 2-stage procedure eliminates the need to store and send prev_C values, as was common in GENESIS.",
      processShared_0,
      sizeof( processShared_0 ) / sizeof( Finfo* ));
	
  static DestFinfo process1( "process",
                             "Handle process call",
                             new ProcOpFunc< DifBuffer >( &DifBuffer::process_1 ) );
  static DestFinfo reinit1( "reinit", 
                            "Reinit happens only in stage 0",
                            new ProcOpFunc< DifBuffer >( &DifBuffer::reinit_1)
                            );
  static Finfo* processShared_1[] = {
    &process1, &reinit1        
  };
    
  static SharedFinfo process_1( "process_1",
                                "Second process call",
                                processShared_1,
                                sizeof( processShared_1 ) / sizeof( Finfo* ) );

  //// Diffusion related shared messages
  static DestFinfo concentration("concentration",
                                 "Receives concentration (from DifShell).",
                                 new OpFunc1<DifBuffer, double>(&DifBuffer::concentration));
  
  static Finfo* bufferShared[] = {
    &concentration, reactionOut()
  };
  static SharedFinfo buffer( "buffer",
                             "This is a shared message from a DifBuffer to a Buffer (FixBuffer or DifBuffer). "
                             "During stage 0:\n "
                             " - DifBuffer sends ion concentration\n"
                             " - Buffer updates buffer concentration and sends it back immediately using a call-back.\n"
                             " - DifBuffer updates the time-derivative ( dC / dt ) \n"
                             "\n"
                             "During stage 1: \n"
                             " - DifBuffer advances concentration C \n\n"
                             "This scheme means that the Buffer does not need to be scheduled, and it does its computations when "
                             "it receives a cue from the DifBuffer. May not be the best idea, but it saves us from doing the above "
                             "computations in 3 stages instead of 2." ,
                             bufferShared,
                             sizeof( bufferShared ) / sizeof( Finfo* ));

  static DestFinfo fluxFromOut( "fluxFromOut",
                                "Destination message",
                                new OpFunc2< DifBuffer, double, double > ( &DifBuffer::fluxFromOut ));
    
  static Finfo* innerDifShared[] = {
    innerDifSourceOut(),
    &fluxFromOut
  };
    
  static SharedFinfo innerDif( "innerDif",
                               "This shared message (and the next) is between DifBuffers: adjoining shells exchange information to "
                               "find out the flux between them. "
                               "Using this message, an inner shell sends to, and receives from its outer shell." ,
                               innerDifShared,
                               sizeof( innerDifShared ) / sizeof( Finfo* ));

  static DestFinfo fluxFromIn( "fluxFromIn", "",
                               new OpFunc2< DifBuffer, double, double> ( &DifBuffer::fluxFromIn ) );
  static Finfo* outerDifShared[] = {
    &fluxFromIn,
    outerDifSourceOut(),
  };

  static  SharedFinfo outerDif( "outerDif",
                                "Using this message, an outer shell sends to, and receives from its inner shell." ,
                                outerDifShared,
                                sizeof( outerDifShared ) / sizeof( Finfo* ));
  
  ////////////////////////////
  // Field defs
  ////////////////////////////
  static ValueFinfo<DifBuffer, double> activation("activation",
                                                  "Ion concentration from incoming conc message.",
                                                  &DifBuffer::setActivation,
                                                  &DifBuffer::getActivation);
  static ValueFinfo<DifBuffer, double> D("D",
                                         "Diffusion constant of buffer molecules. m^2/s",
                                         &DifBuffer::setD,
                                         &DifBuffer::getD);
  static ReadOnlyValueFinfo<DifBuffer, double> bFree("bFree",
                                                     "Free buffer concentration",
                                                     &DifBuffer::getBFree);
  static ReadOnlyValueFinfo<DifBuffer, double> bBound("bBound",
                                                      "Bound buffer concentration",
                                                      &DifBuffer::getBBound);
  // static ReadOnlyValueFinfo<DifBuffer, double> prevFree("prevFree",
  //                                                    "Free buffer concentration in previous step",
  //                                                    &DifBuffer::getPrevFree);
  // static ReadOnlyValueFinfo<DifBuffer, double> prevBound("prevBound",
  //                                                    "Bound buffer concentration in previous step",
  //                                                    &DifBuffer::getPrevBound);
  static ValueFinfo<DifBuffer, double> bTot("bTot",
                                            "Total buffer concentration.",
                                            &DifBuffer::setBTot,
                                            &DifBuffer::getBTot);  
  static ValueFinfo<DifBuffer, double> length("length",
                                              "Length of shell",
                                              &DifBuffer::setLen,
                                              &DifBuffer::getLen);
  static ValueFinfo<DifBuffer, double> diameter("diameter",
                                                "Diameter of shell",
                                                &DifBuffer::setDia,
                                                &DifBuffer::getDia);
  static ValueFinfo<DifBuffer, int> shapeMode("shapeMode",
                                              "shape of the shell: SHELL=0, SLICE=SLAB=1, USERDEF=3",
                                              &DifBuffer::setShapeMode,
                                              &DifBuffer::getShapeMode);
  
  static ValueFinfo<DifBuffer, double> thick("thick",
                                             "Thickness of shell",
                                             &DifBuffer::setThick,
                                             &DifBuffer::getThick);
  static ValueFinfo<DifBuffer, double> thick("thick",
                                             "Thickness of shell",
                                             &DifBuffer::setThick,
                                             &DifBuffer::getThick);
  static ValueFinfo<DifBuffer, double> innerArea("innerArea",
                                                 "Inner area of shell",
                                                 &DifBuffer::setInnerArea,
                                                 &DifBuffer::getInnerArea);
  static ValueFinfo<DifBuffer, double> outerArea("outerArea",
                                                 "Outer area of shell",
                                                 &DifBuffer::setOuterArea,
                                                 &DifBuffer::getOuterArea);
  
  ////
  // DestFinfo
  ////
  static Finfo * difBufferFinfos[] = {
    &activation,
    &D,
    &bFree,
    &bBound,
    &bTot,
    // &prevFree,
    // &prevBound,
    &length,
    &diameter,
    &shapeMode,
    &thick,
    &innerArea,
    &outerArea,
    &process,
    &reinit,
    &innerDif,
    &outerDif,
    &concentration,    
  };

  static string doc[] = {
    "Name", "DifBuffer",
    "Author", "Subhasis Ray (ported from GENESIS2)",
    "Description", "Models diffusible buffer where total concentration is constant. It is"
    " coupled with a DifShell.",
  };
  static Dinfo<DifBuffer> dinfo;
  static Cinfo difBufferCinfo(
      "DifBuffer",
      Neutral::initCinfo(),
      difBufferFinfos,
      sizeof(difBufferFinfos)/sizeof(Finfo*),
      &dinfo,
      doc,
      sizeof(doc)/sizeof(string));

  return &difBufferCinfo;
}

static const Cinfo * difBufferCinfo = DifBuffer::initCinfo();


////////////////////////////////////////////////////////////////////////////////
// Class functions
////////////////////////////////////////////////////////////////////////////////

DifBuffer::DifBuffer()
{ ; }

////////////////////////////////////////////////////////////////////////////////
// Field access functions
////////////////////////////////////////////////////////////////////////////////




// 
// DifBuffer.cpp ends here
