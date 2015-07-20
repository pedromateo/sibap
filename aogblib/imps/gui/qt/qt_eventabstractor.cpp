// -*- mode: c++; c-basic-offset: 4; c-basic-style: bsd; -*-
/*
*   This program is free software; you can redistribute it and/or
*   modify
*   it under the terms of the GNU Lesser General Public License as
*   published by the Free Software Foundation; either version 3.0 of
*   the License, or (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with this library; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
*   02111-1307 USA
*
*   This file is part of the Aspect-Oriented Behavior Framework,
*   https://sourceforge.net/projects/aobf/
*
*/

#include "qt_eventabstractor.h"

#include <QApplication>
#include <QEvent>
#include <QWidget>
#include <QToolTip>

#include <config/log_config_internal.h>

namespace csu {
namespace aog {
namespace gui {
namespace qt {

using namespace csu::aog::framework;
using namespace csu::aog::config::log;

QtEventAbstractor::QtEventAbstractor(BehaviorConfiguration *context):GenericEventAbstractor(context)
{
    _log::debug << "QtEventAbstractor::constructor" << std::endl;
    mapEvents();
}

void QtEventAbstractor::install()
{
    qApp->installEventFilter(this);
}

void QtEventAbstractor::uninstall()
{
    qApp->removeEventFilter(this);
}

void QtEventAbstractor::mapEvents()
{
    eventMap_[QEvent::MouseButtonRelease]=EventNames[CLICK];
    eventMap_[QEvent::MouseButtonDblClick]=EventNames[DOUBLECLICK];
    eventMap_[QEvent::Wheel]=EventNames[WHEEL];
    eventMap_[QEvent::FocusOut]=EventNames[FOCUSOUT];
    eventMap_[QEvent::FocusIn]=EventNames[FOCUSIN];
    eventMap_[QEvent::HoverEnter]=EventNames[MOUSEOVERENTER];
    eventMap_[QEvent::HoverLeave]=EventNames[MOUSEOVERLEAVE];
    eventMap_[QEvent::HoverMove]=EventNames[MOUSEOVERMOVE];
    eventMap_[QEvent::KeyPress]=EventNames[KEYPRESS];
    eventMap_[QEvent::KeyRelease]=EventNames[KEYRELEASE];
    eventMap_[QEvent::Show]=EventNames[SHOW]; //USEFUL?
    eventMap_[QEvent::Hide]=EventNames[HIDE]; //USEFUL?
    eventMap_[QEvent::Paint]=EventNames[REPAINT]; //USEFUL?
}

bool QtEventAbstractor::eventFilter(QObject *obj, QEvent *event)
{

    //QTipLabels, as tooltip text labels, triggers winidchange and close events, thats why it is necessary to
    //filter them. Now i insert a condition in order to make the comparision. In the future
    // maybe should be useful to make a global condition to avoid this class, but only if necessary,
    //beacuse maybe we dont want them to jam with the window creation process, but maybe we want to
    //check them against the srs document


    std::string widgetName = obj->objectName().toStdString();

    if (widgetName == "")
        return false;

    // if event is in the list...
    if (eventMap_[event->type()] != "")
    {
        context_->aspectEngine->executeAction(eventMap_[event->type()],widgetName);
    }
    // if not, check dialog creation...
    else if (strcmp(obj->metaObject()->className(),"QTipLabel") != 0)
    {

        //A dialog has been closed
        if (event->type() == QEvent::HideToParent)
        {
            notifyDelete(widgetName);
        }
        //New Dialog has been created
        else if (event->type() == QEvent::WinIdChange)
        {
            notifyCreate(widgetName);
        }
    }

    return false;
}


}
}
}
}
