TestTaskCOMShellExtension
=========================

COM component, that can be registered in MS Windows 7/8 64-bit:

Implemented:

1. It should add menu item (example, "Calculate the Sum") in Windows Explorer’s context menu (right click on the list of files, folders should be ignored).

2. This menu item should write information about selected files into the log file.

3. Component should work on huge amout of selected files.

4. Each line in the log file should represents: short filename, size in human readable form, data of creation in human readable form.

5. Lines should be sorted by file names in alphabetical order.
