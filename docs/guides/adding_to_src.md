# Adding new files to src/


Source is split into seven main directories: `communications/`,  `data/`, `motor_control/`, `navigation/`, `sensors/`, `state_machine/`, and `utils/`. All demo files that test drivers are found directly in source.

#### Where to add new files

Add new files in apporpropriate subteam directories.
Name all tester/demo files as `demo_[purpose].cpp`

#### Editing Lint.files and Source.files
- *It is imperative to add new files (.cpp or .hpp) to these documents*
- *Write path to the new file starting after `src/`*
- *Make sure to include the path of the file if within one of the seven main directories*
1. `Lint.files`: ensures file is style-checked by linter
    - Add path of new file to the alphabatized list within the file
```
communications/new_classfile.cpp
communications/new_headerfile.cpp
```
2. `Source.files`: ensures file is compiled into project's binary
    - Under `SCRS` add path of new .cpp file to the alphabatized list 
        - Do not add .hpp files to `Source.files`
    - Under `MAINS` add file name without the .cpp extension to the alphabatized list 
    - Add a backslash after your edition to `Source.files`
```
SRCS := \
    communications/new_classfile.cpp \
```
```
MAINS := \
    demo_addingsrc  \
```

#### Things to remember
- Update the project you mount onto the BBB to include the new files
- Compile project 
- For more information, read 'Building src/' and 'Running src/'

 