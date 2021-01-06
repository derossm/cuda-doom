# Plans
	* move all windows code to win32 files to limit header thrashing (found a windows #define was overriding my variable name HELP_KEY)
	* convert all structs with associated free functions into classes
	* switch to containers with allocators instead of raw malloc calls
	* ensure every file ends with a new line
