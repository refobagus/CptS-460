#include "ucode.c"

char buf[1024];

int main(int argc, char * argv[])
{
	if(argc < 3)
	{
		printf("Usage: cp src dest\n");
		return 0;
	}
	//open files
	close(0);
	open(argv[1], O_RDONLY);
	close(1);
	//if file not found, create
	open(argv[2], O_WRONLY|O_CREAT);

	//only overwrites or create new
	while(read(0, buf, 1024))
	{
		buf[strlen(buf) - 1] = 0;
		prints(buf);
	}

	return 0;
}
