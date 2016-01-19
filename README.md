# Isolated I/O Stream
Separated stdout/stdin/stderr on multithreaded function call idea

The idea is to create virtual stdout[/stdin/stderr]s on a specific function call and fetch/push data transparently.
For instance you want to wrap an app or function, in your multi-threaded application as a black box

# build
Run ```make``` in project dir
