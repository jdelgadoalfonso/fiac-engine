# Nombre del proyecto
PROYECTO := animacion

# Modo de compilación, por defecto release
ifeq ($(modo), debug)
   CXXFLAGS += -g -D_DEBUG
else
   modo := release
   CXXFLAGS += -O2 -D_RELEASE
endif

# Directorios
OBJDIR := obj
SRCDIR := src
BINDIR := $(modo)

#Compilador y flags
CC := gcc
CXX := g++
CXXFLAGS := -std=c++0x -Wall -fexceptions

#Fichero de dependencias
DEPFILE := dependencias

# Flags del enlazador
LDFLAGS := -lpthread -L/bin/Debug -L/bin/Release -s
LDFLAGS += -lv8 -lOgreMain -lOIS -lOgreTerrain

# Colores para la salida de consola
COLOR_FIN := \033[00m
COLOR_OK := \033[01;32m
COLOR_ERROR := \033[01;31m
COLOR_AVISO := \033[01;33m
COLOR_COMP := \033[01;34m
COLOR_ENL := \033[01;35m

# Seleccionamos ficheros fuente
SRCS := $(notdir $(shell du -a $(SRCDIR) | cut -f 2 | grep .cpp))
DIRSRCS := `du -a $(SRCDIR) | cut -f 2 | grep .cpp`
OBJS := $(addprefix $(OBJDIR)/, $(addsuffix .o,$(basename $(SRCS))))

.PHONY:all
all: informacion $(PROYECTO)

# Comprobamos el modo de compilación
informacion:
ifneq ($(modo),release)
ifneq ($(modo),debug)
	@echo ''
	@echo -e '$(COLOR_ERROR)Modo de compilación inválido.$(COLOR_FIN)'
	@echo "Por favor usa 'make [modo=release|debug]'"
	@echo ''
	@exit 1
endif
endif
	@echo ''
	@echo -e 'Compilando en modo $(COLOR_AVISO)"$(modo)"$(COLOR_FIN)'
	@echo '..........................'
	@echo ''

$(DEPFILE):
	@echo ''
	@echo -e '$(COLOR_AVISO)Generando dependencias$(COLOR_FIN)...'
	@$(CXX) -MM $(DIRSRCS) | sed 's/^\([a-zA-Z]\+.o:\)/$(OBJDIR)\/\1/g' > $(DEPFILE)
	@echo ''

-include $(DEPFILE)
	
# Compilación
$(OBJS): $(echo $(DIRSRCS) | grep -e '$(basename $(OBJS)'))
	@echo ''
	@echo -e '$(COLOR_COMP)Compilando$(COLOR_FIN)... $(notdir $<)'
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
# Enlazado
$(PROYECTO): $(OBJS)
	@echo ''
	@echo -e '$(COLOR_ENL)Enlazando$(COLOR_FIN)...'
	@echo ''
	$(CXX) -o $(BINDIR)/$(PROYECTO) $(OBJS) $(LDFLAGS) $(LIBS) 
	@echo ''
	@echo -e '$(COLOR_OK)Terminado.$(COLOR_FIN)'
	@echo ''

# Limpiado del directorio
.PHONY:clean
clean:
	@echo ''
	@echo -e '$(COLOR_AVISO)Limpiando$(COLOR_FIN)...'
	@echo ''
	rm $(OBJS) $(BINDIR)/$(PROYECTO) $(DEPFILE) $(OBJDIR) *~ -rf
	@echo ''
	@echo -e '$(COLOR_OK)Terminado.$(COLOR_FIN)'
	@echo ''
