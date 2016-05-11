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

#ifndef QT_EVENTABSTRACTOR_H
#define QT_EVENTABSTRACTOR_H

#include <QObject>

#include <framework/aog_genericeventabstractor.h>

#include <framework/aog_behaviorconfiguration.h>

namespace csu {
namespace aog {
namespace gui {
namespace qt {

class QtEventAbstractor : public QObject, public csu::aog::framework::GenericEventAbstractor
{
public:
    QtEventAbstractor(csu::aog::framework::BehaviorConfiguration* context);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

protected:
    void install();
    void uninstall();
    void mapEvents();

};

}
}
}
}

#endif // QT_EVENTABSTRACTOR_H
