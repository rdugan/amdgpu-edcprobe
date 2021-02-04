amdgpu-edcprobe - Read EDC counters for AMD GPUs
================================================

A tool to read the EDC (Error Detection and Correction) counters on AMD GPUs in Linux.

Requirements
============

Currently supports only MC (memory controller) EDC counters on the Navi10 architecture.
Additional domains and/or architectures may be added in the future.

#. AMD Navi10 GPU
#. amdgpu driver >= 19.30*

:sub:`* for earlier drivers, check that /sys/kernel/debug/dri/<card_num>/amdgpu_regs is available`


Building from source
====================

Requirements
^^^^^^^^^^^^

#. Git
#. GNU Make >= 4.2.1
#. GNU ar >= 2.3.4
#. GCC >= 9.3.0

:sub:`* earlier versions make work without issue - these are simply the versions tested`


Steps
^^^^^

#. Download the source::

    git clone https://github.com/rdugan/amdgpu-edcprobe.git

#. Change into the newly created directory::

    cd amdgpu-edcprobe

#. Run make to build the executable::

    make -f Makefile


Usage
=====

amdgpu-edcprobe is a command line utility which must be run as root.

::

    amdgpu-edcprobe [-v?V] [-m[MEM_ERRS_TYPE]] [--mem-errs[=MEM_ERRS_TYPE]]
                    [--verbose] [--help] [--usage] [--version] AMD_GPU_INDEX
  
Running with no options will show memory errors for the given GPU, split into read and write
summary counts (equivalent to the --mem-errs=b option).


Options
=======

::

    -m, --mem-errs[=MEM_ERRS_TYPE]   display memory errors of TYPE r(ead),
                                     w(rite), b(oth) [DEFAULT], or t(otal)
    -v, --verbose                    show counters by individual register
    -?, --help                       Give this help list
        --usage                      Give a short usage message
    -V, --version                    Print program version


Known Issues
============

Tool currently only reads channel 1 counters, as reference tool from AMD (atitool) always shows 
identical values for channel 2.  If this behavior is correct, then true error counts may be doubled. 


Contributors
============

+-----------------+---------------------------+
| |rdugan_avatar| | rdugan                    |
|                 | https://github.com/rdugan |
+-----------------+---------------------------+

.. |rdugan_avatar| image:: https://avatars.githubusercontent.com/u/1779672?s=60&u=8c19fd7f743b0c0e0762c2ce5e9841b084b7dcdb&v=4


Credits
=======

Initial code based on `AMDTempThing <https://github.com/OhGodAPet/AMDTempThing>`_ by Wolf0
