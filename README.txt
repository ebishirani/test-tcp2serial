To build this module please follow next steps:
1- Clone and build linuxserialport from https://github.com/ebishirani/linuxserialport.git
2- Put created shared object from previous step in /usr/local/lib/mylib directory
3- Put header files of linuxserialport in /usr/local/include/linuxserialport
4- Clone and build tcpclientserver from https://github.com/ebishirani/tcpclientserver.git
5- Put created shared object from previous step in /usr/local/lib/mylib directory
6- Put header files of tcpclientserver in /usr/local/include/tcpclientserver
7- Make a directory name build in top level dir of this project.
2- Change current directory to build one.(cd ./build)
3- Run this command: cmake ../
4- Run this command: make
