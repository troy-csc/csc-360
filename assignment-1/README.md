Tarush Roy
V00883469

=============================================================================================================

NOTE ------

INCOMPLETE ASSIGNMENT. NOT WORKING.

Final Grade: 59%

=============================================================================================================

Resources used:
1. https://brennan.io/2015/01/16/write-a-shell-in-c/
   * this helped me understand the basic layout of the shell as well as helped me with various functions.

2. linux man pages
   * helped with syntax of c library commands

3. stackoverflow
   * helped with the many seg faults and errors
   * some syntax help

Other comments and concerns:
1. in its current stage, SEEsh can only handle a limited number of commands (about 9 in my testing) before it seg faults. (NO IDEA WHY) It doesn't seg fault while valgrind is running so i can't seem to figure out why. The seg fault seems to happen in a malloc.

2. sometimes ctrl-d does not work (i dont know why) but ctrl-c works to exit the shell

3. there are definitely a few memory leaks but i don't know where they are. there seems to be a free for each malloc in my code but valgrind says otherwise
