/*  This file is part of the OpenLB library
 *
 *  Copyright (C) 2007, 2008 Bernd Stahl
 *  Address: Battelle Batiment A, Route de Drize 7, 1227 Carouge, Switzerland
 *  E-mail: bernd.stahl@cui.unige.ch
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
 * Utilities for 3D multi data distributions -- header file.
 */

#ifndef MULTI_DATA_UTILITIES_3D_H
#define MULTI_DATA_UTILITIES_3D_H

#include "core/dataFields3D.h"
#include "complexGrids/multiBlockStructure/multiDataGeometry3D.h"

namespace olb {

/// A 3D field of scalar values used to indicate the type of the cells.
/// Any positive value indicates an active (bulk, boundary) cell,
/// while zero indicates a non-active (no-dynamics) cell
typedef ScalarField3D<unsigned char> CellTypeField3D;

/// Create a nx-by-ny-by-nz data distribution
MultiDataDistribution3D createRegularDataDistribution (
  int nx, int ny, int nz, int numBlocksX, int numBlocksY, int numBlocksZ,
  int envelopeWidth );

/// Create a data distribution with regular blocks, as evenly distributed as possible
MultiDataDistribution3D createRegularDataDistribution(int nx, int ny, int nz, int envelopeWidth=1);

/// Create a data distribution by slicing the domain (a block of nX*nY*nZ cells
/// as defined by cellTypeField) into numBlocks blocks along the x-direction.
/// The x-extent of the blocks is chosen such as to obtain an approximately
/// equal number of active cells in each block.
MultiDataDistribution3D createXSlicedDataDistribution3D (
  CellTypeField3D const& cellTypeField,
  int numBlocks,
  int envelopeWidth );

/// cf above.
MultiDataDistribution3D createYSlicedDataDistribution3D (
  CellTypeField3D const& cellTypeField,
  int numBlocks,
  int envelopeWidth );

/// cf above.
MultiDataDistribution3D createZSlicedDataDistribution3D (
  CellTypeField3D const& cellTypeField,
  int numBlocks,
  int envelopeWidth );

/// Create x-sliced data distribution, balancing the number of active cells between blocks,
/// implicitly setting numBlocks = #processors
MultiDataDistribution3D createXSlicedDataDistribution3D (
  CellTypeField3D const& cellTypeField, int envelopeWidth=1);

/// cf above
MultiDataDistribution3D createYSlicedDataDistribution3D (
  CellTypeField3D const& cellTypeField, int envelopeWidth=1);

/// cf above
MultiDataDistribution3D createZSlicedDataDistribution3D (
  CellTypeField3D const& cellTypeField, int envelopeWidth=1);

}  // namespace olb


#endif
