#ifndef __DESCRIPTORS_HPP__
#define __DESCRIPTORS_HPP__

#include <linux/usb/ch9.h>
#include <stddef.h>
#include "usbstring.h"

#define USB_VENDOR_ID       0xfff0
#define USB_PRODUCT_ID      0x8009
#define USB_DEV_NAME        "Headset_#1"

enum {
    STRINGID_MANUFACTURER   = 1,
    STRINGID_PRODUCT           ,
    STRINGID_SERIAL            ,
    STRINGID_CONFIG_HS         ,
    STRINGID_CONFIG_LS         ,
    STRINGID_INTERFACE         ,
    STRINGID_MAX
};

#define MAX_USB_PCKG_SIZE   512
#define CONFIG_VALUE        1


extern struct usb_gadget_strings       strings;

extern struct usb_config_descriptor    config;
extern struct usb_config_descriptor    config_hs;
extern struct usb_interface_descriptor interface_descriptor;
extern struct usb_endpoint_descriptor  to_hst_ep_descriptor;
extern struct usb_endpoint_descriptor  to_dev_ep_descriptor;
extern struct usb_device_descriptor    device_descriptor;

#endif
