To build and run the project, you'll need to install the required dependencies first:

sudo apt-get install libgtk-3-dev libpoppler-glib-dev cmake build-essential

Then build the project:

mkdir build
cd build
cmake ..
make

Run the PDF viewer:

./pdf-viewer

This implementation includes the following features :

Open PDF files
Navigate between pages
Zoom in/out
Scrollable view
Basic toolbar with controls

To do next : 

Bookmarks
Search functionality
Thumbnails view
Print support
Document properties
Text selection
Annotations
