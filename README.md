Repository for Astro-Accelerate Code
====

Introduction
===
This documentation summarises the content of the Astro-Accelerate software.

Features
===
Astro-Accelerate is used for real-time astronomy data processing. Its features include:
* Acceleration Searching
* Zero DM
* RFI Mitigation

Software Inputs and Outputs
===
The software input is a sample data file.
To process the data file, astro-accelerate makes use of a configuration file.
Please see the section `Creating An Input Configuration File` for instructions on how to create a configuration file to process an input data file.
The software output is dependent on the choice of analysis module that is run.

Checking the Configuration of the Graphics Processing Unit (GPU) and Support for CUDA

In a terminal window, type

    nvidia-smi

The output will look similar to the following example
```
+-----------------------------------------------------------------------------+
| NVIDIA-SMI 375.20                 Driver Version: 375.20                    |
|-------------------------------+----------------------+----------------------+
| GPU  Name        Persistence-M| Bus-Id        Disp.A | Volatile Uncorr. ECC |
| Fan  Temp  Perf  Pwr:Usage/Cap|         Memory-Usage | GPU-Util  Compute M. |
|===============================+======================+======================|
|   0  Tesla P100-PCIE...  On   | 0000:02:00.0     Off |                    0 |
| N/A   32C    P0    26W / 250W |      0MiB / 16308MiB |      0%      Default |
+-------------------------------+----------------------+----------------------+
|   1  Tesla P100-PCIE...  On   | 0000:03:00.0     Off |                    0 |
| N/A   34C    P0    25W / 250W |      0MiB / 16308MiB |      0%      Default |
+-------------------------------+----------------------+----------------------+
                                                                              
+-----------------------------------------------------------------------------+
| Processes:                                                       GPU Memory |
|  GPU       PID  Type  Process name                               Usage      |
|=============================================================================|
|  No running processes found                                                 |
+-----------------------------------------------------------------------------+
```

