TestTaskCOMShellExtension
=========================

COM component, that can be registered in MS Windows 7/8 64-bit:

Implemented:

1. It add menu item "Calculate the Sum" in Windows Explorer’s context menu (right click on the list of files, folders should be ignored).

2. Menu item write information about selected files into the log file (name of log file is random generated when user uses menu item).

3. Component work on huge amout of selected files.

4. Each line in the log file should represents: short filename, size in human readable form, data of creation in human readable form and per-byte-sum.

5. Lines sorted by file names in alphabetical order.

6. The calculation of checksum should be done using concurrency (thread pool).

7. Calculated information for each file logged as soon as possible. 
