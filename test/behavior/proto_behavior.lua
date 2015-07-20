--#########################################
--##      Initial State Functions        ##
--#########################################


function click__pb_calibration_apply()
    __log("My Log: clic__apply")
    -- We can perform an action on widgets placed in differents dialogs and windows
    MainWindow.hs_balanceLevel:setValue(90)
end

function click__pb_calibration_reset()
    __log("My Log: clic__reset")
    MainWindow.hs_balanceLevel:setValue(10)
    MainWindow.di_ampliLevel:setValue(10)
    MainWindow.di_signalEcho:setValue(10)
    MainWindow.pb_left:setValue(10)
    MainWindow.pb_right:setValue(10)
end


-- ------------------------------------------------------------------------
power=0
function calculatePower()
   power=MainWindow.sb_bufferSize:value()*0.15 + MainWindow.sb_sampleRate:value()*0.35
   power=power*(1+(amplification/100))
   power=power*(1-((echo/100)/2))
end

function setChannelsValue()
   aux=power*2
   MainWindow.pb_left:setValue(power * ((100-balance)/100)*2)
   MainWindow.pb_right:setValue(power * ((balance/100)*2))
   --MainWindow.pb_left:setValue(power * ((100-balance)/100)*2)
   --MainWindow.pb_right:setValue(power * ((balance/100)*2))
end

balance=50
function click__hs_balanceLevel()
   __log("Changing balance")
   balance=MainWindow.hs_balanceLevel:value()
   calculatePower()
   setChannelsValue()
end


amplification=0
function click__di_ampliLevel()
   __log("Changing amplification")
   amplification=MainWindow.di_ampliLevel:value()
   calculatePower()
   --power=power*(1+MainWindow.di_ampliLevel:value()/100)
   setChannelsValue()
   MainWindow.lc_ampliLevel:display(MainWindow.di_ampliLevel:value())
end

echo=0
function click__di_signalEcho()
   __log("Changing echo")
   calculatePower()
   echo=MainWindow.di_signalEcho:value()
   setChannelsValue()
   MainWindow.lc_signalEcho:display(MainWindow.di_signalEcho:value())
end

function click__sb_bufferSize()
   __log("Changing buffer size")
   calculatePower()
   setChannelsValue()
end

function click__sb_sampleRate()
   __log("Changing sample rate")
   calculatePower()
   setChannelsValue()
end








