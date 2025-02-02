NAME= yirl

LIBNAME= lib$(NAME)

all:	yirl-loader build-tests

include config.mk

include $(TESTS_DIR)/tests.mk

SRC = 	$(SCRIPT_DIR)/lua-script.c \
	$(SCRIPT_DIR)/entity-script.c \
	$(SCRIPT_DIR)/lua-binding.c \
	$(SCRIPT_DIR)/lua-convert.c \
	$(SCRIPT_DIR)/tcc-script.c \
	$(SCRIPT_DIR)/tcc-syms.c \
	$(SCRIPT_DIR)/native-script.c \
	$(SCRIPT_DIR)/ybytecode-script.c \
	$(SCRIPT_DIR)/s7-script.c \
	$(SCRIPT_DIR)/ph7-script.c \
	$(SCRIPT_DIR)/quickjs.c \
	$(SCRIPT_DIR)/script.c \
	$(BYTECODE_DIR)/ybytecode.c \
	$(BYTECODE_DIR)/condition.c \
	$(DESCRIPTION_DIR)/description.c \
	$(DESCRIPTION_DIR)/json-desc.c	 \
	$(DESCRIPTION_DIR)/rawfile-decs.c \
	$(ENTITY_DIR)/entity.c \
	$(ENTITY_DIR)/entity-string.c \
	$(ENTITY_DIR)/entity-convertions.c \
	$(ENTITY_DIR)/entity-patch.c \
	$(GAME_DIR)/game.c \
	$(UTIL_DIR)/math.c \
	$(UTIL_DIR)/util.c \
	$(UTIL_DIR)/simple-net.c \
	$(UTIL_DIR)/block-array.c \
	$(UTIL_DIR)/debug.c \
	$(WID_DIR)/widget.c \
	$(WID_DIR)/text-screen.c \
	$(WID_DIR)/menu.c \
	$(WID_DIR)/map.c \
	$(WID_DIR)/pos.c \
	$(WID_DIR)/rect.c \
	$(WID_DIR)/events.c \
	$(WID_DIR)/container.c \
	$(WID_DIR)/texture.c \
	$(WID_DIR)/canvas.c \
	$(SDL_WID_DIR)/sdl.c \
	$(SDL_WID_DIR)/map.c \
	$(SDL_WID_DIR)/menu.c \
	$(SDL_WID_DIR)/text-screen.c \
	$(SDL_WID_DIR)/canvas.c

SRC += $(SOUND_SRC)

O_SRC = $(SCRIPT_DIR)/s7.c ph7/ph7.c

O_OBJ = $(O_SRC:.c=.o)

SRCXX += 	$(ENTITY_DIR)/entity-cplusplus.cpp

CXX = $(CC)

OBJ =   $(SRC:.c=.o)
OBJXX = $(SRCXX:.cpp=.o)

QUICKJS_V = 2020-03-16

QUICKJS_PATH = quickjs-$(QUICKJS_V)
QUICKJS_LIB_PATH = $(QUICKJS_PATH)/libquickjs.a

GEN_LOADER_SRC = $(GEN_LOADER_DIR)/main.c
GEN_LOADER_OBJ = $(GEN_LOADER_SRC:.c=.o)

#../SDL_mixer/build/.libs/libSDL2_mixer-2.0.so.0.2.2
#SDL_MIXER_LDFLAGS = "/home/uso/SDL_mixer/build/.libs/libSDL2_mixer.a"
#SDL_MIXER_CFLAGS = "-I../SDL_mixer/"

LDFLAGS += $(TCC_LIB_PATH)$(TCC_LIB_NAME)
LDFLAGS += $(SDL_MIXER_LDFLAGS) $(SDL_MIXER_ARFLAGS) #  $(shell $(PKG_CONFIG) --libs SDL2_mixer)
LDFLAGS += $(SDL_GPU_LDFLAGS)
LDFLAGS += -L./
LDFLAGS += $(shell $(PKG_CONFIG) --libs glib-2.0)
LDFLAGS += $(LUA_LIB)
LDFLAGS += $(shell $(PKG_CONFIG) --libs json-c)
LDFLAGS += $(shell $(PKG_CONFIG) --libs SDL2_image SDL2_ttf $(WIN_SDL_EXTRA)) $(WIN_SDL_EXTRA2)
LDFLAGS += $(LDFLAGS_EXT)
LDFLAGS += $(LIBS_SAN) -ldl $(QUICKJS_LIB_PATH)
LDFLAGS += $(ANALYZER_FLAG)
LDFLAGS += $(shell $(PKG_CONFIG) --libs gl glu)

COMMON_CFLAGS += $(SDL_MIXER_CFLAGS)
COMMON_CFLAGS += $(shell $(PKG_CONFIG) --cflags glib-2.0)
COMMON_CFLAGS += $(shell sdl2-config --cflags)
COMMON_CFLAGS += -I$(YIRL_INCLUDE_PATH)
COMMON_CFLAGS += -I$(YIRL_INCLUDE_PATH2)
COMMON_CFLAGS += -I$(TCC_LIB_PATH)
COMMON_CFLAGS += -I./core/script/
COMMON_CFLAGS += -I./$(DUCK_V)/ -I./$(DUCK_V)/src/ # <--- last one is here so I can compile extras
COMMON_CFLAGS += -I./$(QUICKJS_PATH)
COMMON_CFLAGS += -fpic
COMMON_CFLAGS += $(LUA_CFLAGS)
COMMON_CFLAGS += -I$(SDL_GPU_CFLAGS)
COMMON_CFLAGS += $(WERROR) -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-int-to-pointer-cast

