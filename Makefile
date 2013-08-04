LDFLAGS+=-lm
OBJ=bmp.o color.o main.o vector3.o

all: raytracer

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

raytracer: ${OBJ}
	@echo LD $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f ${OBJ}
	rm -f raytracer
	rm -f scene.bmp
