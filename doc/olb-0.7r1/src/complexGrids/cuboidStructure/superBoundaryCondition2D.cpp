/*  This file is part of the OpenLB library
 *
 *  Copyright (C) 2007 Mathias J. Krause
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

/** \file
 * A helper for initialising 2D boundaries -- template instantiation.
 */


#include "complexGrids/mpiManager/mpiManager.h"
#include "superBoundaryCondition2D.h"
#include "superBoundaryCondition2D.hh"
#include "core/latticeDescriptors.h"
#include "core/latticeDescriptors.hh"
#include "core/dynamics.h"
#include "core/dynamics.hh"

namespace olb {

template class sOnLatticeBoundaryCondition2D
<double, descriptors::D2Q9Descriptor>;

template void createLocalBoundaryCondition2D
<double,descriptors::D2Q9Descriptor,
BGKdynamics<double,descriptors::D2Q9Descriptor> >
(sOnLatticeBoundaryCondition2D<double,descriptors::D2Q9Descriptor>& sBC);
template void createLocalBoundaryCondition2D
<double,descriptors::D2Q9Descriptor>
(sOnLatticeBoundaryCondition2D<double,descriptors::D2Q9Descriptor>& sBC);

template void createInterpBoundaryCondition2D
<double,descriptors::D2Q9Descriptor,
RLBdynamics<double,descriptors::D2Q9Descriptor> >
(sOnLatticeBoundaryCondition2D<double,descriptors::D2Q9Descriptor>& sBC);
template void createInterpBoundaryCondition2D
<double,descriptors::D2Q9Descriptor>
(sOnLatticeBoundaryCondition2D<double,descriptors::D2Q9Descriptor>& sBC);
}
