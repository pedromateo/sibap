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

#include "qtlua_adapters.h"

// QTimeEdit
int QtLua_Adapters::QTimeEdit_hour(const QTimeEdit * qte)
{
return qte->time().hour();
}

int QtLua_Adapters::QTimeEdit_minute(const QTimeEdit * qte)
{
return qte->time().minute();
}

int QtLua_Adapters::QTimeEdit_second(const QTimeEdit * qte)
{
return qte->time().second();
}

// QDateEdit
int QtLua_Adapters::QDateEdit_day(const QDateEdit * qde){
return qde->date().day();
}
int QtLua_Adapters::QDateEdit_month(const QDateEdit * qde){
return qde->date().month();
}
int QtLua_Adapters::QDateEdit_year(const QDateEdit * qde){
return qde->date().year();
}

// QDateTimeEdit
int QtLua_Adapters::QDateTimeEdit_hour(const QDateTimeEdit * qdte)
{
return qdte->time().hour();
}
int QtLua_Adapters::QDateTimeEdit_minute(const QDateTimeEdit * qdte)
{
return qdte->time().minute();
}
int QtLua_Adapters::QDateTimeEdit_second(const QDateTimeEdit * qdte)
{
return qdte->time().second();
}
int QtLua_Adapters::QDateTimeEdit_day(const QDateTimeEdit * qdte)
{
return qdte->date().day();
}
int QtLua_Adapters::QDateTimeEdit_month(const QDateTimeEdit * qdte)
{
return qdte->date().month();
}
int QtLua_Adapters::QDateTimeEdit_year(const QDateTimeEdit * qdte)
{
return qdte->date().year();
}

// QComboBox
std::string QtLua_Adapters::QComboBox_currentText(const QComboBox * qcb){
return qcb->currentText().toStdString();
}

// QFontComboBox
std::string QtLua_Adapters::QFontComboBox_currentText(const QFontComboBox * qfcb){
return qfcb->currentText().toStdString();
}

// QLineEdit
std::string QtLua_Adapters::QLineEdit_text(const QLineEdit * qle)
{
return qle->text().toStdString();
}
std::string QtLua_Adapters::QLineEdit_selectedText(const QLineEdit * qle)
{
return qle->selectedText().toStdString();
}

// QTextEdit
std::string QtLua_Adapters::QTextEdit_plainText(const QTextEdit * qte)
{
return qte->toPlainText().toStdString();
}

// QPlainTextEdit
std::string QtLua_Adapters::QPlainTextEdit_plainText(const QPlainTextEdit * qpte)
{
return qpte->toPlainText().toStdString();
}

// QLabel
std::string QtLua_Adapters::QLabel_text(const QLabel * ql)
{
return ql->text().toStdString();
}

void QtLua_Adapters::QLabel_setText(const QLabel * ql,const std::string & text)
{
    const_cast<QLabel *>(ql)->setText(text.c_str());
}

// QPushButton
std::string QtLua_Adapters::QPushButton_text(const QPushButton * qpb)
{
return qpb->text().toStdString();
}

// QRadioButton
std::string QtLua_Adapters::QRadioButton_text(const QRadioButton * qrb)
{
return qrb->text().toStdString();
}

// QCheckBox
std::string QtLua_Adapters::QCheckBox_text(const QCheckBox * qcb)
{
return qcb->text().toStdString();
}

// QGroupBox
std::string QtLua_Adapters::QGroupBox_title(const QGroupBox * qgb)
{
return qgb->title().toStdString();
}

void QtLua_Adapters::QWidget_move(const QWidget * w,int x,int y)
{
    const_cast<QWidget *>(w)->move(x,y);
}


void QtLua_Adapters::QListWidget_addItem(const QListWidget * w,const std::string &item)
{
    const_cast<QListWidget *>(w)->addItem(item.c_str());
}

void QtLua_Adapters::QListWidget_insertItem(const QListWidget * w,int row,const std::string &item)
{
    const_cast<QListWidget *>(w)->insertItem(row,item.c_str());
}

QListWidgetItem * QtLua_Adapters::QListWidget_currentItem(const QListWidget * w)
{
   return w->currentItem();
}

std::string QtLua_Adapters::QListWidgetItem_text(const QListWidgetItem *w)
{
    return w->text().toStdString();
}

// QLCDNumber
void QtLua_Adapters::QLCDNumber_display(const QLCDNumber * w, int value)
{
    const_cast<QLCDNumber *>(w)->display(value);
}

