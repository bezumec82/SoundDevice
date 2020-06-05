#include "descriptors.h"

struct usb_string stringtab [] =
{
    { STRINGID_MANUFACTURER,"microElk",                 },
    { STRINGID_PRODUCT,     USB_DEV_NAME,               },
    { STRINGID_SERIAL,      "0001",                     },
    { STRINGID_CONFIG_HS,   "High speed configuration", },
    { STRINGID_CONFIG_LS,   "Low speed configuration",  },
    { STRINGID_INTERFACE,   "Custom interface",         },
    { STRINGID_MAX,         NULL                        },
};

struct usb_gadget_strings strings =
{
    .language               = 0x0409, /* en-us */
    .strings                = stringtab,
};

struct usb_config_descriptor config =
{
    .bLength                = USB_DT_CONFIG_SIZE,
    .bDescriptorType        = USB_DT_CONFIG,
    /*------------------------------------------------------------------------*/
    .wTotalLength           = USB_DT_CONFIG_SIZE
                            + USB_DT_INTERFACE_SIZE
                            + USB_DT_ENDPOINT_SIZE
                            + USB_DT_ENDPOINT_SIZE,
    .bNumInterfaces         = 1,
    .bConfigurationValue    = CONFIG_VALUE,
    .iConfiguration         = 0,
    .bmAttributes           = USB_CONFIG_ATT_ONE,
    .bMaxPower              = 250,
};
struct usb_config_descriptor config_hs =
{
    .bLength                = USB_DT_CONFIG_SIZE,
    .bDescriptorType        = USB_DT_CONFIG,
    /*------------------------------------------------------------------------*/
    .wTotalLength           = USB_DT_CONFIG_SIZE
                            + USB_DT_INTERFACE_SIZE
                            + USB_DT_ENDPOINT_SIZE
                            + USB_DT_ENDPOINT_SIZE,
    .bNumInterfaces         = 1,
    .bConfigurationValue    = CONFIG_VALUE,
    .iConfiguration         = 0,
    .bmAttributes           = USB_CONFIG_ATT_ONE,
    .bMaxPower              = 250,
};
struct usb_interface_descriptor interface_descriptor =
{
    .bLength                = USB_DT_INTERFACE_SIZE,
    .bDescriptorType        = USB_DT_INTERFACE,
    /*------------------------------------------------------------------------*/
    .bInterfaceNumber       = 0,
    .bAlternateSetting      = 0,
    .bNumEndpoints          = 2,
    .bInterfaceClass        = USB_CLASS_VENDOR_SPEC,
    .bInterfaceSubClass     = USB_SUBCLASS_VENDOR_SPEC,
    .bInterfaceProtocol     = 0xff,
    .iInterface             = 2,
};

struct usb_endpoint_descriptor to_hst_ep_descriptor =
{
     .bLength               = USB_DT_ENDPOINT_SIZE,
     .bDescriptorType       = USB_DT_ENDPOINT,
     /*-----------------------------------------------------------------------*/
     .bEndpointAddress      = USB_DIR_IN | 1, /* to host */
     .bmAttributes          = USB_ENDPOINT_XFER_BULK,
     .wMaxPacketSize        = __constant_cpu_to_le16 (MAX_USB_PCKG_SIZE),
     .bInterval             = 0,

     .bRefresh = 0,
     .bSynchAddress = 0
};
struct usb_endpoint_descriptor to_dev_ep_descriptor =
{
    .bLength                = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType        = USB_DT_ENDPOINT,
    /*------------------------------------------------------------------------*/
    .bEndpointAddress       = USB_DIR_OUT | 2, /* to device */
    .bmAttributes           = USB_ENDPOINT_XFER_BULK,
    .wMaxPacketSize         = __constant_cpu_to_le16 (MAX_USB_PCKG_SIZE),
    .bInterval              = 0,

    .bRefresh = 0,
    .bSynchAddress = 0
};
struct usb_device_descriptor device_descriptor =
{
     .bLength               = USB_DT_DEVICE_SIZE,
     .bDescriptorType       = USB_DT_DEVICE,
     /*-----------------------------------------------------------------------*/
     .bcdUSB                = 0,
     .bDeviceClass          = USB_CLASS_PER_INTERFACE,
     .bDeviceSubClass       = 0,
     .bDeviceProtocol       = 0,
     .bMaxPacketSize0       = 0xff, /* Set by driver */
     .idVendor              = USB_VENDOR_ID,
     .idProduct             = USB_PRODUCT_ID,
     .bcdDevice             = 0x0400,
     // Strings
     .iManufacturer         = STRINGID_MANUFACTURER,
     .iProduct              = STRINGID_PRODUCT,
     .iSerialNumber         = STRINGID_SERIAL,
     .bNumConfigurations    = 1 /* Only one configuration */
};

