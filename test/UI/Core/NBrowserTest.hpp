// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_GUI_Client_uTests_NBrowserTest_hpp
#define CF_GUI_Client_uTests_NBrowserTest_hpp

////////////////////////////////////////////////////////////////////////////

#include <QObject>

////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace UI {
namespace CoreTest {

  //////////////////////////////////////////////////////////////////////////

  class NBrowserTest : public QObject
  {
    Q_OBJECT

  private slots:

    void test_generateName();

  }; // class NBrowserTest

  //////////////////////////////////////////////////////////////////////////

} // CoreTest
} // UI
} // CF

////////////////////////////////////////////////////////////////////////////

#endif // CF_GUI_Client_uTests_NBrowserTest_hpp
