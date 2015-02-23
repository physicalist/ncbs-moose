/* DifBuffer.h --- 
 * 
 * Filename: DifBuffer.h
 * Description: 
 * Author: Subhasis Ray
 * Maintainer: 
 * Created: Mon Feb 16 11:42:50 2015 (-0500)
 * Version: 
 * Package-Requires: ()
 * Last-Updated: Mon Feb 23 10:49:54 2015 (-0500)
 *           By: Subhasis Ray
 *     Update #: 25
 * URL: 
 * Doc URL: 
 * Keywords: 
 * Compatibility: 
 * 
 */

/* Commentary: 
 * 
 * 
 * 
 */

/* Change Log:
 * 
 * 
 */

/* This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Code: */

#ifndef _DifBuffer_h
#define _DifBuffer_h

class DifBuffer
{
 public:
  DifBuffer();

  double getActivation() const;
  void setActivation(double value);
  double getBFree() const;
  double getBBound() const;
  double getPrevFree() const;   // Bfree at previous time step
  double getPrevBound() const;  // Bbound at previous time step
  double getBTot() const;           //  total buffer concentration in mM (free + bound)
  void setBTot(double value);
  double getKf() const;         // forward rate constant in 1/(mM*sec)
  double getKb() const;         // backward rate constant in 1/sec
  double getD() const;          // diffusion constant of buffer molecules, m^2/sec
  int getShapeMode() const; //    Set to one of the predefined global
  void setShapeMode(int value); // variables SHELL=0, SLICE=SLAB=1, USERDEF=3.
  double getLen() const; //             shell length
  void setLen(double value);
  double getDia() const; //            shell diameter
  void setDia(double value);
  double getThick() const; //           shell thickness
  void setThick(double value);
  double getVol() const; //             shell volume
  double getOuterArea() const; //         area of upper (outer) shell surface
  double getInnerArea() const; //       area of lower (inner) shell surface

  void concentration();

  void reinit(const Eref & e, ProcPtr p);
  void init(const Eref & e, ProcPtr p);
  
  static const Cinfo * initCinfo();

 private:
  double activation_;
  double bFree_;
  double bBound_;
  double prevFree_;
  double prevBound_;
  double bTot_;
  double kf_;
  double kb_;
  double D_;
  int shapeMode_;
  double dia_;
  double len_;
  double thick_;
  double outerArea_;
  double innerArea_;
  
};

#endif // _DifBuffer_h
/* DifBuffer.h ends here */
