# ServerMoniter
a simple server program that deliver server information through http protocol according to client's request


Steps:
  1)All that is needed to build the source is by typing the make command and ENTER.
  2)Execute the server by typing ./server
  3)Click the browser & then enter the address eg:127.0.0.1:8080/processes
  
ps: 
  this server only support the GET command & allows to request limited server information 
  such as process & time & disk information
  
  this server use the epoll IO multiplexing and a threadpool to implement.
