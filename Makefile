PROJECT_NAME = miata-gate

PROD_ENV = uno_r4_wifi
DEV_ENV = uno_r4_wifi__with_debug
PORT ?= /dev/cu.usbmodemDC5475EA051C2

ifdef BOARD_DEBUG
    ifeq ($(BOARD_DEBUG), true)
        ENVIRONMENT = $(DEV_ENV)
    else
        ENVIRONMENT = $(PROD_ENV)
    endif
else
    ENVIRONMENT = $(PROD_ENV)
endif


PIO = pio

.PHONY: all build debug upload clean

all: build

build:
	$(PIO) run -e $(ENVIRONMENT)

upload:
	$(PIO) run -e $(ENVIRONMENT) --target upload

clean:
	$(PIO) run -e $(ENVIRONMENT) --target clean

check_device:
	$(PIO) device list

monitor:
	$(PIO) device monitor -p $(PORT)