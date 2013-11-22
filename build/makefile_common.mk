EMU = ../sources/src
CPU = $(EMU)/uae-cpu
FALCON = $(EMU)/falcon
DBG = $(EMU)/debug
FLP = $(EMU)
GUI = $(EMU)/gui-retro
ALTEMU = ../sources/altsrc
LIBCOOBJ = ../sources/utils/libco
LIBUTILS =../sources/utils

CPUC_SRCS = \
$(CPU)/cpudefs.o \
$(CPU)/cpuemu.o \
$(CPU)/cpustbl.o \
$(CPU)/hatari-glue.o \
$(CPU)/memory.o \
$(CPU)/newcpu.o \
$(CPU)/readcpu.o \
$(CPU)/fpp.o

DLG_SRCS = \
$(GUI)/dlgAbout.o \
$(GUI)/dlgAlert.o \
$(GUI)/dlgDevice.o \
$(GUI)/dlgFileSelect.o \
$(GUI)/dlgFloppy.o \
$(GUI)/dlgHardDisk.o \
$(GUI)/dlgJoystick.o \
$(GUI)/dlgKeyboard.o \
$(GUI)/dlgMain.o \
$(GUI)/dlgMemory.o \
$(GUI)/dlgNewDisk.o \
$(GUI)/dlgRom.o \
$(GUI)/dlgScreen.o \
$(GUI)/dlgSound.o \
$(GUI)/dlgSystem.o \
$(GUI)/sdlgui.o

FALC_SRCS = \
$(FALCON)/crossbar.o \
$(FALCON)/dsp.o \
$(FALCON)/dsp_core.o \
$(FALCON)/dsp_cpu.o \
$(FALCON)/dsp_disasm.o \
$(FALCON)/hostscreen.o \
$(FALCON)/microphone.o \
$(FALCON)/nvram.o \
$(FALCON)/videl.o

DBG_SRCS = \
$(DBG)/log.o \
$(DBG)/debugui.o \
$(DBG)/breakcond.o \
$(DBG)/debugcpu.o \
$(DBG)/debugInfo.o \
$(DBG)/debugdsp.o \
$(DBG)/evaluate.o \
$(DBG)/history.o \
$(DBG)/symbols.o \
$(DBG)/profile.o \
$(DBG)/profilecpu.o \
$(DBG)/profiledsp.o \
$(DBG)/natfeats.o \
$(DBG)/console.o \
$(DBG)/68kDisass.o

FLP_SRCS = \
$(FLP)/createBlankImage.o \
$(FLP)/dim.o \
$(FLP)/msa.o \
$(FLP)/st.o \
$(FLP)/zip.o

CORE_SRCS = \
$(EMU)/acia.o \
$(EMU)/audio.o \
$(ALTEMU)/avi_record.o \
$(EMU)/bios.o \
$(EMU)/blitter.o \
$(EMU)/cart.o \
$(EMU)/cfgopts.o \
$(EMU)/clocks_timings.o \
$(EMU)/configuration.o \
$(EMU)/options.o \
$(EMU)/change.o \
$(EMU)/control.o \
$(EMU)/cycInt.o \
$(EMU)/cycles.o \
$(EMU)/dialog.o \
$(EMU)/dmaSnd.o \
$(EMU)/fdc.o \
$(EMU)/file.o \
$(EMU)/floppy.o \
$(EMU)/gemdos.o \
$(EMU)/hd6301_cpu.o \
$(EMU)/hdc.o \
$(EMU)/ide.o \
$(EMU)/ikbd.o \
$(EMU)/ioMem.o \
$(EMU)/ioMemTabST.o \
$(EMU)/ioMemTabSTE.o \
$(EMU)/ioMemTabTT.o \
$(EMU)/ioMemTabFalcon.o \
$(EMU)/joy.o \
$(EMU)/keymap.o \
$(EMU)/m68000.o \
$(EMU)/main.o \
$(EMU)/midi.o \
$(EMU)/memorySnapShot.o \
$(EMU)/mfp.o \
$(EMU)/paths.o \
$(EMU)/psg.o \
$(EMU)/printer.o \
$(EMU)/resolution.o \
$(EMU)/rs232.o \
$(EMU)/reset.o \
$(EMU)/rtc.o \
$(EMU)/scandir.o \
$(EMU)/stMemory.o \
$(EMU)/screen.o \
$(EMU)/screenSnapShot.o \
$(EMU)/shortcut.o \
$(EMU)/sound.o \
$(EMU)/spec512.o \
$(EMU)/statusbar.o \
$(EMU)/str.o \
$(EMU)/tos.o \
$(EMU)/unzip.o \
$(EMU)/utils.o \
$(EMU)/vdi.o \
$(EMU)/video.o \
$(EMU)/wavFormat.o \
$(EMU)/xbios.o \
$(EMU)/ymFormat.o \
$(ALTEMU)/bmp.o

LIBCO_SRCS = $(LIBCOOBJ)/libco.o 
ifeq ($(platform),android)
LIBCO_SRCS += $(LIBCOOBJ)/armeabi_asm.o
endif

BUILD_APP =  $(ZLIB_OBJECTS) $(CPUC_SRCS) $(FALC_SRCS)  $(FLP_SRCS) $(DBG_SRCS)  $(CORE_SRCS) $(DLG_SRCS) $(LIBCO_SRCS)

HINCLUDES := -I./$(EMU)  -I./$(CPU)  -I./$(FALCON)  \
	-I./$(EMU)/includes -I./$(DBG) -I./$(FLP) -I$(LIBRETRO) -I$(LIBUTILS)


OBJECTS := $(LIBRETRO)/libretro-hatari.o $(LIBRETRO)/hatari-mapper.o $(LIBRETRO)/vkbd.o \
	$(LIBRETRO)/graph.o $(LIBRETRO)/fontmsx.o \
	$(BUILD_APP)


