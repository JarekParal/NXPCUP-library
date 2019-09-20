# NXPCUP-library [![Build Status](https://travis-ci.org/JarekParal/NXPCUP-library.svg?branch=master)](https://travis-ci.org/JarekParal/NXPCUP-library)
Library for car's control board on [NXP-CUP competition](https://community.nxp.com/groups/tfc-emea) based on the [Mbed framework](https://mbed.com).

[Doxygen documentation](https://jarekparal.github.io/NXPCUP-library/annotated.html).

Actually tested on [Alamak](https://nxp.gitbook.io/nxp-cup-hardware-reference-alamak/kit-contents) car with [FRDM-KL25Z](https://os.mbed.com/platforms/KL25Z/) (mbed-os2, mbed-os5) and [FRDM-K66F](https://os.mbed.com/platforms/FRDM-K66F) (mbed-os5).

## Supported peripheries

- motors
- servos
- cameras
- buttons
- encoders 

## Support classes

- Image - class for working with data from sensors 
- BorderDetector - detector of the road
- MotorControl - PI regulator for motors
- ObstacleDetector - obstacle detection and path modification

## Code style

This library has [WebKit code style](https://webkit.org/code-style-guidelines/).
When you want to send some improvement through pull-request, please check that your changes don't violate the WebKit code style.
You can use program [clang-format](https://clang.llvm.org/docs/ClangFormat.html) with parameter `-style=webkit` for checking or correction the format.

Implace edit: `clang-format -style=webkit -i src/*`
