/*  This file is part of the OpenLB library
 *
 *  Copyright (C) 2006, 2007, 2012 Jonas Latt, Mathias J. Krause
 *  E-mail contact: info@openlb.net
 *  The most recent release of OpenLB can be downloaded at
 *  <http://www.openlb.net/>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this program; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
*/

#include "olb2D.h"
#ifndef OLB_PRECOMPILED // Unless precompiled version is used,
#include "olb2D.hh"   // include full template code
#endif
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>

using namespace olb;
using namespace olb::descriptors;
using namespace olb::graphics;
using namespace std;

typedef double T;
#define DESCRIPTOR D2Q9Descriptor

T poiseuilleVelocity(int iY, int N, T u) {
  T y = (T)iY / (T)N;
  return 4.*u * (y-y*y);
}

void iniGeometry( BlockStructure2D<T,DESCRIPTOR>& lattice,
                  LBconverter<T> const& converter,
                  Dynamics<T, DESCRIPTOR>& bulkDynamics,
                  OnLatticeBoundaryCondition2D<T,DESCRIPTOR>& boundaryCondition )
{
  const T lx1   = 5.0;
  const T ly1   = 0.75;
  const T omega = converter.getOmega();

  const int nx = lattice.getNx();
  const int ny = lattice.getNy();
  const int nx1 = converter.numCells(lx1);
  const int ny1 = converter.numCells(ly1);

  lattice.defineDynamics(0,nx-1,0,ny-1,   &bulkDynamics);
  lattice.defineDynamics(0,nx1-1,0,ny1-1, &instances::getNoDynamics<T,DESCRIPTOR>());

  boundaryCondition.addVelocityBoundary0N(0,0,        ny1+1,ny-2, omega);
  boundaryCondition.addVelocityBoundary0N(nx1,nx1,    1,ny1-1, omega);
  boundaryCondition.addVelocityBoundary0P(nx-1,nx-1,  1, ny-2, omega);
  boundaryCondition.addVelocityBoundary1P(1,nx-2,     ny-1,ny-1, omega);
  boundaryCondition.addVelocityBoundary1N(1,nx1-1,    ny1,ny1, omega);
  boundaryCondition.addVelocityBoundary1N(nx1+1,nx-2, 0,0, omega);

  boundaryCondition.addExternalVelocityCornerNN(0,ny1, omega);
  boundaryCondition.addExternalVelocityCornerNN(nx1,0, omega);

  boundaryCondition.addExternalVelocityCornerNP(0,ny-1, omega);
  boundaryCondition.addExternalVelocityCornerPN(nx-1,0, omega);
  boundaryCondition.addExternalVelocityCornerPP(nx-1,ny-1, omega);

  boundaryCondition.addInternalVelocityCornerNN(nx1,ny1, omega);

  for (int iX=0; iX<=nx1; ++iX) {
    for (int iY=ny1; iY<ny; ++iY) {
      T vel[] = {
        poiseuilleVelocity(iY-ny1, ny-ny1-1, converter.getLatticeU()),
        T()
      };
      lattice.get(iX,iY).defineRhoU((T)1, vel);
      lattice.get(iX,iY).iniEquilibrium((T)1, vel);
    }
  }

  for (int iX=nx1+1; iX<nx; ++iX) {
    for (int iY=0; iY<ny; ++iY) {
      T vel[] = {
        poiseuilleVelocity( iY, ny-1,
        converter.getLatticeU()*((T)1-(T)ny1/(T)ny) ),
        T()
      };
      lattice.get(iX,iY).defineRhoU((T)1, vel);
      lattice.get(iX,iY).iniEquilibrium((T)1, vel);
    }
  }

  lattice.initialize();
}

void writeGifs(BlockStructure2D<T,DESCRIPTOR>& lattice,
               LBconverter<T> const& converter, int iter)
{
  const int imSize = 600;
  DataAnalysisBase2D<T,DESCRIPTOR> const& analysis = lattice.getDataAnalysis();
  ImageWriter<T> imageWriter("leeloo");
  imageWriter.writeScaledGif(createFileName("u5_", iter, 6),
                             analysis.getVelocityNorm(), imSize,imSize);
  imageWriter.writeScaledGif(createFileName("omega", iter, 6),
                             analysis.getVorticity(), imSize,imSize);
}

int main(int argc, char* argv[]) {
  olbInit(&argc, &argv);
  singleton::directories().setOutputDir("./tmp/");

  OstreamManager clout(std::cout, "main");

  LBconverter<T> converter(
    (int) 2,                               // dim
    (T)   1./60.,                          // latticeL_
    (T)   2e-2,                            // latticeU_
    (T)   1./500.,                         // charNu_
    (T)   1.                               // charL_ = 1,
  );
  writeLogFile(converter, "backwardFacingStep2d");

  const T maxT        = (T)10.;
  const int  iterStat = 100;
  const int  iterGif  = 2000;
  const int  iterSave = 10000;

#ifndef PARALLEL_MODE_MPI  // sequential program execution
  BlockLattice2D<T, DESCRIPTOR> lattice(converter.numNodes(20), converter.numNodes(1.5) );
#else                      // parallel program execution
  MultiBlockLattice2D<T, DESCRIPTOR> lattice (
    createRegularDataDistribution( converter.numNodes(20), converter.numNodes(1.5) ) );
#endif

  ConstRhoBGKdynamics<T, DESCRIPTOR> bulkDynamics (
    converter.getOmega(),
    instances::getBulkMomenta<T,DESCRIPTOR>()
  );

  // choose between local and non-local boundary condition
  OnLatticeBoundaryCondition2D<T,DESCRIPTOR>*
  // boundaryCondition = createInterpBoundaryCondition2D(lattice);
  boundaryCondition = createLocalBoundaryCondition2D(lattice);

  iniGeometry(lattice, converter, bulkDynamics, *boundaryCondition);

  for (int iT=0; iT < converter.numTimeSteps(maxT); ++iT) {
    if (iT%iterStat==0 && iT>0) {
      lattice.getStatistics().print(iT,converter.physTime(iT) );
    }
    if (iT%iterGif==0 && iT>0) {
      writeGifs(lattice, converter, iT);
    }
    if (iT%iterSave==0 && iT>0) {
      clout << "Checkpointing the system at t=" << iT << endl;
      saveData(lattice, "bstep2d.checkpoint");
      // The data can be reloaded using
      //     loadData(lattice, "bstep2d.checkpoint");
    }

    lattice.collideAndStream();
  }

  delete boundaryCondition;
}
