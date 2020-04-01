#include "ucode.c"

char buf[1024];

void upper(char * c)
{
	int len = strlen(c);
	for(int i = 0; i < len; i++)
		//if lowercase, change to upper, else leave it be
		c[i] = (c[i] >= 'a' && c[i] <= 'z') ? c[i] + 'A' - 'a' : c[i];
}

int main(int argc, char * argv[])
{
	if(argc == 3)
	{
		//open file and create new to swap as uppercase
		close(0);
		open(argv[1], O_RDONLY);
		close(1);
		open(argv[2], O_WRONLY|O_CREAT);
	}

	while(getline(buf))
	{
		//change upper per line
		upper(buf);
		prints(buf);
	}
		
}
