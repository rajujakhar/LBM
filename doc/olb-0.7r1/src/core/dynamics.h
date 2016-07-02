/*  This file is part of the OpenLB library
 *
 *  Copyright (C) 2006, 2007 Jonas Latt
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
 * A collection of dynamics classes (e.g. BGK) with which a Cell object
 * can be instantiated -- header file.
 */
#ifndef LB_DYNAMICS_H
#define LB_DYNAMICS_H

#include "latticeDescriptors.h"
#include "util.h"
#include "postProcessing.h"

namespace olb {

namespace dynamicParams {
// Use 0-99 for relaxation parameters
const int omega_shear = 0;
const int omega_bulk  = 1;

// Use 100-199 for material constants
const int sqrSpeedOfSound = 100; // Speed of sound squared
const int sqrInvSpeedOfSound = 101; // Inverse speed of sound squared

// Use 1000 and higher for custom user-defined constants
}

template<typename T, template<typename U> class Lattice> class Cell;

/// Interface for the dynamics classes
template<typename T, template<typename U> class Lattice>
struct Dynamics {
  /// Destructor: virtual to enable inheritance
  virtual ~Dynamics() { }
  /// Clone the object on its dynamic type.
  virtual Dynamics<T,Lattice>* clone() const =0;
  /// Implementation of the collision step
  virtual void collide(Cell<T,Lattice>& cell,
                       LatticeStatistics<T>& statistics_) =0;
  /// Collide with fixed velocity
  virtual void staticCollide(Cell<T,Lattice>& cell,
                             const T u[Lattice<T>::d],
                             LatticeStatistics<T>& statistics_) =0;
  /// Compute equilibrium distribution function
  virtual T computeEquilibrium(int iPop, T rho, const T u[Lattice<T>::d], T uSqr) const =0;
  /// Initialize cell at equilibrium distribution
  virtual void iniEquilibrium(Cell<T,Lattice>& cell, T rho, const T u[Lattice<T>::d]);
  /// Compute particle density on the cell.
  /** \return particle density
   */
  virtual T computeRho(Cell<T,Lattice> const& cell) const =0;
  /// Compute fluid velocity on the cell.
  /** \param u fluid velocity
   */
  virtual void computeU( Cell<T,Lattice> const& cell,
                         T u[Lattice<T>::d] ) const =0;
  /// Compute fluid momentum (j=rho*u) on the cell.
  /** \param j fluid momentum
   */
  virtual void computeJ( Cell<T,Lattice> const& cell,
                         T j[Lattice<T>::d] ) const =0;
  /// Compute components of the stress tensor on the cell.
  /** \param pi stress tensor */
  virtual void computeStress (
    Cell<T,Lattice> const& cell,
    T rho, const T u[Lattice<T>::d],
    T pi[util::TensorVal<Lattice<T> >::n] ) const =0;
  /// Compute fluid velocity and particle density on the cell.
  /** \param rho particle density
   *  \param u fluid velocity
   */
  virtual void computeRhoU (
    Cell<T,Lattice> const& cell,
    T& rho, T u[Lattice<T>::d]) const =0;
  /// Compute all momenta on the cell, up to second order.
  /** \param rho particle density
   *  \param u fluid velocity
   *  \param pi stress tensor
   */
  virtual void computeAllMomenta (
    Cell<T,Lattice> const& cell,
    T& rho, T u[Lattice<T>::d],
    T pi[util::TensorVal<Lattice<T> >::n] ) const =0;
  /// Access particle populations through the dynamics object.
  /** Default implementation: access cell directly.
   */
  virtual void computePopulations(Cell<T,Lattice> const& cell, T* f) const;
  /// Access external fields through the dynamics object.
  /** Default implementation: access cell directly.
   */
  virtual void computeExternalField (
    Cell<T,Lattice> const& cell, int pos, int size, T* ext ) const;
  /// Set particle density on the cell.
  /** \param rho particle density
   */
  virtual void defineRho(Cell<T,Lattice>& cell, T rho) =0;
  /// Set fluid velocity on the cell.
  /** \param u fluid velocity
   */
  virtual void defineU(Cell<T,Lattice>& cell,
                       const T u[Lattice<T>::d]) =0;
  /// Define fluid velocity and particle density on the cell.
  /** \param rho particle density
   *  \param u fluid velocity
   */
  virtual void defineRhoU (
    Cell<T,Lattice>& cell,
    T rho, const T u[Lattice<T>::d]) =0;
  /// Define all momenta on the cell, up to second order.
  /** \param rho particle density
   *  \param u fluid velocity
   *  \param pi stress tensor
   */
  virtual void defineAllMomenta (
    Cell<T,Lattice>& cell,
    T rho, const T u[Lattice<T>::d],
    const T pi[util::TensorVal<Lattice<T> >::n] ) =0;
  /// Define particle populations through the dynamics object.
  /** Default implementation: access cell directly.
   */
  virtual void definePopulations(Cell<T,Lattice>& cell, const T* f);
  /// Define external fields through the dynamics object.
  /** Default implementation: access cell directly.
   */
  virtual void defineExternalField (
    Cell<T,Lattice>& cell, int pos, int size, const T* ext);
  /// Get local relaxation parameter of the dynamics
  virtual T getOmega() const =0;
  /// Set local relaxation parameter of the dynamics
  virtual void setOmega(T omega_) =0;
  /// Get local value of any parameter
  virtual T getParameter(int whichParameter) const;
  /// Set local value of any parameter
  virtual void setParameter(int whichParameter, T value);
};

/// Interface for classes that compute velocity momenta
/** This class is useful for example to distinguish between bulk and
 * boundary nodes, given that on the boundaries, a particular strategy
 * must be applied to compute the velocity momenta.
 */
template<typename T, template<typename U> class Lattice>
struct Momenta {
  /// Destructor: virtual to enable inheritance
  virtual ~Momenta() { }
  /// Compute particle density on the cell.
  virtual T computeRho(Cell<T,Lattice> const& cell) const =0;
  /// Compute fluid velocity on the cell.
  virtual void computeU (
    Cell<T,Lattice> const& cell,
    T u[Lattice<T>::d] ) const =0;
  /// Compute fluid momentum on the cell.
  virtual void computeJ (
    Cell<T,Lattice> const& cell,
    T j[Lattice<T>::d] ) const =0;
  /// Compute components of the stress tensor on the cell.
  virtual void computeStress (
    Cell<T,Lattice> const& cell,
    T rho, const T u[Lattice<T>::d],
    T pi[util::TensorVal<Lattice<T> >::n] ) const =0;
  /// Compute fluid velocity and particle density on the cell.
  virtual void computeRhoU (
    Cell<T,Lattice> const& cell,
    T& rho, T u[Lattice<T>::d]) const;
  /// Compute all momenta on the cell, up to second order.
  virtual void computeAllMomenta (
    Cell<T,Lattice> const& cell,
    T& rho, T u[Lattice<T>::d],
    T pi[util::TensorVal<Lattice<T> >::n] ) const;
  /// Set particle density on the cell.
  virtual void defineRho(Cell<T,Lattice>& cell, T rho) =0;
  /// Set fluid velocity on the cell.
  virtual void defineU(Cell<T,Lattice>& cell,
                       const T u[Lattice<T>::d]) =0;
  /// Define fluid velocity and particle density on the cell.
  virtual void defineRhoU (
    Cell<T,Lattice>& cell,
    T rho, const T u[Lattice<T>::d]);
  /// Define all momenta on the cell, up to second order.
  virtual void defineAllMomenta (
    Cell<T,Lattice>& cell,
    T rho, const T u[Lattice<T>::d],
    const T pi[util::TensorVal<Lattice<T> >::n] ) =0;
};

/// Abstract base for dynamics classes
/** In this version of the Dynamics classes, the computation of the
 * velocity momenta is taken care of by an object of type Momenta.
 */
template<typename T, template<typename U> class Lattice>
class BasicDynamics : public Dynamics<T,Lattice> {
public:
  /// Must be contructed with an object of type Momenta
  BasicDynamics(Momenta<T,Lattice>& momenta_);
  /// Implemented via the Momenta object
  virtual T computeRho(Cell<T,Lattice> const& cell) const;
  /// Implemented via the Momenta object
  virtual void computeU (
    Cell<T,Lattice> const& cell,
    T u[Lattice<T>::d] ) const;
  /// Implemented via the Momenta object
  virtual void computeJ (
    Cell<T,Lattice> const& cell,
    T j[Lattice<T>::d] ) const;
  /// Implemented via the Momenta object
  virtual void computeStress (
    Cell<T,Lattice> const& cell,
    T rho, const T u[Lattice<T>::d],
    T pi[util::TensorVal<Lattice<T> >::n] ) const;
  /// Implemented via the Momenta object
  virtual void computeRhoU (
    Cell<T,Lattice> const& cell,
    T& rho, T u[Lattice<T>::d]) const;
  /// Implemented via the Momenta object
  virtual void computeAllMomenta (
    Cell<T,Lattice> const& cell,
    T& rho, T u[Lattice<T>::d],
    T pi[util::TensorVal<Lattice<T> >::n] ) const;
  /// Implemented via the Momenta object
  virtual void defineRho(Cell<T,Lattice>& cell, T rho);
  /// Implemented via the Momenta object
  virtual void defineU(Cell<T,Lattice>& cell,
                       const T u[Lattice<T>::d]);
  /// Implemented via the Momenta object
  virtual void defineRhoU (
    Cell<T,Lattice>& cell,
    T rho, const T u[Lattice<T>::d]);
  /// Implemented via the Momenta object
  virtual void defineAllMomenta (
    Cell<T,Lattice>& cell,
    T rho, const T u[Lattice<T>::d],
    const T pi[util::TensorVal<Lattice<T> >::n] );
protected:
  Momenta<T,Lattice>& momenta;  ///< computation of velocity momenta
};

/// Implementation of the BGK collision step
template<typename T, template<typename U> class Lattice>
class BGKdynamics : public BasicDynamics<T,Lattice> {
public:
  /// Constructor
  BGKdynamics(T omega_, Momenta<T,Lattice>& momenta_);
  /// Clone the object on its dynamic type.
  virtual BGKdynamics<T,Lattice>* clone() const;
  /// Compute equilibrium distribution function
  virtual T computeEquilibrium(int iPop, T rho, const T u[Lattice<T>::d], T uSqr) const;
  /// Collision step
  virtual void collide(Cell<T,Lattice>& cell,
                       LatticeStatistics<T>& statistics_);
  /// Collide with fixed velocity
  virtual void staticCollide(Cell<T,Lattice>& cell,
                             const T u[Lattice<T>::d],
                             LatticeStatistics<T>& statistics_);
  /// Get local relaxation parameter of the dynamics
  virtual T getOmega() const;
  /// Set local relaxation parameter of the dynamics
  virtual void setOmega(T omega_);
private:
  T omega;  ///< relaxation parameter
};

/// Implementation of the pressure-corrected BGK collision step
template<typename T, template<typename U> class Lattice>
class ConstRhoBGKdynamics : public BasicDynamics<T,Lattice> {
public:
  /// Constructor
  ConstRhoBGKdynamics(T omega_, Momenta<T,Lattice>& momenta_);
  /// Clone the object on its dynamic type.
  virtual ConstRhoBGKdynamics<T,Lattice>* clone() const;
  /// Compute equilibrium distribution function
  virtual T computeEquilibrium(int iPop, T rho, const T u[Lattice<T>::d], T uSqr) const;
  /// Collision step
  virtual void collide(Cell<T,Lattice>& cell,
                       LatticeStatistics<T>& statistics_);
  /// Collide with fixed velocity
  virtual void staticCollide(Cell<T,Lattice>& cell,
                             const T u[Lattice<T>::d],
                             LatticeStatistics<T>& statistics_);
  /// Get local relaxation parameter of the dynamics
  virtual T getOmega() const;
  /// Set local relaxation parameter of the dynamics
  virtual void setOmega(T omega_);
private:
  T omega;  ///< relaxation parameter
};

/// Implementation of the so-called incompressible collision step
template<typename T, template<typename U> class Lattice>
class IncBGKdynamics : public BasicDynamics<T,Lattice> {
public:
  /// Constructor
  IncBGKdynamics(T omega_, Momenta<T,Lattice>& momenta_);
  /// Clone the object on its dynamic type.
  virtual IncBGKdynamics<T,Lattice>* clone() const;
  /// Compute equilibrium distribution function
  virtual T computeEquilibrium(int iPop, T rho, const T u[Lattice<T>::d], T uSqr) const;
  /// Collision step
  virtual void collide(Cell<T,Lattice>& cell,
                       LatticeStatistics<T>& statistics_);
  /// Collide with fixed velocity
  virtual void staticCollide(Cell<T,Lattice>& cell,
                             const T u[Lattice<T>::d],
                             LatticeStatistics<T>& statistics_);
  /// Get local relaxation parameter of the dynamics
  virtual T getOmega() const;
  /// Set local relaxation parameter of the dynamics
  virtual void setOmega(T omega_);
private:
  T omega;  ///< relaxation parameter
};



/// Implementation of the Regularized BGK collision step
/** This model is substantially more stable than plain BGK, and has roughly
 * the same efficiency. However, it cuts out the modes at higher Knudsen
 * numbers and can not be used in the regime of rarefied gases.
 */
template<typename T, template<typename U> class Lattice>
class RLBdynamics : public BasicDynamics<T,Lattice> {
public:
  /// Constructor
  RLBdynamics(T omega_, Momenta<T,Lattice>& momenta_);
  /// Clone the object on its dynamic type.
  virtual RLBdynamics<T,Lattice>* clone() const;
  /// Compute equilibrium distribution function
  virtual T computeEquilibrium(int iPop, T rho, const T u[Lattice<T>::d], T uSqr) const;
  /// Collision step
  virtual void collide(Cell<T,Lattice>& cell,
                       LatticeStatistics<T>& statistics_);
  /// Collide with fixed velocity
  virtual void staticCollide(Cell<T,Lattice>& cell,
                             const T u[Lattice<T>::d],
                             LatticeStatistics<T>& statistics_);
  /// Get local relaxation parameter of the dynamics
  virtual T getOmega() const;
  /// Set local relaxation parameter of the dynamics
  virtual void setOmega(T omega_);
private:
  T omega;  ///< relaxation parameter
};

/// Implementation of Regularized BGK collision, followed by any Dynamics
template<typename T, template<typename U> class Lattice, typename Dynamics>
class CombinedRLBdynamics : public BasicDynamics<T,Lattice>
{
public:
  /// Constructor
  CombinedRLBdynamics(T omega_, Momenta<T,Lattice>& momenta_);
  /// Clone the object on its dynamic type.
  virtual CombinedRLBdynamics<T, Lattice, Dynamics>* clone() const;
  /// Compute equilibrium distribution function
  virtual T computeEquilibrium(int iPop, T rho, const T u[Lattice<T>::d], T uSqr) const;
  /// Collision step
  virtual void collide(Cell<T,Lattice>& cell,
                       LatticeStatistics<T>& statistics_);
  /// Collide with fixed velocity
  virtual void staticCollide(Cell<T,Lattice>& cell,
                             const T u[Lattice<T>::d],
                             LatticeStatistics<T>& statistics_);
  /// Get local relaxation parameter of the dynamics
  virtual T getOmega() const;
  /// Set local relaxation parameter of the dynamics
  virtual void setOmega(T omega_);
  /// Get local value of any parameter
  virtual T getParameter(int whichParameter) const;
  /// Set local value of any parameter
  virtual void setParameter(int whichParameter, T value);
private:
  Dynamics boundaryDynamics;
};

/// Implementation of the BGK collision step with external force
template<typename T, template<typename U> class Lattice>
class ForcedBGKdynamics : public BasicDynamics<T,Lattice> {
public:
  /// Constructor
  ForcedBGKdynamics(T omega_, Momenta<T,Lattice>& momenta_);
  /// Clone the object on its dynamic type.
  virtual ForcedBGKdynamics<T,Lattice>* clone() const;
  /// Compute equilibrium distribution function
  virtual T computeEquilibrium(int iPop, T rho, const T u[Lattice<T>::d], T uSqr) const;
  /// Collision step
  virtual void collide(Cell<T,Lattice>& cell,
                       LatticeStatistics<T>& statistics_);
  /// Collide with fixed velocity
  virtual void staticCollide(Cell<T,Lattice>& cell,
                             const T u[Lattice<T>::d],
                             LatticeStatistics<T>& statistics_);
  /// Get local relaxation parameter of the dynamics
  virtual T getOmega() const;
  /// Set local relaxation parameter of the dynamics
  virtual void setOmega(T omega_);
private:
  T omega;  ///< relaxation parameter
  static const int forceBeginsAt = Lattice<T>::ExternalField::forceBeginsAt;
  static const int sizeOfForce   = Lattice<T>::ExternalField::sizeOfForce;
};



/// Implementation of the 3D D3Q13 dynamics
/** This is (so far) the minimal existing 3D model, with only 13
 * directions. Three different relaxation times are used to achieve
 * asymptotic hydrodynamics, isotropy and galilean invariance.
 */
template<typename T, template<typename U> class Lattice>
class D3Q13dynamics : public BasicDynamics<T,Lattice> {
public:
  /// Constructor
  D3Q13dynamics(T omega_, Momenta<T,Lattice>& momenta_);
  /// Clone the object on its dynamic type.
  virtual D3Q13dynamics<T,Lattice>* clone() const;
  /// Compute equilibrium distribution function
  virtual T computeEquilibrium(int iPop, T rho, const T u[Lattice<T>::d], T uSqr) const;
  /// Collision step
  virtual void collide(Cell<T,Lattice>& cell,
                       LatticeStatistics<T>& statistics_);
  /// Collide with fixed velocity
  virtual void staticCollide(Cell<T,Lattice>& cell,
                             const T u[Lattice<T>::d],
                             LatticeStatistics<T>& statistics_);
  /// Get local relaxation parameter of the dynamics
  virtual T getOmega() const;
  /// Set local relaxation parameter of the dynamics
  virtual void setOmega(T omega_);
private:
  T lambda_nu;        ///< first relaxation parameter
  T lambda_nu_prime;  ///< second relaxation parameter
};

/// Standard computation of velocity momenta in the bulk
template<typename T, template<typename U> class Lattice>
struct BulkMomenta : public Momenta<T,Lattice> {
  /// Compute particle density on the cell.
  virtual T computeRho(Cell<T,Lattice> const& cell) const;
  /// Compute fluid velocity on the cell.
  virtual void computeU (
    Cell<T,Lattice> const& cell,
    T u[Lattice<T>::d] ) const;
  /// Compute fluid momentum on the cell.
  virtual void computeJ (
    Cell<T,Lattice> const& cell,
    T j[Lattice<T>::d] ) const;
  /// Compute components of the stress tensor on the cell.
  virtual void computeStress (
    Cell<T,Lattice> const& cell,
    T rho, const T u[Lattice<T>::d],
    T pi[util::TensorVal<Lattice<T> >::n] ) const;
  /// Compute fluid velocity and particle density on the cell.
  virtual void computeRhoU (
    Cell<T,Lattice> const& cell,
    T& rho, T u[Lattice<T>::d]) const;
  /// Compute all momenta on the cell, up to second order.
  virtual void computeAllMomenta (
    Cell<T,Lattice> const& cell,
    T& rho, T u[Lattice<T>::d],
    T pi[util::TensorVal<Lattice<T> >::n] ) const;
  /// Set particle density on the cell.
  virtual void defineRho(Cell<T,Lattice>& cell, T rho);
  /// Set fluid velocity on the cell.
  virtual void defineU(Cell<T,Lattice>& cell,
                       const T u[Lattice<T>::d]);
  /// Define fluid velocity and particle density on the cell.
  virtual void defineRhoU (
    Cell<T,Lattice>& cell,
    T rho, const T u[Lattice<T>::d]);
  /// Define all momenta on the cell, up to second order.
  virtual void defineAllMomenta (
    Cell<T,Lattice>& cell,
    T rho, const T u[Lattice<T>::d],
    const T pi[util::TensorVal<Lattice<T> >::n] );
};

/// Velocity is stored in external scalar (and computed e.g. in a PostProcessor)
template<typename T, template<typename U> class Lattice>
struct ExternalVelocityMomenta : public Momenta<T,Lattice> {
  /// Compute particle density on the cell.
  virtual T computeRho(Cell<T,Lattice> const& cell) const;
  /// Compute fluid velocity on the cell.
  virtual void computeU (
    Cell<T,Lattice> const& cell,
    T u[Lattice<T>::d] ) const;
  /// Compute fluid momentum on the cell.
  virtual void computeJ (
    Cell<T,Lattice> const& cell,
    T j[Lattice<T>::d] ) const;
  /// Compute components of the stress tensor on the cell.
  virtual void computeStress (
    Cell<T,Lattice> const& cell,
    T rho, const T u[Lattice<T>::d],
    T pi[util::TensorVal<Lattice<T> >::n] ) const;
  /// Compute fluid velocity and particle density on the cell.
  virtual void computeRhoU (
    Cell<T,Lattice> const& cell,
    T& rho, T u[Lattice<T>::d]) const;
  /// Compute all momenta on the cell, up to second order.
  virtual void computeAllMomenta (
    Cell<T,Lattice> const& cell,
    T& rho, T u[Lattice<T>::d],
    T pi[util::TensorVal<Lattice<T> >::n] ) const;
  /// Set particle density on the cell.
  virtual void defineRho(Cell<T,Lattice>& cell, T rho);
  /// Set fluid velocity on the cell.
  virtual void defineU(Cell<T,Lattice>& cell,
                       const T u[Lattice<T>::d]);
  /// Define fluid velocity and particle density on the cell.
  virtual void defineRhoU (
    Cell<T,Lattice>& cell,
    T rho, const T u[Lattice<T>::d]);
  /// Define all momenta on the cell, up to second order.
  virtual void defineAllMomenta (
    Cell<T,Lattice>& cell,
    T rho, const T u[Lattice<T>::d],
    const T pi[util::TensorVal<Lattice<T> >::n] );
};

/// Implementation of "bounce-back" dynamics
/** This is a very popular way to implement no-slip boundary conditions,
 * because the dynamics are independent of the orientation of the boundary.
 * It is a special case, because it implements no usual LB dynamics.
 * For that reason, it derives directly from the class Dynamics.
 *
 * The code works for both 2D and 3D lattices.
 */
template<typename T, template<typename U> class Lattice>
class BounceBack : public Dynamics<T,Lattice> {
public:
  /// You may fix a fictitious density value on bounce-back nodes via the constructor.
  BounceBack(T rho_=T());
  /// Clone the object on its dynamic type.
  virtual BounceBack<T,Lattice>* clone() const;
  /// Yields 0;
  virtual T computeEquilibrium(int iPop, T rho, const T u[Lattice<T>::d], T uSqr) const;
  /// Collision step
  virtual void collide(Cell<T,Lattice>& cell,
                       LatticeStatistics<T>& statistics_);
  /// Collide with fixed velocity
  virtual void staticCollide(Cell<T,Lattice>& cell,
                             const T u[Lattice<T>::d],
                             LatticeStatistics<T>& statistics_);
  /// Yields 1;
  virtual T computeRho(Cell<T,Lattice> const& cell) const;
  /// Yields 0;
  virtual void computeU (
    Cell<T,Lattice> const& cell,
    T u[Lattice<T>::d] ) const;
  /// Yields 0;
  virtual void computeJ (
    Cell<T,Lattice> const& cell,
    T j[Lattice<T>::d] ) const;
  /// Yields NaN
  virtual void computeStress (
    Cell<T,Lattice> const& cell,
    T rho, const T u[Lattice<T>::d],
    T pi[util::TensorVal<Lattice<T> >::n] ) const;
  virtual void computeRhoU (
    Cell<T,Lattice> const& cell,
    T& rho, T u[Lattice<T>::d]) const;
  virtual void computeAllMomenta (
    Cell<T,Lattice> const& cell,
    T& rho, T u[Lattice<T>::d],
    T pi[util::TensorVal<Lattice<T> >::n] ) const;
  /// Does nothing
  virtual void defineRho(Cell<T,Lattice>& cell, T rho);
  /// Does nothing
  virtual void defineU(Cell<T,Lattice>& cell,
                       const T u[Lattice<T>::d]);
  /// Does nothing
  virtual void defineRhoU (
    Cell<T,Lattice>& cell,
    T rho, const T u[Lattice<T>::d]);
  /// Does nothing
  virtual void defineAllMomenta (
    Cell<T,Lattice>& cell,
    T rho, const T u[Lattice<T>::d],
    const T pi[util::TensorVal<Lattice<T> >::n] );
  /// Yields NaN
  virtual T getOmega() const;
  /// Does nothing
  virtual void setOmega(T omega_);
private:
  T rho;
};

/// Implementation of a "dead cell" that does nothing
template<typename T, template<typename U> class Lattice>
class NoDynamics : public Dynamics<T,Lattice> {
public:
  /// Clone the object on its dynamic type.
  virtual NoDynamics<T,Lattice>* clone() const;
  /// Yields 0;
  virtual T computeEquilibrium(int iPop, T rho, const T u[Lattice<T>::d], T uSqr) const;
  /// Collision step
  virtual void collide(Cell<T,Lattice>& cell,
                       LatticeStatistics<T>& statistics_);
  /// Collide with fixed velocity
  virtual void staticCollide(Cell<T,Lattice>& cell,
                             const T u[Lattice<T>::d],
                             LatticeStatistics<T>& statistics_);
  /// Yields 1;
  virtual T computeRho(Cell<T,Lattice> const& cell) const;
  /// Yields 0;
  virtual void computeU (
    Cell<T,Lattice> const& cell,
    T u[Lattice<T>::d] ) const;
  /// Yields 0;
  virtual void computeJ (
    Cell<T,Lattice> const& cell,
    T j[Lattice<T>::d] ) const;
  /// Yields NaN
  virtual void computeStress (
    Cell<T,Lattice> const& cell,
    T rho, const T u[Lattice<T>::d],
    T pi[util::TensorVal<Lattice<T> >::n] ) const;
  virtual void computeRhoU (
    Cell<T,Lattice> const& cell,
    T& rho, T u[Lattice<T>::d]) const;
  virtual void computeAllMomenta (
    Cell<T,Lattice> const& cell,
    T& rho, T u[Lattice<T>::d],
    T pi[util::TensorVal<Lattice<T> >::n] ) const;
  /// Does nothing
  virtual void defineRho(Cell<T,Lattice>& cell, T rho);
  /// Does nothing
  virtual void defineU(Cell<T,Lattice>& cell,
                       const T u[Lattice<T>::d]);
  /// Does nothing
  virtual void defineRhoU (
    Cell<T,Lattice>& cell,
    T rho, const T u[Lattice<T>::d]);
  /// Does nothing
  virtual void defineAllMomenta (
    Cell<T,Lattice>& cell,
    T rho, const T u[Lattice<T>::d],
    const T pi[util::TensorVal<Lattice<T> >::n] );
  /// Yields NaN
  virtual T getOmega() const;
  /// Does nothing
  virtual void setOmega(T omega_);
};

namespace instances {

template<typename T, template<typename U> class Lattice>
BulkMomenta<T,Lattice>& getBulkMomenta();

template<typename T, template<typename U> class Lattice>
ExternalVelocityMomenta<T,Lattice>& getExternalVelocityMomenta();

template<typename T, template<typename U> class Lattice>
BounceBack<T,Lattice>& getBounceBack();

template<typename T, template<typename U> class Lattice>
NoDynamics<T,Lattice>& getNoDynamics();

}

}

#endif
