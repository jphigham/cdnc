# cdnc

### Windows:

```
Open Developer Command Prompt for VS 2019
> git clone <url> --recurse-submodules
> mkdir <path_to_build_dir>
> cd <path_to_build_dir>
> cmake -G "NMake Makefiles" <path_to_source_dir> -DCMAKE_BUILD_TYPE:STRING=Debug
> nmake
> cdnc.exe
```

### Linux:

```
$ git clone <url> --recurse-submodules
$ mkdir <path_to_build_dir>
$ cd <path_to_build_dir>
$ cmake -G "Unix Makefiles" <path_to_source_dir> -DCMAKE_BUILD_TYPE:STRING=Debug
$ make
$ cdnc
```