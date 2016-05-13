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

#ifndef AOG_DEFINITIONS_H
#define AOG_DEFINITIONS_H

/// functions signature elements

#define BF_INIT "init"
#define BF_LOG "log"
#define BF_STATE "state_"
#define BF_ASSERT "assert"
#define BF_DSEP "__"


/// configuration options

#define BO_UPDATETIME "UpdateFileTime"
#define BO_LOGFORMAT "LogFormat"
#define BO_LOGOUTPUTFILE "LogOutPutFile"
#define BO_SCRIPTSPATH "ScriptPath"
#define BO_STDERR "StandardError"
#define BO_STDOUT "StandardOutput"


/// log format options

#define LO_BASE '%'
#define LO_TIMESTAMP "tm"
#define LO_EVENTNAME "en"
#define LO_WIDGET "wi"
#define LO_CSTATE "cs"
#define LO_ASSERT "ar"
#define LO_ASSERT_OK "Satisfied"
#define LO_ASSERT_KO "Not Satisfied"
#define LO_FUNCTIONCALLED "fc"
#define LO_FUNCTIONRESULT "fr"
#define LO_FUNCTIONRESULT_OK "True"
#define LO_FUNCTIONRESULT_KO "False"

// behavior states
#define BS_INITSTATE "init"

#endif // AOG_DEFINITIONS_H
