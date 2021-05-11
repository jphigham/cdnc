# cdnc

### Windows source setup:

Clone source, copy data_api.h, create build dir:

```
Open Developer Command Prompt for VS 2019
> mkdir <path_to_source_dir>
> cd <path_to_source_dir>
> git clone <cdnc_repo_url> --recurse-submodules
> cd cdnc
> copy <path_to_data_api_header_download>/data_api.h .
> mkdir <path_to_build_dir>
> cd <path_to_build_dir>
```

Continue with either nmake or msbuild below

#### Windows nmake build:

Use nmake:

```
> cmake -G "NMake Makefiles" <path_to_source_dir>\cdnc -DCMAKE_BUILD_TYPE:STRING=[Debug|Release]
> nmake
> cdnc.exe
```

#### Windows msbuild:

Use msbuild:

```
> cmake <path_to_source_dir>\cndc -DCMAKE_BUILD_TYPE:STRING=[Debug|Release]
> msbuild ALL_BUILD.vcxproj /property:Configuration=[Debug|Release]
> [Debug|Release]\cdnc.exe
```

### Linux:

```
$ mkdir <path_to_source_dir>
$ cd <path_to_source_dir>
$ git clone <cdnc_repo_url> --recurse-submodules
$ cd cdnc
$ cp <path_to_data_api_header_download>/data_api.h .
$ mkdir <path_to_build_dir>
$ cd <path_to_build_dir>
$ env CC=clang CXX=clang++ cmake -G "Unix Makefiles" <path_to_source_dir>/cdnc -DCMAKE_BUILD_TYPE:STRING=[Debug|Release]
$ make [-jN]
$ ./cdnc
```