///
/// adapters loading method
///
void QtLua_Adapters::LoadAdapters(lua_State *L)
{

///
/// Generic QWidget adapter
///

//FIXME cambiar para utilizar binding
luabind::module(L) [
        luabind::class_<QWidget>("QWidget")
        .def("isVisible", &QWidget::isVisible)
        .def("setEnabled", &QWidget::setEnabled)
        .def("x", &QWidget::x)
        .def("y", &QWidget::y)
        .def("width", &QWidget::width)
        .def("height", &QWidget::height)
        .def("move",luabind::tag_function<void(const QWidget *,int,int)>(&QtLua_Adapters::QWidget_move))
        ];
///
/// QSpinBox adapter
///
luabind::module(L)[
        luabind::class_<QSpinBox,QWidget>("QSpinBox")
        .def(luabind::constructor<QWidget*>())
        .def("maximum",&QSpinBox::maximum)
        .def("minimum",&QSpinBox::minimum)
        .def("value",&QSpinBox::value)
        .def("isReadOnly",&QSpinBox::isReadOnly)
        .def("isVisible",&QSpinBox::isVisible)
        .def("isEnabled",&QSpinBox::isEnabled)
        ///
        .def("setValue",&QSpinBox::setValue)
        ];

///
/// QDoubleSpinBox adapter
///
luabind::module(L)[
        luabind::class_<QDoubleSpinBox,QWidget>("QDoubleSpinBox")
        .def(luabind::constructor<QWidget*>())
        .def("maximum",&QDoubleSpinBox::maximum)
        .def("minimum",&QDoubleSpinBox::minimum)
        .def("value",&QDoubleSpinBox::value)
        .def("decimals",&QDoubleSpinBox::decimals)
        .def("readOnly",&QDoubleSpinBox::isReadOnly)
        .def("isVisible",&QDoubleSpinBox::isVisible)
        .def("isEnabled",&QDoubleSpinBox::isEnabled)
        ];

///
/// QTimeEdit adapter
///
luabind::module(L)[
        luabind::class_<QTimeEdit,QWidget>("QTimeEdit")
        .def(luabind::constructor<QWidget*>())
        .def("hour", luabind::tag_function<int(const QTimeEdit *)>(&QtLua_Adapters::QTimeEdit_hour))
        .def("minute", luabind::tag_function<int(const QTimeEdit *)>(&QtLua_Adapters::QTimeEdit_minute))
        .def("second", luabind::tag_function<int(const QTimeEdit *)>(&QtLua_Adapters::QTimeEdit_second))
        .def("isReadOnly",&QTimeEdit::isReadOnly)
        .def("isVisible",&QTimeEdit::isVisible)
        .def("isEnabled",&QTimeEdit::isEnabled)
        ];

///
/// QDateEdit adapter
///
luabind::module(L)[
        luabind::class_<QDateEdit,QWidget>("QDateEdit")
        .def(luabind::constructor<QWidget*>())
        .def("day", luabind::tag_function<int(const QDateEdit *)>(&QtLua_Adapters::QDateEdit_day))
        .def("month", luabind::tag_function<int(const QDateEdit *)>(&QtLua_Adapters::QDateEdit_month))
        .def("year", luabind::tag_function<int(const QDateEdit *)>(&QtLua_Adapters::QDateEdit_year))
        .def("isReadOnly",&QDateEdit::isReadOnly)
        .def("isVisible",&QDateEdit::isVisible)
        .def("isEnabled",&QDateEdit::isEnabled)
        ];

///
/// QDateTimeEdit adapter
///
luabind::module(L)[
        luabind::class_<QDateTimeEdit,QWidget>("QDateTimeEdit")
        .def(luabind::constructor<QWidget*>())
        .def("hour", luabind::tag_function<int(const QDateTimeEdit *)>(&QtLua_Adapters::QDateTimeEdit_hour))
        .def("minute", luabind::tag_function<int(const QDateTimeEdit *)>(&QtLua_Adapters::QDateTimeEdit_minute))
        .def("second", luabind::tag_function<int(const QDateTimeEdit *)>(&QtLua_Adapters::QDateTimeEdit_second))
        .def("day", luabind::tag_function<int(const QDateTimeEdit *)>(&QtLua_Adapters::QDateTimeEdit_day))
        .def("month", luabind::tag_function<int(const QDateTimeEdit *)>(&QtLua_Adapters::QDateTimeEdit_month))
        .def("year", luabind::tag_function<int(const QDateTimeEdit *)>(&QtLua_Adapters::QDateTimeEdit_year))
        .def("isReadOnly",&QDateTimeEdit::isReadOnly)
        .def("isVisible",&QDateTimeEdit::isVisible)
        .def("isEnabled",&QDateTimeEdit::isEnabled)
        ];

///
/// QComboBox adapter
///
luabind::module(L)[
        luabind::class_<QComboBox,QWidget>("QComboBox")
        .def(luabind::constructor<QWidget*>())
        .def("count", &QComboBox::count)
        .def("currentIndex", &QComboBox::currentIndex)
        .def("currentText", luabind::tag_function<std::string(const QComboBox *)>(&QtLua_Adapters::QComboBox_currentText))
        .def("isVisible",&QComboBox::isVisible)
        .def("isEnabled",&QComboBox::isEnabled)
        ];

///
/// QFontComboBox adapter
///
luabind::module(L)[
        luabind::class_<QFontComboBox,QWidget>("QFontComboBox")
        .def(luabind::constructor<QWidget*>())
        .def("count", &QFontComboBox::count)
        .def("currentIndex", &QFontComboBox::currentIndex)
        .def("currentText", luabind::tag_function<std::string(const QFontComboBox *)>(&QtLua_Adapters::QFontComboBox_currentText))
        .def("isVisible",&QFontComboBox::isVisible)
        .def("isEnabled",&QFontComboBox::isEnabled)
        ];

///
/// QLineEdit adapter
///
luabind::module(L)[
        luabind::class_<QLineEdit,QWidget>("QLineEdit")
        .def(luabind::constructor<QWidget*>())
        .def("maxLength", &QLineEdit::maxLength)
        .def("isModified", &QLineEdit::isModified)
        .def("isReadOnly", &QLineEdit::isReadOnly)
        .def("selectedText",luabind::tag_function<std::string(const QLineEdit *)>(&QtLua_Adapters::QLineEdit_selectedText))
        .def("hasSelectText", &QLineEdit::hasSelectedText)
        .def("text", luabind::tag_function<std::string(const QLineEdit *)>(&QtLua_Adapters::QLineEdit_text))
        .def("cursorPosition", &QLineEdit::cursorPosition)
        .def("isVisible",&QLineEdit::isVisible)
        .def("isEnabled",&QLineEdit::isEnabled)
        ];

///
/// QTextEdit adapter
///
luabind::module(L)[
        luabind::class_<QTextEdit,QWidget>("QTextEdit")
        .def(luabind::constructor<QWidget*>())
        .def("plainText", luabind::tag_function<std::string(const QTextEdit *)>(&QtLua_Adapters::QTextEdit_plainText))
        .def("isReadOnly",&QTextEdit::isReadOnly)
        .def("isVisible",&QTextEdit::isVisible)
        .def("isEnabled",&QTextEdit::isEnabled)
        ];

///
/// QPlainTextEdit adapter
///
luabind::module(L)[
        luabind::class_<QPlainTextEdit,QWidget>("QPlainTextEdit")
        .def(luabind::constructor<QWidget*>())
        .def("plainText", luabind::tag_function<std::string(const QPlainTextEdit *)>(&QtLua_Adapters::QPlainTextEdit_plainText))
        .def("isReadOnly",&QPlainTextEdit::isReadOnly)
        .def("isVisible",&QPlainTextEdit::isVisible)
        .def("isEnabled",&QPlainTextEdit::isEnabled)
        ];

///
/// QDial adapter
///
luabind::module(L)[
        luabind::class_<QDial,QWidget>("QDial")
        .def(luabind::constructor<QWidget*>())
        .def("maximum",&QDial::maximum)
        .def("minimum",&QDial::minimum)
        .def("value",&QDial::value)
        .def("isVisible",&QDial::isVisible)
        .def("isEnabled",&QDial::isEnabled)
        ///
        .def("setValue",&QDial::setValue)
        ];

///
/// QSlider adapter
///
luabind::module(L)[
        luabind::class_<QSlider,QWidget>("QSlider")
        .def(luabind::constructor<QWidget*>())
        .def("maximum",&QSlider::maximum)
        .def("minimum",&QSlider::minimum)
        .def("value",&QSlider::value)
        .def("isVisible",&QSlider::isVisible)
        .def("isEnabled",&QSlider::isEnabled)
        ///
        .def("setValue",&QSlider::setValue)
        ];

///
/// QScrollBar adapter
///
luabind::module(L)[
        luabind::class_<QScrollBar,QWidget>("QScrollBar")
        .def(luabind::constructor<QWidget*>())
        .def("maximum",&QScrollBar::maximum)
        .def("minimum",&QScrollBar::minimum)
        .def("value",&QScrollBar::value)
        .def("isVisible",&QScrollBar::isVisible)
        .def("isEnabled",&QScrollBar::isEnabled)
        ];

///
/// QProgressBar adapter
///
luabind::module(L)[
        luabind::class_<QProgressBar,QWidget>("QProgressBar")
        .def(luabind::constructor<QWidget*>())
        .def("maximum",&QProgressBar::maximum)
        .def("minimum",&QProgressBar::minimum)
        .def("value",&QProgressBar::value)
        .def("isVisible",&QProgressBar::isVisible)
        .def("isEnabled",&QProgressBar::isEnabled)
        ///
        .def("setValue",&QProgressBar::setValue)
        ];

///
/// QLabel adapter
///
luabind::module(L)[
        luabind::class_<QLabel,QWidget>("QLabel")
        .def(luabind::constructor<QWidget*>())
        .def("text", luabind::tag_function<std::string(const QLabel *)>(&QtLua_Adapters::QLabel_text))
        .def("setText", luabind::tag_function<void(const QLabel *,const std::string &)>(&QtLua_Adapters::QLabel_setText))
        .def("isVisible",&QLabel::isVisible)
        .def("isEnabled",&QLabel::isEnabled)
        ];

///
/// QPushButton adapter
///
luabind::module(L)[
        luabind::class_<QPushButton,QWidget>("QPushButton")
        .def(luabind::constructor<QWidget*>())
        .def("text", luabind::tag_function<std::string(const QPushButton *)>(&QtLua_Adapters::QPushButton_text))
        .def("isDefault",&QPushButton::isDefault)
        .def("isCheckable",&QPushButton::isCheckable)
        .def("isChecked",&QPushButton::isChecked)
        .def("isDown",&QPushButton::isDown)
        .def("isVisible",&QPushButton::isVisible)
        .def("isEnabled",&QPushButton::isEnabled)
        ];

///
/// QRadioButton adapter
///
luabind::module(L)[
        luabind::class_<QRadioButton,QWidget>("QRadioButton")
        .def(luabind::constructor<QWidget*>())
        .def("text", luabind::tag_function<std::string(const QRadioButton *)>(&QtLua_Adapters::QRadioButton_text))
        .def("isCheckable",&QRadioButton::isCheckable)
        .def("isChecked",&QRadioButton::isChecked)
        .def("isDown",&QRadioButton::isDown)
        .def("isVisible",&QRadioButton::isVisible)
        .def("isEnabled",&QRadioButton::isEnabled)
        ];

///
/// QCheckBox adapter
///
luabind::module(L)[
        luabind::class_<QCheckBox,QWidget>("QCheckBox")
        .def(luabind::constructor<QWidget*>())
        .def("text", luabind::tag_function<std::string(const QCheckBox *)>(&QtLua_Adapters::QCheckBox_text))
        .def("isCheckable",&QCheckBox::isCheckable)
        .def("isChecked",&QCheckBox::isChecked)
        .def("isDown",&QCheckBox::isDown)
        .def("checkState",&QCheckBox::checkState)
        .def("isTristate",&QCheckBox::isTristate)
        .def("isVisible",&QCheckBox::isVisible)
        .def("isEnabled",&QCheckBox::isEnabled)
        ];

///
/// QGroupBox adapter
///
luabind::module(L)[
        luabind::class_<QGroupBox,QWidget>("QGroupBox")
        .def(luabind::constructor<QWidget*>())
        .def("title", luabind::tag_function<std::string(const QGroupBox *)>(&QtLua_Adapters::QGroupBox_title))
        .def("isCheckable",&QGroupBox::isCheckable)
        .def("isChecked",&QGroupBox::isChecked)
        .def("isVisible",&QGroupBox::isVisible)
        .def("isEnabled",&QGroupBox::isEnabled)
        ];

luabind::module(L)[
        luabind::class_<QListWidget,QWidget>("QListWidget")
        .def(luabind::constructor<QWidget*>())
        .def("addItem", luabind::tag_function<void(const QListWidget *,const std::string & )>(&QtLua_Adapters::QListWidget_addItem))
        .def("count",&QListWidget::count)
        .def("currentRow",&QListWidget::currentRow)
        .def("clear",&QListWidget::clear)
        .def("insertItem",luabind::tag_function<void(const QListWidget *,int ,const std::string & )>(&QtLua_Adapters::QListWidget_insertItem))
        .def("currentItem", luabind::tag_function<QListWidgetItem *(const QListWidget *)>(&QtLua_Adapters::QListWidget_currentItem))
        ];

luabind::module(L)[
        luabind::class_<QListWidgetItem,QWidget>("QListWidgetItem")
        .def("text", luabind::tag_function<std::string(const QListWidgetItem *)>(&QtLua_Adapters::QListWidgetItem_text))
        ];

///
/// QLCDNumber adapter
///
luabind::module(L)[
        luabind::class_<QLCDNumber,QWidget>("QProgressBar")
        .def(luabind::constructor<QWidget*>())
        .def("value",&QLCDNumber::value)
        .def("isVisible",&QLCDNumber::isVisible)
        .def("isEnabled",&QLCDNumber::isEnabled)
        ///
        //.def("display",&QLCDNumber::display)
        .def("display", luabind::tag_function<void(const QLCDNumber *, int)>(&QtLua_Adapters::QLCDNumber_display))
        ];

}
