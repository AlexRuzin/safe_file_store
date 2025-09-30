# Safe File Store

This application is a replacement for `vim`'s encryption method. 

## Function

 Create a new file using the `store` command, using a user-supplied password. The password will encrypt the file and write it to the disk
* Load files using the `store` command, the file will require a password to open and edit
* Write text to the file and save

## Features

* Platform-independent using non-native APIs (STL only)
* Strong/modern encryption of files
    * Passwords are hashed using SHA256
    * Files encrypted using AES256-CBC
    * File integrity checksums
* Simple vim-like interface [todo]
