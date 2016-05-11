# saproto

### Script-based Aspect-oriented Prototyping Framework

*This implementation corresponds to a research work. It is ready to be downloaded and integrated into Qt/C++ projects. It uses Lua as the prototyping language* 

To attempt to ease collaboration, this framework can be used by designers and developers to work together on the same software artifact. Prototypes are reused and iteratively refined towards the final application: they are provided with new, final functionality while prototyped behavior is removed.

To support this, this framework treats prototyped and final functionality separately. On the one hand, prototyped functionality is decoupled from the GUI specification and the final implementation. It is implemented in separate files using interpreted languages. On the other hand, final functionality is native and implemented into the application code. Both coexist and run together.

Since interpreted languages are used to implement prototyped functionality, then it can be changed at runtime. Prototypes can be changed on the fly to support the immediate integration of stakeholders decisions and suggestions during participatory design sessions. Feedback from users, clients, etc., can then be obtained more effectively during testing and evaluation stages. 

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



Go to http://www.catedrasaes.org/html/projects/sibap/sibap.html for further project description and instructions.
