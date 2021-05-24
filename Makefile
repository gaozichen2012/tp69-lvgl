LVGL_DIR ?= ${TOP_DIR}/interface/tp69
LVGL_DIR_NAME ?= lvgl
include lvgl.mk

#-------------------------------------------------------------------------------
# Configure variable
#-------------------------------------------------------------------------------
TOP_DIR:=$(TOP_DIR)
ROOT_DIR:=$(TOP_DIR)/../..
MAKEFILE_PATH :=$(subst $(strip \),/,$(abspath $(lastword $(MAKEFILE_LIST))))


#-------------------------------------------------------------------------------
# Configure source code files
#-------------------------------------------------------------------------------
SRC_FILES+= font.c gpio.c lcd.c tp69_main.c \
            lvgl_init.c \
            $(CSRCS) \
            lv_tp69/src/lv_desktop.c \
            lv_tp69/src/lv_event.c \
            lv_tp69/src/lv_menu.c \
            lv_tp69/src/lv_menu_net.c \
            lv_tp69/src/lv_startup.c \
            lv_tp69/assets/freq_20x20.c \
            lv_examples/src/lv_demo_benchmark/lv_demo_benchmark.c \
            lv_examples/src/lv_demo_widgets/lv_demo_widgets.c \
            lv_examples/src/assets/img_cogwheel_alpha16.c \
            lv_examples/src/assets/img_cogwheel_argb.c \
            lv_examples/src/assets/img_cogwheel_chroma_keyed.c \
            lv_examples/src/assets/img_cogwheel_indexed16.c \
            lv_examples/src/assets/img_cogwheel_rgb.c \
            lv_examples/src/assets/img_hand.c \
            lv_examples/src/assets/lv_font_montserrat_12_compr_az.c \
            lv_examples/src/assets/lv_font_montserrat_16_compr_az.c \
            lv_examples/src/assets/lv_font_montserrat_28_compr_az.c \

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
  -I${TOP_DIR}/interface/tp69/lvgl \
  -I${TOP_DIR}/interface/tp69/lv_tp69 \
  -I${TOP_DIR}/interface/tp69/lv_examples \
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
