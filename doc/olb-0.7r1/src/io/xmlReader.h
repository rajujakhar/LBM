/*  This file is part of the OpenLB library
 *
 *  Copyright (C) 2010 Jonas Latt, Jonas Fietz, Mathias Krause
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
 * Input/Output in XML format -- header file.
 */
#ifndef XML_IO_H
#define XML_IO_H
#ifdef ADT
template <class T, unsigned DIM> class ADf;
#endif
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#define TIXML_USE_STL
#include "external/tinyxml/tinyxml.h"
#include "complexGrids/mpiManager/mpiManager.h"
#include "io/ostreamManager.h"

namespace olb {

class XMLreader {
public:
  /**
   * Constructs a new XMLreader from another XMLreader
   * \param pParent The new root node for the XMLreader
   */
  XMLreader( TiXmlNode* pParent );

  /**
   * Constructs a new XMLreader from a file
   * \param fName The XML file name
   */
  XMLreader( const std::string& fName );

  /**
   * destructor
   */
  ~XMLreader();

  /**
   * Prints out the XML structure read in, mostly for debugging purposes
   */
  void print(int indent) const;

  /**
   * Read a value from the xml file
   * \param reference to return the value
   * \return returns the value
   */
  template <typename T> bool read(T& value) const;
#ifdef ADT
  template <typename T,unsigned DIM> bool read(ADf<T,DIM>& value) const;
#endif
  template <typename T> bool read(std::vector<T>& value) const;

  template <typename T> T get() const;

  /**
   * Return a Subtree placed at name
   * \param name The name from which to take the subtree
   */
  XMLreader const& operator[] (std::string name) const;

  /**
   * Return an iterator for this level at the tree
   */
  std::vector<XMLreader*>::const_iterator begin() const;

  /**
   * Return an iterator end element
   */
  std::vector<XMLreader*>::const_iterator end() const;

  /**
   * return the name of the element
   */
  std::string getName() const;
private:
  void mainProcessorIni(TiXmlNode* pParent);
  void slaveProcessorIni();
  XMLreader();
private:
  mutable OstreamManager clout;
  std::string name;
  std::string text;
  std::vector<XMLreader*> children;
  static XMLreader notFound;
};

template <typename T>
bool XMLreader::read(T& value) const {
  std::stringstream valueStr(text);
  T tmp = T();
  if (!(valueStr >> tmp)) {
    clout << std::string("Error: cannot read value from XML element ") << name << std::endl;
    return false;
  }
  value = tmp;
  return true;
}

#ifdef ADT
template <typename T, unsigned DIM>
bool XMLreader::read(ADf<T,DIM>& value) const {
  std::stringstream valueStr(text);
  T tmp = T();
  if (!(valueStr >> tmp)) {
    clout << std::string("Error: cannot read value from XML element ") << name << std::endl;
    return false;
  }
  value = ADf<T,DIM>(tmp);
  return true;
}
#endif

template <typename T>
bool XMLreader::read(std::vector<T>& values) const {
  std::stringstream multiValueStr(text);
  std::string word;
  std::vector<T> tmp(values);
  while (multiValueStr>>word) {
    std::stringstream valueStr(word);
    T value;
    if (!(valueStr >> value)) {
      clout << std::string("Error: cannot read value array from XML element ") << name << std::endl;
      return false;
    }
    tmp.push_back(value);
  }
  values.swap(tmp);
  return true;
}

template <typename T>
T XMLreader::get() const {
  std::stringstream valueStr(text);
  T tmp = T();
  if (!(valueStr >> tmp)) {
    clout << "Error: cannot read value from XML element " << name << std::endl;
  }
  return tmp;
}

}  // namespace olb

#endif  // XML_IO_H
