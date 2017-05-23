# roadsign

Code for the MakeHaven roadsign control project. Code inside the `portal/` folder is for the front end. The image conversion and sign update procedure will follow the method laid out in `Image Upload.ipynb`. It works more or less like this:

1. A user uploads an image or enters text in the front end site.
2. The server runs a Python program to create a black-and-white version of the image or render the text as an image (text processing program not yet written).
3. The Python code stores the created image as a vector of 1's and 0's defined in a C header file `buf.h`.
4. The Python code then calls the Particle API to compile and update the Arduino source code of the Particle Electron controlling the sign. The source code it sends is a simple "boilerplate" code like `base.ino` or `scroll.ino`. The image to display is "smuggled in" by having the code `#include` the `buf.h` file.
5. The Arduino code handles the wacky bit shift conventions necessary to properly address the sign itself, meaning the Python code can export the image in a normal, left-to-right, top-to-bottom order.

Currently, the code is somewhat opaque and not well documented. My first goal in this fork is to improve that somewhat. Next I'm planning to try to write a program to process text inputs.