COMMON_CFLAGS += -DYIRL_INCLUDE_PATH=\"$(YIRL_INCLUDE_PATH2)\"
COMMON_CFLAGS += -DTCC_LIB_PATH=\"$(TCC_LIB_PATH)\"
COMMON_CFLAGS += $(FLAGS_SAN)
COMMON_CFLAGS += -Wno-unknown-warning-option
COMMON_CFLAGS += -Wno-cast-function-type
COMMON_CFLAGS += -fno-strict-aliasing # casting entity doesn't really respect strict aliasing rules
COMMON_CFLAGS += $(ANALYZER_FLAG)
COMMON_CFLAGS += -I./ph7/

CXXFLAGS = $(COMMON_CFLAGS) -x c++ -Wno-missing-exception-spec -fno-exceptions -fno-rtti -fpermissive

CFLAGS += $(COMMON_CFLAGS) -std=gnu11 -D_GNU_SOURCE

INSTALL_MOD=$(PREFIX)/share/yirl/modules/
SCRIPT_DEP=$(PREFIX)/share/yirl/scripts-dependancies/
#this one is here so my screen don't cur the install line
ULPCS=Universal-LPC-spritesheet/

sdl-gpu-build:
	cmake -B ./sdl-gpu-build ./sdl-gpu/  -DCMAKE_C_FLAGS="-fPIC"

$(SDL_GPU_LDFLAGS): sdl-gpu-build
	make -C sdl-gpu-build

$(QUICKJS_PATH):
	git clone https://github.com/cosmo-ray/quickjs.git quickjs-$(QUICKJS_V)

$(QUICKJS_LIB_PATH): $(QUICKJS_PATH)
	CONFIG_FPIC=1 make -C $(QUICKJS_PATH) libquickjs.a

ph7/ph7.o:
	$(CC) -c -o ph7/ph7.o ph7/ph7.c -I./ph7/ -O2 -g -fPIC

$(SCRIPT_DIR)/s7.o:
	$(CC) -c -o $(SCRIPT_DIR)/s7.o $(SCRIPT_DIR)/s7.c -Wno-implicit-fallthrough -fPIC -O2 -g

$(LIBNAME).a: $(OBJ) $(O_OBJ) $(OBJXX) $(QUICKJS_LIB_PATH) $(SDL_GPU_LDFLAGS)
	$(AR)  -r -c -s $(LIBNAME).a $(OBJ) $(O_OBJ) $(OBJXX) $(QUICKJS_LIB_PATH)
$(LIBNAME).$(LIBEXTENSION): $(OBJ) $(O_OBJ) $(OBJXX) $(QUICKJS_LIB_PATH) $(SDL_GPU_LDFLAGS)
	$(CC) -shared -o  $(LIBNAME).$(LIBEXTENSION) $(OBJ) $(O_OBJ) $(OBJXX) $(LDFLAGS)

yirl-loader: $(YIRL_LINKING) $(GEN_LOADER_OBJ)
	$(CC) -o yirl-loader$(BIN_EXT) $(GEN_LOADER_OBJ) $(BINARY_LINKING) $(LDFLAGS) 


clean:	clean-tests
	rm -rvf $(OBJ) $(OBJXX) $(GEN_LOADER_OBJ)

fclean: clean
	rm -rvf $(LIBNAME).a $(O_OBJ) $(LIBNAME).so $(LIBNAME).dll

clean_all: fclean
	rm -rvf $(DUCK_OBJ) $(QUICKJS_LIB_PATH)

install: yirl-loader
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)$(INSTALL_MOD)
	mkdir -p $(PREFIX)/bin
	cp yirl-loader $(PREFIX)/bin/
	cp libyirl.so $(PREFIX)/lib/
	mkdir -p $(INSTALL_MOD)/8086-emu/
	cp  modules/8086-emu/asm-inst.h $(INSTALL_MOD)/8086-emu/
	cp  modules/8086-emu/asm-tok.h $(INSTALL_MOD)/8086-emu/
	cp  modules/8086-emu/asm.c $(INSTALL_MOD)/8086-emu/
	cp  modules/8086-emu/start.c $(INSTALL_MOD)/8086-emu/
	cp  modules/8086-emu/charset.h $(INSTALL_MOD)/8086-emu/
	mkdir -p $(INSTALL_MOD)/dialogue/
	cp  modules/dialogue/init.c $(INSTALL_MOD)/dialogue/
	cp  modules/dialogue/start.json $(INSTALL_MOD)/dialogue/
	mkdir -p $(INSTALL_MOD)/dialogue-box/
	cp  modules/dialogue-box/arrow_sheet.png $(INSTALL_MOD)/dialogue-box/
	cp  modules/dialogue-box/init.lua $(INSTALL_MOD)/dialogue-box/
	cp  modules/dialogue-box/start.json $(INSTALL_MOD)/dialogue-box/
	mkdir -p $(SCRIPT_DEP)
	cp  scripts-dependancies/object-wrapper.lua $(SCRIPT_DEP)
	cp -rvf include $(PREFIX)/share/yirl/
	mkdir -p $(PREFIX)/share/yirl/tinycc/
	cp tinycc/libtcc1.a $(PREFIX)/share/yirl/tinycc/
	cp DejaVuSansMono.ttf $(PREFIX)/share/yirl/
	cp sazanami-mincho.ttf $(PREFIX)/share/yirl/
	install -D ./yirl-completion.bash $(PREFIX)/share/bash-completion/completions/yirl-loader
	echo "Install everything in: "$(PREFIX)

