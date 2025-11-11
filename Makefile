include config.mk

# Sources
SRC = shell.c \
	  cmd.c \
	  calc.c \
	  help.c \
	  date.c \
	  global.c \
	  hostname.c \
	  whoami.c \
	  credit.c \
	  chat.c \
	  register.c \
	  login.c \
	  users.c \
	  huser.c
OBJ = ${SRC:.c=.o}

# Library sources
LIBSRC = lib/tinyexpr.c lib/compatibility.c lib/util.c
LIBOBJ = ${LIBSRC:.c=.o}

# All objects
OBJ += ${LIBOBJ}

TARGET = mutinyos
BINDIR ?= /usr/bin
BASE_DIR ?= /var/tmp/mutiny

# Default target
all: $(TARGET)

# Compile object files
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

lib/%.o: lib/%.c
	$(CC) -c $(CFLAGS) $< -o $@

${OBJ}: config.h config.mk

# Link target
$(TARGET): ${OBJ}
	$(CC) -o $@ ${OBJ} $(LDFLAGS)

# Clean
clean:
	rm -f $(OBJ) $(TARGET) shell-${VERSION}.tar.gz mutinyos-${VERSION}.tar.gz

# Run
run: $(TARGET)
	./$(TARGET)

# Install: create directories and copy binary
install: $(TARGET)
	@echo "Creating directories under $(BASE_DIR)..."
	@for d in bin etc home root; do \
		mkdir -p "$(BASE_DIR)/$$d"; \
		done
	@touch "$(BASE_DIR)/etc/users"
	@echo "root" > "$(BASE_DIR)/etc/users"
	@echo "Copying $(TARGET) to $(BINDIR)..."
	@cp -f ./$(TARGET) $(BINDIR)/
	@chmod 755 $(BINDIR)/$(TARGET)
	@echo "Changing permission..."
	@echo "Making $(BASE_DIR) world-accessible..."
	@chmod -R 777 $(BASE_DIR)
	@echo "Installation complete."

# Uninstall: remove directories and binary
uninstall:
	@echo "WARNING: This will remove $(BASE_DIR) and $(BINDIR)/$(TARGET)!"
	@read -p "Are you sure? [y/N]: " confirm; \
		case "$$confirm" in \
		[yY]|[yY][eE][sS]) \
		echo "Removing $(BASE_DIR)..."; \
		rm -rf $(BASE_DIR); \
		echo "Removing $(BINDIR)/$(TARGET)..."; \
		rm -f $(BINDIR)/$(TARGET); \
		echo "Uninstalled."; \
		;; \
		*) \
		echo "Aborted."; \
		;; \
		esac

readme:
	./scc-hook.readme.sh

.PHONY: all clean run install uninstall readme
