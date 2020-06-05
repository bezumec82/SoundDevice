curr := $(root)
root := ../$(root)
curr_lemzgw := $(root_lemzgw)
root_lemzgw := ../$(root_lemzgw)
include $(root)/root.mk
