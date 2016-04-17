# nginx
Nginx war zone

The following is a directory tree of this project:


    build.sh                // script to test modules
    LICENSE                
    modules                 // root dir of all modules
    --$module               // root dir of $module
      --config              // config of $module
      --src                 // src dir of $module
        --*.c               // c files of $module
    --$module_nginx.conf    // test conf of $module
    README.md
