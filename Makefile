
#-------------------------------------------------------------------------------
# Configure variable
#-------------------------------------------------------------------------------
TOP_DIR:=$(TOP_DIR)
ROOT_DIR:=$(TOP_DIR)/../..
MAKEFILE_PATH :=$(subst $(strip \),/,$(abspath $(lastword $(MAKEFILE_LIST))))


#-------------------------------------------------------------------------------
# Configure source code files
#-------------------------------------------------------------------------------
SRC_FILES+= gpio.c lcd.c tp69_main.c 

#-------------------------------------------------------------------------------
# the include directories
#-------------------------------------------------------------------------------
INC_DIRS+= \
  -I${TOP_DIR}/ \
  -I${TOP_DIR}/interface/init/inc \
  -I${TOP_DIR}/common/include \
  -I${TOP_DIR}/interface/os/inc \
  -I${TOP_DIR}/interface/driver/inc \
  -I${TOP_DIR}/interface/sim/inc \
  -I${TOP_DIR}/interface/network/nw/inc \
  -I${TOP_DIR}/interface/network/sockets/inc \
  -I${TOP_DIR}/interface/fs/inc \
  -I${TOP_DIR}/interface/dev/inc \
  -I${TOP_DIR}/interface/time/inc \
  -I${TOP_DIR}/interface/timer/inc \
  -I${TOP_DIR}/interface/lcd/inc \
  -I${TOP_DIR}/interface/platform/inc \
  -I${TOP_DIR}/interface/multimedia\inc \
  -I${TOP_DIR}/interface/fota/inc \
  -I${TOP_DIR}/interface/ssl/inc \
  -I${TOP_DIR}/interface/tp69 \

#-------------------------------------------------------------------------------
# Configure compile flag
#-------------------------------------------------------------------------------
U_FLAGS:=

#-------------------------------------------------------------------------------
# Configure default macro define
#-------------------------------------------------------------------------------
DFLAGS+= \
  -D_REENT_ONLY \
  -D__OCPU_COMPILER_GCC__ \
  -D_WANT_USE_LONG_TIME_T \
  -D LANGUAGE_SETTING_CH \
  -D REMOVE_TTS_AND_SYNC_CHARGE \

#-------------------------------------------------------------------------------
# include mk file
#-------------------------------------------------------------------------------
include ${TOP_DIR}\config\common\makefile.mk
