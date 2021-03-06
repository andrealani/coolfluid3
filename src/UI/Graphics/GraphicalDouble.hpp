// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_GUI_Graphics_GraphicalDouble_hpp
#define CF_GUI_Graphics_GraphicalDouble_hpp

////////////////////////////////////////////////////////////////////////////

#include "UI/Graphics/GraphicalValue.hpp"

class QDoubleValidator;
class QLineEdit;

////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace UI {
namespace Graphics {

//////////////////////////////////////////////////////////////////////////

class Graphics_API GraphicalDouble : public GraphicalValue
{
  Q_OBJECT

public:

  GraphicalDouble(Real value = 0.0, QWidget * parent = 0);

  ~GraphicalDouble();

  virtual bool setValue(const QVariant & value);

  virtual QVariant value() const;

private slots:

  void textUpdated(const QString & text);

private:

  QLineEdit * m_lineEdit;

  QDoubleValidator * m_validator;

}; // class GraphicalDouble

//////////////////////////////////////////////////////////////////////////

} // Graphics
} // UI
} // CF

////////////////////////////////////////////////////////////////////////////

#endif // CF_GUI_Graphics_GraphicalDouble_hpp
