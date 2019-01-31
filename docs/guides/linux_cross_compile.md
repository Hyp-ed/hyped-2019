# Linux Cross Compile

1.  Download the cross compilation tools from [here](https://drive.google.com/file/d/1BabuQGy5jR1iEFaZxb80RC5DiTmvyEmi/view?usp=sharing) to anywhere
2.  Unzip the file into folder `arm-4.9.3-linux-gnueabihf`
3.  `cd arm-4.9.3-linux-gnueabihf/bin/`
4.  `ln -s arm-linux-gnueabihf-g++ hyped-cross-g++`
6.  ``echo "export PATH=\$PATH:`pwd`" >> ~/.bashrc``
7.  `. ~/.bashrc`

## Try it out
Check location of the cross-compiler; should not be empty output

`which hyped-cross-g++`

Check you can actually run the cross-compiler; should be something reasonable

`hyped-cross-g++ --version`

Check you can compile and run files

Copy to your HOST terminal
```
echo $'
#include <cstdio>
int main () {
    printf("hello hyped\\n");
    return 0;
}' > hello.cpp
```
Run

`hyped-cross-g++ -o main hello.cpp`

If compilation succeeded copy binary `main` onto BBB and try to execute it. The expected output is:
```
hyped@beaglebone:~$ ./main 
hello hyped
```

## Run Cross-compilation
To enable cross compilation you only need to add `CROSS=1` command when running your typical HYPED Makefile, e.g.:

`[Host] make MAIN=demo_imu.cpp CROSS=1 -j`

Note, you can run the cross-compilation using your laptop, which is much faster than compiling on BBB. Furthermore, you can utilise parallel compilation using `-j` option. To limit the parallelism to a particular number, you can use `-j3`. This would create only 3 parallel jobs at a time.
