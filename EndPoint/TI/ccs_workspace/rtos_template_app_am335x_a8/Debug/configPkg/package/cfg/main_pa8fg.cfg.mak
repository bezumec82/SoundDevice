# invoke SourceDir generated makefile for main.pa8fg
main.pa8fg: .libraries,main.pa8fg
.libraries,main.pa8fg: package/cfg/main_pa8fg.xdl
	$(MAKE) -f /media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/ccs_workspace/rtos_template_app_am335x_a8/src/makefile.libs

clean::
	$(MAKE) -f /media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/ccs_workspace/rtos_template_app_am335x_a8/src/makefile.libs clean

