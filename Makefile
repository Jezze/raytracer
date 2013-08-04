all:
	gcc main.c -lm -o raytracer

clean:
	rm -f raytracer
	rm -f scene.bmp
