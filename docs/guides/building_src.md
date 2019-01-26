# Building src/

After adding new files to `src/` (review 'Adding new files to src/' if not done so), you must compile all source files into the project's binary. This is done by connecting to the BBB, running the make file, and running the `hyped` executable. 

#### Mounting to the BBB
1. On the command line, navigate to the same directory with `BeagleBone_black`
2. Create an empty directory called `mount`
3. Mount the BBB onto your computer through that new directory
4. Move a copy of the `BeagleBone_black` directory into the `mount` directory
```
$ mkdir mount
$ sshfs hyped@192.168.6.2: <insert_path_here>/mount
$ mv BeagleBone_black mount
```
- Note: 
    - For Mac: 192.168.6.2
    - For Windows/Linux: 192.168.7.2
- For more information visit, 'Using the BBB'

#### Compiling using Makefile
- *It is imperative that all new files are added to Source.files and Lint.files before compilation. Visit 'Adding new files to src/' for more information*

1. On the command line, SSH onto the BBB    
    - Visit 'Using the BBB' for more information
2. Navigate directories into `BeagleBone_black`
3. Run `make clean`
4. Run `make MAIN=<your_main_file>`
    - The project can be built using any main files in 'src' but defaults to `main.cpp`. Specify your main file here
5. If the project compiles without errors, with exception of lint errors, run `./hyped` to run the `hyped` executable created from compilation
```
hyped@beaglebone $ cd BeagleBone_black
hyped@beaglebone $ make clean
hyped@beaglebone $ make MAIN=<your_main_file>
hyped@beaglebone $ ./hyped
```

#### Things to remember
- If you want to ignore a lint error for particular line in your source code, add `// NOLINT [rule]` at the end of the line, e.g. `// NOLINT [whitespace/line_length]`. [rule] part is optional, it selects which lint rule checking should be skipped. If no rule is provided, all lint rules will be skipped.
- Use cross-compiler if available. See 'Cross-compiler' for more information
- For more information, read 'Running src/'

 