CFLAGS+=-pedantic
LDFLAGS+=-lm -lglfw -lGL
OBJ=bmp.o camera.o color.o glfw.o main.o material.o plane.o scene.o sphere.o vector3.o

all: raytracer

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

raytracer: ${OBJ}
	@echo LD $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@rm -f ${OBJ}
	@rm -f raytracer
	@rm -f scene.bmp