If no output is shown, or if an error appears, then it may indicate that a GPU has not been detected,
or that the [CUDA toolkit](https://developer.nvidia.com/cuda-zone) is not properly installed.

Selecting the Graphics Processing Unit (GPU) in the case of a system with more than one GPU
===
If you have a multi-GPU system, you need to modify two files to use one specific card.

1. Edit `include/params.h`, so that the `CARD` variable is set to the right card number id:
    #define CARD ... 
2. Edit the Makefile and set the `GENCODE_FLAG` to the correct architecture. For example:
    GENCODE_FLAGS := $(GENCODE_SM61)
    Where `GENCODE_SM61` corresponds ot the Pascal architecture.

Software Pre-Requisites
===
CUDA: CUDA 8.0 (see https://developer.nvidia.com/cuda-downloads)

C/C++ (version): As supported and required by CUDA.

Compiler: As supported by CUDA, but requiring also [OpenMP](https://www.openmp.org) support (compiler support can be found [here](https://www.openmp.org/resources/openmp-compilers-tools/)).

Creating An Input Configuration File
===
An example input configuration file is shown below:
 
    range    0    150  0.1  1 1
    range    150  300  0.2  1 1
    range    300  500  0.25 1 1
    range    500  900  0.4  2 2
    range    900  1200 0.6  4 4
    range    1200 1500 0.8  4 4
    range    1500 2000 1.0  4 4
    range    2000 3000 2.0  8 8
    sigma_cutoff     6
    analysis
    -acceleration
    -periodicity
    -output_dmt
    -zero_dm
    -zero_dm_with_outliers
    -rfi
    fdas_custom_fft
    -fdas_inbin
    fdas_norm
    debug
    file /home/wa78/filterbank/ska-mid-b2.fil`
 
Features can be turned on or off by adding a character at the beginning of the line (here "-" is used).

Setting `range`
=
`range` tells the code to dedisperse and has input.

The format for the `range` parameter is
`range dm_low dm_high dm_step downsampling_in_input_time downsampling_in_output_time`
where `dm_low`, `dm_high`, `dm_step`, `downsampling_in_input_time`, and `downsampling_in_output_time` are to be replaced with suitable numerical values.
For example, a valid input for the `range` parameter is

    range 500  900  0.4  2 2
 
astro-accelerate will parse this input and dedisperse from a `dm` of `500` to a `dm` of `900` in steps of `0.4` (making (`900`-`500`)/`0.4` `dm` trials), with input data 
downsampled in time by `2` (e.g. 64 uS would be binned into 128 uS samples).
 
Setting `sigma_cutoff`
=
`sigma_cutoff` is the Signal-to-Noise Ratio (SNR) cutoff for your single pulse search.

Setting `analysis`
= 
`analysis` this tells the code to analyse the dedispersed data, outputting data into the output directory. The output data are binary files and so which can be read in gnuplot and python.

For example, you could use python as follows
    splot "./57663_22588_B0531+21_000007.fil/global_analysed_frb.dat" binary format="%float%float%float%float" u 1:2:3 palette
 
 Setting `acceleration`
 =
`acceleration` this does a Fourier domain acceleration search on the data.

Setting `periodicity`
=
`periodicity` sets a search for periodic objects.

Setting `output_dmt`
=
`output_dmt` outputs the entire dedispersed data to a file (in ASCII).

Setting `zero_dm`
= 
`zero_dm` you can guess

Setting `zero_dm_with_outliers`
=
`zero_dm_with_outliers` is part of the RFI mitigation routine.
 
Setting `rfi`
=
`rfi` tries to eliminate RFI.
(Astro-Accelerate welcomes developers to supply the team with data that includes RFI, which would be very helpful.)

Setting `fdas_custom_fft`
=
`fdas_custom_fft` runs fourier domain acceleration search with a custom FFT.

Setting `fdas_inbin`
=
`fdas_inbin` performs interbinning on the complex output.

Setting `fdas_norm`
=
`fdas_norm` performs PRESTO block median normalization.

Setting `debug`
`debug` this gives detailed output.
 
Setting `file`
Please supply the input data file by using the `file` setting followed by the path to a valid
input data file

    file <your input file>
 
Input data files are by definition assumed to be formatted with 8-bit data.
Please contact Astro-Accelerate if you have input data that is not 8-bits let me know (development for support for this is in progress and we may be able to help).

Installation and Usage (Linux)
===
Step 1: Download the files
==
Obtain the Astro-Accelerate code by doing
    git clone https://github.com/AstroAccelerateOrg/astro-accelerate

Step 2: Compile
==
Ensure you have the correct environment and pre-requisites.
Set-up the environment (which will add CUDA to PATH and LD_LIBRARY_PATH)

    source setup.sh

`setup.sh` contains a hardcoded version number and a variable string to identify
whether the system is a 64-bit or 32-bit architecture. The user may need to edit
`setup.sh` to suit the CUDA version number, library paths, and the architecture number
in order to suit their needs. Users who already have all relevant CUDA paths configured
do not need to source setup.sh.

At this point, the user has a choice, they can either 1.) use the pre-configured Makefile
that comes with the repository by default, or they can 2.) configure the build system
themselves using CMake.

Note that in the case of using CMake, the Makefile that CMake
produces will overwrite the default Makefile if the build is performed
in source.

To run using the default Makefile, simply type

    make

To configure the build system using CMake, create a `build` directory

    mkdir build

and then

    cd build/

run CMake

    cmake ../

The software can then be compiled using the generated Makefile. To do so, simply type

    make

In both cases, the compilation process indicates which components are being compiled.
The result is an executable called

    astro-accelerate

in the directory from which the build was performed.
In the case of using the default Makefile,
the library is compiled as a static library called

    libastroaccelerate.a
    
against which the executable is linked.
In the case of using CMake to configure the build system, the library is compiled
as a shared object library called

    libastroaccelerate.so


against which the executable is linked.
In both cases, the library file will be located in the astro-accelerate build directory.

Step 3: Run
==
Astro-Accelerate assumes its input is ready and compatible. To obtain compatible input, please follow the steps below. Please also ensure that the aforementioned environment variables have been set.

1. Run astro-accelerate using the format

    ./astro-accelerate --settingname settingvalue

By default, the output of astro-accelerate will be located in the same directory in which astro-accelerate was executed.
Configuration files may be used to further specify, set, and change options.
By default, the astro-accelerate executable looks for a configuration file in the same directory as the executable.
A configuration file is required in order to run astro-accelerate.
A number of example configuration files are included in the repository.

Optimisations and Tuning
==
Astro-Accelerate comes with the facility to tune the software to the input that the user provides.
1. To do this, `cd` to the `scripts` directory.
Modify `profiling.sh`, changing the line that says

        ./astro-accelerate.sh ../input_files/ska_tune.txt

    to

        ./astro-accelerate.sh ../input_files/<your input file>

    where `<your input file>` must be replaced by the input configuration file.

3. The next step is to run the profiler tool that is provided in the repository

        ./profiling.sh

    This will create an optimised code for your search and GPU type.

4. Then, astro-accelerate can be run as usual by doing 

        ./astro-accelerate.sh ../input_files/<your input file>

    where `<your input file>` must be replaced by the path to the input configuration file as specified in the previous step.

Further Documentation
===
More detailed information can be found on the [Wiki page of the repository](https://github.com/AstroAccelerateOrg/astro-accelerate/wiki) and the [Astro-Accelerate webpage](http://www.oerc.ox.ac.uk/projects/astroaccelerate).

Contact and Support
===
If you notice any errors or have suggestions,
please contact someone on the astro-accelerate team,
or file an issue or bug report on the repository.

Disclaimers and Licensing
===
A number of code files may be covered by different licences.
Please refer to these seperately.
Please also refer to the Astro-Accelerate licence file provided.

Copyright © 2018 Astro-Accelerate. All rights reserved.
