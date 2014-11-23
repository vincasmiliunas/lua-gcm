TARGET = gcm.so
PKG_NAME = lua-gcm
CFLAGS += -O2 -fPIC
LDFLAGS += -shared

ifdef OPENWRT_BUILD
	LDADD += -l:libpolarssl.a
else
	LDFLAGS += -lpolarssl
endif

all: $(TARGET)

$(TARGET): $(PKG_NAME).o
	$(CC) $(LDFLAGS) $< -o $@ $(LDADD)

$(PKG_NAME).o: $(PKG_NAME).c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.so

test:
	lua test.lua
