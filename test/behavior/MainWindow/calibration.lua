power=0
nr=50
volume=50
amplification=50
echo=50
balance=50
buffer_size=50
sample_rate=20

--#########################################
--##                Power                ##
--#########################################


function calculatePower()
   power=buffer_size * 0.5 + sample_rate * 0.5
   power=power*(1+(amplification/100))
   power=power*(1-((echo/100)/2))
   power=power*volume/100
   power=power - nr/10
end

function setChannelsValue()
   aux=power*2
   MainWindow.pb_left:setValue(power * ((100-balance)/100)*2)
   MainWindow.pb_right:setValue(power * ((balance/100)*2))
end

function updateValues()
   calculatePower()
   setChannelsValue()
end


--#########################################
--##                Dials                ##
--#########################################


function click__di_nrLevel()
   __log("Changing n/r level")
   nr=MainWindow.di_nrLevel:value()
   MainWindow.lc_nrLevel:display(nr)
   updateValues()
end

function click__di_volumeLevel()
   __log("Changing volume level")
   volume=MainWindow.di_volumeLevel:value()
   MainWindow.lc_volumeLevel:display(volume)
   updateValues()
end

function click__di_ampliLevel()
   __log("Changing amplification")
   amplification=MainWindow.di_ampliLevel:value()
   MainWindow.lc_ampliLevel:display(amplification)
   updateValues()
end

function click__di_signalEcho()
   __log("Changing echo")
   echo=MainWindow.di_signalEcho:value()
   MainWindow.lc_signalEcho:display(echo)
   updateValues()
end


--#########################################
--##           Other widgets             ##
--#########################################


function click__hs_balanceLevel()
   __log("Changing balance")
   balance=MainWindow.hs_balanceLevel:value()
   updateValues()
end

function click__sb_bufferSize()
   __log("Changing buffer size")
   buffer_size=MainWindow.sb_bufferSize:value()
   updateValues()
end

function click__sb_sampleRate()
   __log("Changing sample rate")
   sample_rate=MainWindow.sb_sampleRate:value()
   updateValues()
end


--#########################################
--##              Buttons                ##
--#########################################


function click__pb_calibration_apply()
    __log("My Log: clic__apply")
    -- TODO check native implementation
end

function click__pb_calibration_reset()
    __log("My Log: clic__reset")
    -- methods from other files can be called as well
    init__hs_balanceLevel()
    init__di_ampliLevel()
    init__sb_bufferSize()
    updateValues()
end


--#########################################
--##            Assertions               ##
--#########################################

-- Assertions must be always satisfied.
-- If the assertion for a widget is not satisfied, 
-- then their functions are not executed.

function assert__hs_balanceLevel()
   __log("Asserting balance")
   balance=MainWindow.hs_balanceLevel:value()
   return balance >= 0 and balance <= 99
end


