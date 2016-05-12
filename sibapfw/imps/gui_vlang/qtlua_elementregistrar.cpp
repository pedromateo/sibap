// -*- mode: c++; c-basic-offset: 4; c-basic-style: bsd; -*-
/*
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public License as
*   published by the Free Software Foundation; either version 3.0 of
*   the License, or (at your option) any later version.
*
*   This file is part of the SIBAP Prototyping Framework
*   http://pedromateo.github.io/sibap/
*/

#include "qtlua_elementregistrar.h"

#include <config/log_config_internal.h>

#include <boost/lexical_cast.hpp>

#include <imps/gui_vlang/qtlua_adapters.h>

#include <QApplication>
#include <QSpinBox>
#include <QFrame>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QList>
#include <QWidgetList>
#include <QListWidget>
#include <QLabel>
#include <QProgressBar>


namespace csu {
namespace aog {
namespace gui_vlang {
namespace qt_lua {

using namespace csu::aog::config::log;

QtLua_ElementRegistrar::QtLua_ElementRegistrar(BehaviorConfiguration *context):GenericElementRegistrar(context)
{
    _log::debug << "(QtLua_ElementRegistrar::constructor)" << std::endl;

    L_=NULL;
    registerClass(boost::lexical_cast<std::string>(QSpinBox::staticMetaObject.className()));
    registerClass(boost::lexical_cast<std::string>(QLineEdit::staticMetaObject.className()));
    registerClass(boost::lexical_cast<std::string>(QComboBox::staticMetaObject.className()));
    registerClass(boost::lexical_cast<std::string>(QPushButton::staticMetaObject.className()));
    registerClass(boost::lexical_cast<std::string>(QCheckBox::staticMetaObject.className()));
    registerClass(boost::lexical_cast<std::string>(QRadioButton::staticMetaObject.className()));
    registerClass(boost::lexical_cast<std::string>(QListWidget::staticMetaObject.className()));
    registerClass(boost::lexical_cast<std::string>(QLabel::staticMetaObject.className()));
    registerClass(boost::lexical_cast<std::string>(QDial::staticMetaObject.className()));
    registerClass(boost::lexical_cast<std::string>(QSlider::staticMetaObject.className()));
    registerClass(boost::lexical_cast<std::string>(QProgressBar::staticMetaObject.className()));
    registerClass(boost::lexical_cast<std::string>(QLCDNumber::staticMetaObject.className()));
}

QtLua_ElementRegistrar::~QtLua_ElementRegistrar(){}

boost::any QtLua_ElementRegistrar::registerGUIElements(boost::any& object)
{

    _log::debug << "(QtLua_ElementRegistrar::registerGUIElements) Registering GUI Elements." << std::endl;

    ///
    /// first, this method register all the widgets adapters in the
    /// lua_State object, that is encapsulated in the boost::any
    ///

    L_ =  boost::any_cast<lua_State*>(object);
    QtLua_Adapters::LoadAdapters(L_);

    return updateGUIElementRegister();
}

////FIXME DELETE
//void pintaGlobal(lua_State * L)
//{
//
//   for (luabind::iterator i(luabind::globals(L)), end; i != end; ++i)
//   {
//       std::string aux=boost::lexical_cast<std::string>(i.key());
//       std::cout<<"Clave: "<<aux<<std::endl;
//   }
//
//}

////FIXME DELETE
//void pintaLuaTable(std::string parentName,luabind::object table)
//{
//   if (!table)
//       return;
//
//   std::cout<<"Listando la tabla "<<parentName<<std::endl;
//   for (luabind::iterator i(table), end; i != end; ++i)
//   {
//       std::string aux=boost::lexical_cast<std::string>(i.key());
//       std::cout<<"Clave: "<<aux<<std::endl;
//   }
//
//}

void QtLua_ElementRegistrar::windowCreate(const std::string & windowName)
{
    _log::debug << "(QtLua_ElementRegistrar::windowCreate) "<<windowName<<std::endl;
    //FIXME WATCH PERFORMANCE
    //Maybe we should only register new windows with its child object
    //not the whole application again
    updateGUIElementRegister();
}

void QtLua_ElementRegistrar::windowDelete(const std::string & windowName)
{
    _log::debug << "(QtLua_ElementRegistrar::windowDelete) "<<windowName<<std::endl;

    //lua has a garbage collector and does not provide a proper delete
    //function throught luabind
    luabind::globals(L_)[windowName]=luabind::nil;

    //Delete the window table from internals data structures
    tables_.erase(windowName);

}

boost::any QtLua_ElementRegistrar::updateGUIElementRegister()
{
    assert(L_);

    ///
    /// then, it is necessary to register the widgets in the
    /// global context
    ///

    /// get global context
    luabind::object global = luabind::globals(L_);
    int nregistered = 0;

    //Getting all widgets from the application
    foreach ( QWidget *w, QApplication::allWidgets() )
    {

        std::string parentName=w->topLevelWidget()->objectName().toStdString();
        std::string elementName=w->objectName().toStdString();

        //Avoiding some qt stuff objects
        if ((parentName=="") || (elementName==""))
            continue;

        //Get the parent table
        luabind::object parentTable = global[parentName];

        //The table doesnt exist
        if (!parentTable)
        {
            parentTable=luabind::newtable(L_);
            //Storing the new table inside the global table
            global[parentName]=parentTable;
            tables_.insert(parentName);
        }

        //Assert to be sure that the parentTable is a valid lua table
        assert(luabind::type(parentTable) == LUA_TTABLE);

        std::string className = boost::lexical_cast<std::string>(w->metaObject()->className());

        //If it is registrable, (it ocurrs when the widgets class is a registrable class), it is visible and it has not
        //an empty name (which it is typical of auxiliar widgets)

        //FIXME UNDER TESTING. I dont know if this condition is enough to get a good filtering
        if (isRegistrable(className)){
          if (className==QLineEdit::staticMetaObject.className()){
              parentTable[elementName]=dynamic_cast<QLineEdit *>(w);
          }else if (className==QSpinBox::staticMetaObject.className()){
              parentTable[elementName]=dynamic_cast<QSpinBox *>(w);
          }else if (className==QComboBox::staticMetaObject.className()){
              parentTable[elementName]=dynamic_cast<QComboBox *>(w);
          }else if (className==QPushButton::staticMetaObject.className()){
              parentTable[elementName]=dynamic_cast<QPushButton *>(w);
          }else if (className==QCheckBox::staticMetaObject.className()){
              parentTable[elementName]=dynamic_cast<QCheckBox *>(w);
          }else if (className==QRadioButton::staticMetaObject.className()){
              parentTable[elementName]=dynamic_cast<QRadioButton *>(w);
          }else if (className==QListWidget::staticMetaObject.className()){
              parentTable[elementName]=dynamic_cast<QListWidget *>(w);
          }else if (className==QLabel::staticMetaObject.className()){
              parentTable[elementName]=dynamic_cast<QLabel *>(w);
          }else if (className==QDial::staticMetaObject.className()){
              parentTable[elementName]=dynamic_cast<QDial *>(w);
          }else if (className==QSlider::staticMetaObject.className()){
              parentTable[elementName]=dynamic_cast<QSlider *>(w);
          }else if (className==QProgressBar::staticMetaObject.className()){
              parentTable[elementName]=dynamic_cast<QProgressBar *>(w);
          }else if (className==QLCDNumber::staticMetaObject.className()){
              parentTable[elementName]=dynamic_cast<QLCDNumber *>(w);
          }else{
              continue;
          }

          nregistered++;

          //_log::debug << "(QtLua_ElementRegistrar::updateGUIElementRegister) Registered: " << elementName <<std::endl;
        }
    }

    _log::debug << "(QtLua_ElementRegistrar::updateGUIElementRegister) #Registered widgets: "
                << nregistered << std::endl;

    return tables_;
}



}
}
}
}
