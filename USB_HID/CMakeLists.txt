cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)

project(USB_HID C CXX ASM)
pico_sdk_init()

add_executable(USB_HID 
    USB_HID.c 
    usb_descriptors.c
)

pico_set_program_name(USB_HID "USB_HID")
pico_set_program_version(USB_HID "0.1")

pico_enable_stdio_uart(USB_HID 1)
pico_enable_stdio_usb(USB_HID 0) # TinyUSBと競合するため、標準のUSB出力はOFF

target_link_libraries(USB_HID
    pico_stdlib
    tinyusb_device
    tinyusb_board
)

target_include_directories(USB_HID PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(USB_HID)