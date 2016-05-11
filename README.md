# SIBAP
# ScrIpt-Based Aspect-oriented Prototyping framework

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

## Framework integration

The integration process is divided into two stages: configuration and deployment.

Stage 1: Configuration. The process is easily configured using a BehaviorConfiguration
object. This object encapsulates all configuration data needed during execution (e.g., the di-rectory of behavior files or refreshing time) It can be instantiated from a configuration file:

    // behavior layer configuration
    BehaviorConfiguration bconf("config/layer.config");

Stage 2: Deployment. Once the configuration object is created, it is used to create and configure a new BehaviorLayer. The BehaviorLayer object encapsulates the whole prototyping framework. Once created, the
init() method is called to deploy and start the behavior service:

    // behavior layer deployment
    QtLua_BehaviorLayer blayer(bconf);
    blayer.init();

## Framework functions

### Main function template

    state stateID event widget()   // Executed when event is performed on widget 
                                   // being in state stateID. state stateID is optional.

### Initialization functions

    init widget()                  // Initialization of widget
    state stateID widget()         // Initialization of widget when stateID is enabled

### State functions

    state stateID()                // Does transition to state stateID. (State functions
                                   // are automatically defined by SIBAP ).

### Logging functions

    log widget()                   // Executed when widget is interacted
    event log()                    // Executed when event is performed
    log(content)                   // Adds content to log streams
    flog(fileName,content)         // Adds content to the file fileName


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
