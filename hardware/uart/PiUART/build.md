
sudo apt install cmake python3 build-essential gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib


# Note: currently using this version of Circle
$ git log -n 1
commit c21f2efdad86c1062f255fbf891135a2a356713e (HEAD -> master, tag: Step45.3.1, origin/master, origin/HEAD)
Merge: 82cc4a3 f54fde2
Author: Rene Stange <rsta2@o2online.de>
Date:   Sun Oct 8 11:19:19 2023 +0200

    Merge branch 'hotfix-45.3.1'

#
#

$ cd solo86/hardware/uart/PiUART

# grab Circle from github. We need the git version so the WLAN modules will build
$ git submodule init
$ git submodule update

# build Circle
$ cp src/Config.mk circle
$ cd circle
circle$ ./makeall clean
circle$ ./makeall

# build OLED display library
circle$ cd addon/display/
circle/addon/display$ make clean
circle/addon/display$ make

# build FATfs (needed by WLAN for it's config file)
circle$ cd addon/fatfs/
circle/addon/fatfs$ make clean
circle/addon/fatfs$ make

# build SDCard (needed by WLAN for it's config file)
circle$ cd addon/SDCard/
circle/addon/SDCard$ make clean
circle/addon/SDCard$ make

# build WLAN library
circle$ git submodule update --init addon/wlan/hostap
circle$ cd addon/wlan/
circle/addon/wlan$ ./makeall clean
circle/addon/wlan$ ./makeall


