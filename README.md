
**This implementation corresponds to a research work. It is ready to be downloaded and integrated into Qt/C++ projects. It uses Lua as the prototyping language**

First of all, watch this ;-)
https://www.youtube.com/watch?v=H9-wh-08BSg

This framework proposes to treat prototyping as a separate aspect of the agile development process.
Prototype designs are completely reused and iteratively refined towards the
final GUI design. In parallel, final functionality is implemented and
smoothly integrated into the prototype replacing prototyped behaviors.

The design and implementation of a framework supporting this approach is
presented. It uses separate files to define prototyped behaviors, which are written
using general-purpose scripting languages and executed by an
interpreter. The GUI and the final functionality run natively.
Native and interpreted functionality coexist and run in parallel within the
application.

As a result, developers and UX designers work in parallel, separately, but
collaborating on the same software artifact. This encourages communication
during development. Scripting languages allow also to change the prototype on-the-fly in order
to incorporate suggestions from different stakeholders in real time. This
allows to gather feedback more effectively by implementing participatory
design techniques.

## Running example

While the SIBAP framework is included in `sibapfw` folder, an example including this
framework is included in `test` folder. 

This example can be easily opened, compiled and run with 
QtCreator (https://www.qt.io/ide/). 
Behavior files are included in `behavior` folder. 
An execution log can be found at `/tmp/sibap_behavior.log`.



## Framework integration

The integration process is divided into two stages: configuration and deployment.

Stage 1: Configuration. The process is easily configured using a BehaviorConfiguration
object. This object encapsulates all configuration data needed during execution (e.g., the di-rectory of behavior files or refreshing time) It can be instantiated from a configuration file:

    int main(int argc, char *argv[])
    {
        QApplication qapp(argc, argv);
        MainWindow w;
        w.show();

        ///
        /// behavior layer configuration

        // Alternative 1
        //BehaviorConfiguration bconf("../layer.config");

        // Alternative 2
        //BehaviorConfiguration bconf;
        //bconf.loadConfigFile("layer.config");

        // Alternative 3
        BehaviorConfiguration bconf;
        bconf.updateFileTime(10)
             .logFormat("[%tm] %wi Event:%en :: State: %cs :: Assertion: %ar :: Function Called: %fc :: Result: %fr")
             .addLogOutputFile("/tmp/sibap_behavior.log")
             .addScriptPath("./test/behavior/");

        // You can also configure if the log info goes to the screen.
        // Available parameters are on,off
        // StandardOutput=on
        // StandardError=off
    ...

Stage 2: Deployment. Once the configuration object is created, it is used to create and configure a new BehaviorLayer. The BehaviorLayer object encapsulates the whole prototyping framework. Once created, the
init() method is called to deploy and start the behavior service:


    ...
        ///
        /// behavior layer deployment

        QtLua_BehaviorLayer blayer(&bconf);
        blayer.init();

        ///

        return qapp.exec();
    }

## Framework functions

### Main function template

    state_stateID__event__widget()  // Executed when event is performed on widget 
                                    // being in state stateID. state stateID is optional.

### Initialization functions

    init__widget()                  // Initialization of widget
    state_stateID__widget()         // Initialization of widget when stateID is enabled
    
Example:

    function init__di_ampliLevel()
        __log("Initializing dials")
        MainWindow.di_ampliLevel:setValue(50)
        MainWindow.di_signalEcho:setValue(50)
        MainWindow.di_nrLevel:setValue(50)
        MainWindow.di_volumeLevel:setValue(50)
    end

### State functions

    state__stateID()                // Does transition to state stateID. (State functions
                                    // are automatically defined by SIBAP).

### Assert functions

    assert__widget()                // Assertion for a widget must be satisfied for its
                                    // behavior functions to be executed.
                                    
Example:

    function assert__hs_balanceLevel()
       __log("Asserting balance")
       balance=MainWindow.hs_balanceLevel:value()
       return balance >= 0 and balance <= 99
    end

### Logging functions

    log__widget()                   // Executed when widget is interacted
    event__log()                    // Executed when event is performed
    __log(content)                  // Adds content to log streams
    __flog(fileName,content)        // Adds content to the file fileName


### Supporting functions

All the expressiveness of the scripting language
(Lua in this case) and its interpreter can be used to define the behavior
of a GUI. Variables, data structures, supporting functions, etc. can be
used in the behavior functions to compute additional values and build
high-quality prototypes.

Examples:

    function calculatePower()
       power=buffer_size * 0.5 + sample_rate * 0.5
       ...
    end

    function setChannelsValue()
       aux=power*2
       MainWindow.pb_left:setValue(power * ((100-balance)/100)*2)
       ...
    end

    function updateValues()
       calculatePower()
       setChannelsValue()
    end

## Framework events

All the following scene events (i.e., those application events related to user 
interaction) are supported by SIBAP, thus can be used to define behavior functions:

- `click`
- `doubleclick`
- `wheel`
- `focusin`
- `focusout`
- `mouseoverenter`
- `mouseoverleave`
- `mouseovermove`
- `keypress`
- `keyrelease`
- `show`
- `hide`
- `repaint`

Examples:

    function click__di_ampliLevel()
       __log("Changing amplification")
       amplification=MainWindow.di_ampliLevel:value()
       MainWindow.lc_ampliLevel:display(amplification)
       updateValues() -- supporting functions can be used as well
    end


    function wheel__hs_balanceLevel()
       __log("Changing balance")
       balance=MainWindow.hs_balanceLevel:value()
       updateValues()
    end




## Prototype initialization

Designers start creating a file, say initialization.lua, to define how the
values in the prototype GUI are initialized. This file is included in the directory
behavior/MainWindow/ to indicate that this behavior file corresponds to the dialog with
name MainWindow. init * functions are used to initialize the prototype (see above).

    function init__widget_name()
      MainWindow.a_widget:setValue(0)
      MainWindow.another_widget:setValue(0)
    end

## Defining prototyped behavors

In order to define new prototyped behavior, a new file, say somebehavior.lua, is created
in the behavior/MainWindow/ folder. The following code is used to handle the click event
performed on the widget1 widget:

    value, result = 0
    
    function click__widget1()
       value = MainWindow.widget2:value()
       calculateSomething()
    end
    
    function calculateSomething()
       result = ...
    end

Supporting functions can be used along with behavior functions.


## Integrating native functionality

At some point during the development, prototyped behaviors are replaced with the corresponding native functionality. Native functionality is implemented, as usual, using C++ methods and functions. Once the native functionality is integrated into the implementation of the application (i.e., the *.h and *.cpp files) the corresponding prototyped behaviors need to be removed from the script files. 

After recompiling and relaunching the application (needed to make native functionality work) the application will respond now with native functionality when the corresponding widget is interacted.




Go to http://www.catedrasaes.org/html/projects/sibap/sibap.html for further project description.
