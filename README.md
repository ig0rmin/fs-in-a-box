Library consists of three main parts:

* libfsbox - cross-platform library, that implements virtual FS inside a file. This code is intended to be linked as a static library.
* fsboxmscom - Windows specific COM object, that exposes libfsbox facilities in a form of COM interfaces.
* fsboxpcom - XPCOM object. XPCOM is a portable implementation of COM from Mozilla. It can seamlessly work under Linux and used to access libfsbox on Linux.

THE LIBRARY IS IN THE PROCESS OF ACTIVE DEVELOPMENT
