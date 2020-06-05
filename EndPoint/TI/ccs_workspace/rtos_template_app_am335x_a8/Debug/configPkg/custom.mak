## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,a8fg linker.cmd package/cfg/main_pa8fg.oa8fg

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/main_pa8fg.xdl
	$(SED) 's"^\"\(package/cfg/main_pa8fgcfg.cmd\)\"$""\"/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/ccs_workspace/rtos_template_app_am335x_a8/Debug/configPkg/\1\""' package/cfg/main_pa8fg.xdl > $@
	-$(SETDATE) -r:max package/cfg/main_pa8fg.h compiler.opt compiler.opt.defs
