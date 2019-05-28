# How to Configure Everything Using Configuration Files

Configuration files can be used to set up any variable in the code base. For example, IP and port number to be used by the telemetry module, number of IMUs in the system, GPIO pins used to embrakes, ...

The main advantage of configuring all this using configuration file instead of hard-coding this in the code it that you can see easily look up the current configuration just by looking at a single file and there is no need to recompile the code to change the configuration.

## Running
Configuration to be used is `<root>/config.txt` by default. The configuration file can be changed for any setting, e.g. fake/testing/disable telemetry, by running
`./hyped --config=<path to config file>` or `./hyped -C=<path to config file>`.

## Using Configuration in the Code

Several steps need to be performed in order to use configuration files for your variable. There is an example usage of configuration files for IP and port number used by the telemetry module.

Config file is parsed during program start up to fill a unique (think singleton) object structure of type `hyped::utils::Config`. This object can have any number of sub-objects with unique structure. As a rule of thumb, try to keep the substructures with the same granularity as modules in `hyped::data::Data` and/or code modules that have `hyped::<module>::Main` function defined. I.e., `telemetry`, `sensors`, `navigation`, `motors`, `state_machine`.

### Structure of config.txt file
Lines starting with `#` are considered comments and have no effect on parsing the config file.

Lines with format `> <module>`, e.g. `> Telemetry` indicate that following lines should be parsed into using a substructure with the `<module>` name.

All other lines can indicate variables/values in any format. For example:
`IP 192.168.0.7`
indicates `hyped::utils::Config::telemetry.IP` should be configured with value `192.168.0.7`.

### Adding new modules
To add another module, first extend definition of `hyped::utils::Config` class with a custom module substructure. This is done in file `src/utils/config.hpp`.

Furthermode, declare a member function that will parse all config lines for this module. The parser function needs to have signature `void <Name>(char* line)`.

For example:
```
class Config {
  ...
  struct Telemetry {
    char IP[16];
    char Port[5];
  } telemetry;
  ...
  void ParseTelemetry(char* line);
  ...
}
```

Then, let `hyped::utils::Config` class know there is the new submodule and define the parser functon in `src/utils/config.cpp` file. You can start by extending the `ModuleEntry` table on line 51, following the same pattern as previous modules. This is to define what module name should be recognised in the config.txt file and which parser function should be used for all config lines belonging to the corresponding module. Note, module name cannot be longer than 20 characters.


### Parsing line in config.txt file
Have a look at `hyped::utils::Config::ParseTelemetry` function. You will need to parse all line manually yourself. This is a lot of string manipulation but this guide is here to help.

The simples approach is to keep to this format for config lines:
`<variable name> <value>`
e.g.
`port 5500`

With this format, parsing lines is easy:
1. Get the variable name by "splitting" (tokenizing) the input line using something like `char* token = strtok(line, " ");`
2. Switch on the variable name (token) by comparing the token string with known variable names, e.g. `if (strcmp(token, "port") == 0)`
3. Get the line value by getting the second "split" value (token) using `char* value = strtok(NULL, " ");`
4. Store the line value into the corresponding variable in the module substructure, e.g. `strncpy(telemetry.Port, value, 5);`
   * Optional: sometimes you need to convert the string value into another type, e.g. for `int port`, use `telemetry.port = atoi(value);`

Useful string manipulation commands:
`strtok` - tokenized, almost line `split` in python, usage is a bit tricky, reading documenation is recommended
`strcmp` - string comparison, return value of 0 indicates equality, positive/negative values indicate alphabetical ordering
`atoi`   - convert string, e.g. "-23" or "0x34" into integer value
`atof`   - similar to `atoi` but for double floating values

### Using configured values
If configuration lines are correctly parsed into the substructures of `hyped::utils::Config` class, the variables can be accessed from the `hyped::utils::System` singleton object. For example:
```
hyped::utils::Config* config = hyped::utils::System::getSystem().config;
char* portToUse = config->telemetry.Port;
```

