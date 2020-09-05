# watchdog
A project in c.

I wrote a watchdog application. A watchdog is a process used to protect another process from specific software or hardware failures that may cause the users app to stop responding. The application is first registered with the watchdog device. Once the watchdog is running on your app the process must periodically send information to the watchdog process. If the process doesn't receive a fixed amount of signals within the set period of time it would execute the proper keystrokes restart the application. 
Moreover if the watchdog failes the users app will restart the watchdog too so this way both apps protect each other.