install_extra_modules:
	mkdir -p $(INSTALL_MOD)
	mkdir -p $(INSTALL_MOD)/c_app
	cp  modules/c_app/* $(INSTALL_MOD)/c_app/
	mkdir -p $(INSTALL_MOD)/snake
	cp  modules/snake/snake.lua $(INSTALL_MOD)/snake/snake.lua
	cp  modules/snake/start.json $(INSTALL_MOD)/snake/start.json
	cp  modules/snake/bg.png $(INSTALL_MOD)/snake/bg.png
	mkdir -p $(INSTALL_MOD)/hightscore/
	cp modules/hightscore/score.lua $(INSTALL_MOD)/hightscore/
	cp modules/hightscore/start.json $(INSTALL_MOD)/hightscore/
	mkdir -p $(INSTALL_MOD)/jrpg-fight/
	cp modules/jrpg-fight/init.lua $(INSTALL_MOD)/jrpg-fight/
	cp modules/jrpg-fight/animation.lua $(INSTALL_MOD)/jrpg-fight/
	cp modules/jrpg-fight/start.json $(INSTALL_MOD)/jrpg-fight/
	cp modules/jrpg-fight/image0007.png $(INSTALL_MOD)/jrpg-fight/
	cp modules/jrpg-fight/image0009.png $(INSTALL_MOD)/jrpg-fight/
	cp modules/jrpg-fight/explosion.png $(INSTALL_MOD)/jrpg-fight/
	cp modules/jrpg-fight/BG_City.jpg $(INSTALL_MOD)/jrpg-fight/
	cp modules/jrpg-fight/README.md $(INSTALL_MOD)/jrpg-fight/
	mkdir -p $(INSTALL_MOD)/sprite-manager/
	cp modules/sprite-manager/start.c $(INSTALL_MOD)/sprite-manager/
	mkdir -p $(INSTALL_MOD)/loading_bar/
	cp modules/loading_bar/start.c $(INSTALL_MOD)/loading_bar/
	mkdir -p $(INSTALL_MOD)/$(ULPCS)/
	cp modules/$(ULPCS)/start.json $(INSTALL_MOD)/$(ULPCS)/
	cp modules/$(ULPCS)/lpcs.lua $(INSTALL_MOD)/$(ULPCS)/
	cp -rvf modules/$(ULPCS)/spritesheets/ $(INSTALL_MOD)/$(ULPCS)/
	cp modules/$(ULPCS)/CREDITS.TXT $(INSTALL_MOD)/$(ULPCS)/
	cp modules/$(ULPCS)/cc-by-sa-3_0.txt $(INSTALL_MOD)/$(ULPCS)/
	mkdir -p $(INSTALL_MOD)/pong/
	cp modules/pong/start.scm $(INSTALL_MOD)/pong/
	mkdir -p $(INSTALL_MOD)/tiled/
	cp modules/tiled/tiled.c $(INSTALL_MOD)/tiled/
	cp modules/tiled/start.json $(INSTALL_MOD)/tiled/
	mkdir -p $(INSTALL_MOD)/vapp/
	cp 'modules/vapp/New Piskel.png' $(INSTALL_MOD)/vapp/
	cp modules/vapp/viking.png $(INSTALL_MOD)/vapp/
	cp modules/vapp/start.json $(INSTALL_MOD)/vapp/
	cp modules/vapp/pizza.png $(INSTALL_MOD)/vapp/
	cp modules/vapp/init.c $(INSTALL_MOD)/vapp/
	cp modules/vapp/resources.json $(INSTALL_MOD)/vapp/
	mkdir -p $(INSTALL_MOD)/shooter/
	cp modules/shooter/DurrrSpaceShip.png $(INSTALL_MOD)/shooter/
	cp modules/shooter/start.lua $(INSTALL_MOD)/shooter/
	cp -rvf modules/shooter/jswars_gfx/ $(INSTALL_MOD)/shooter/
	mkdir -p $(INSTALL_MOD)/asteroide-shooter/
	cp modules/asteroide-shooter/start.json $(INSTALL_MOD)/asteroide-shooter/

.PHONY : install clean_all fclean clean all install_extra_modules build-tests clean-tests
