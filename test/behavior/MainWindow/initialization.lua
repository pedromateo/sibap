
-- NOTE: we can place this functionality in any file, the decision of creating
-- a new file only for these methods is only for a better reading and
-- understanding

--#########################################
--##       Initialization methods        ##
--#########################################

function init__hs_balanceLevel()
    __log("Initializing balance")
    MainWindow.hs_balanceLevel:setValue(50)
end

function init__di_ampliLevel()
    __log("Initializing dials")
    MainWindow.di_ampliLevel:setValue(50)
    MainWindow.di_signalEcho:setValue(50)
    MainWindow.di_nrLevel:setValue(50)
    MainWindow.di_volumeLevel:setValue(50)
end

function init__sb_bufferSize()
    __log("Initializing the rest of values")
    MainWindow.sb_bufferSize:setValue(50)
    MainWindow.sb_sampleRate:setValue(20)
end

